#include "cardcache.h"

#include "defs.h"
#include "format.h"
#include "http.h"
#include "logger.h"
#include "util.h"

#include <memory>

#include "cJSON.h"

#include "esp_log.h"

constexpr util::duration MAX_CACHE_AGE = std::chrono::minutes(15);

Card_cache& Card_cache::instance()
{
    static Card_cache the_instance;
    return the_instance;
}

void Card_cache::set_api_token(const std::string& token)
{
    api_token = token;
}

Card_cache::Result Card_cache::has_access(Card_cache::Card_id id)
{
    User_info ui;
    bool found = false;
    {
        std::lock_guard<std::mutex> g(cache_mutex);
        const auto it = cache.find(id);
        if (it != cache.end())
        {
            found = true;
            ui = it->second;
        }
    }
    if (found)
    {
        if (util::now() - ui.last_update < MAX_CACHE_AGE)
        {
            Logger::instance().log(format(CARD_ID_FORMAT " cached", id));
            if (ui.allowed)
            {
                Logger::instance().log_backend(ui.user_id, "Granted access");
                return Result(Access::Allowed, ui.user_int_id, "",
                              ui.user_name);
            }
            Logger::instance().log_backend(ui.user_id, "Denied access");
            return Result(Access::Forbidden, ui.user_int_id, "",
                          ui.user_name);
        }
        Logger::instance().log(format(CARD_ID_FORMAT ": stale", id));
        // Cache entry is outdated
        return Result(Access::Error, 0, "", "");
    }
    return Result(Access::Unknown, 0, "", "");
}

Card_cache::Card_id Card_cache::get_id_from_string(const std::string& s)
{
    std::istringstream is(s);
    Card_cache::Card_id id = 0;
    is >> std::hex >> id;
    return id;
}

void Card_cache::thread_body()
{
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    while (1)
    {
        if (api_token.empty())
        {
            ESP_LOGE(TAG, "Card_cache: no API token");
            vTaskDelay(60000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "Update card cache");

        // Fetch card info
        constexpr int HTTP_MAX_OUTPUT = 10*1024;
        auto buffer = std::unique_ptr<char[]>(new (std::nothrow) char[HTTP_MAX_OUTPUT+1]);
        if (!buffer)
        {
            ESP_LOGE(TAG, "Card_cache: Could not allocate %d bytes", HTTP_MAX_OUTPUT+1);
            continue;
        }
        Http_data http_data;
        http_data.buffer = buffer.get();
        http_data.max_output = HTTP_MAX_OUTPUT;
        esp_http_client_config_t config {
            .host = "panopticon.hal9k.dk",
            .path = "/api/v3/permissions/",
            .event_handler = http_event_handler,
            .transport_type = HTTP_TRANSPORT_OVER_SSL,
            .user_data = &http_data,
            .crt_bundle_attach = esp_crt_bundle_attach,
        };
        ESP_LOGI(TAG, "cardcache update begin");
        size_t size = 0;
        {
            std::lock_guard<std::mutex> g(http_mutex);
            esp_http_client_handle_t client = esp_http_client_init(&config);
            Http_client_wrapper w(client);

            ESP_ERROR_CHECK(esp_http_client_set_method(client, HTTP_METHOD_GET));

            const char* content_type = "application/json";
            ESP_ERROR_CHECK(esp_http_client_set_header(client, "Accept", content_type));
            ESP_ERROR_CHECK(esp_http_client_set_header(client, "Content-Type", content_type));
            const std::string auth = std::string("Token ") + std::string(api_token);
            ESP_ERROR_CHECK(esp_http_client_set_header(client, "Authorization", auth.c_str()));
            esp_err_t err = esp_http_client_perform(client);
            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "/v3/permissions: error %s", esp_err_to_name(err));
                continue;
            }
            const auto code = esp_http_client_get_status_code(client);
            if (code != 200)
            {
                ESP_LOGE(TAG, "Error: Unexpected response from /v3/permissions: %d", code);
                Logger::instance().log(format("Error: Unexpected response from /v3/permissions: %d", code));
                continue;
            }
            auto root = cJSON_Parse(buffer.get());
            cJSON_wrapper jw(root);
            if (!root)
            {
                ESP_LOGE(TAG, "Error: Bad JSON from /v3/permissions: %s", buffer.get());
                Logger::instance().log(format("Error: Bad JSON from /v3/permissions"));
                continue;
            }
            if (!cJSON_IsArray(root))
            {
                ESP_LOGE(TAG, "Error: Response from /v3/permissions is not an array");
                Logger::instance().log("Error: Response from /v3/permissions is not an array");
                continue;
            }
            // Create new cache
            Cache new_cache;
            cJSON* it;
            cJSON_ArrayForEach(it, root)
            {
                if (!cJSON_IsObject(it))
                {
                    ESP_LOGE(TAG, "Error: Item from /v3/permissions is not an object");
                    Logger::instance().log("Error: Item from /v3/permissions is not an object");
                    continue;
                }
                auto card_id_node = cJSON_GetObjectItem(it, "card_id");
                if (!cJSON_IsString(card_id_node))
                {
                    ESP_LOGE(TAG, "Error: Item from /v3/permissions has no card_id");
                    Logger::instance().log("Error: Item from /v3/permissions has no card_id");
                    continue;
                }
                auto id_node = cJSON_GetObjectItem(it, "id");
                if (!cJSON_IsNumber(id_node))
                {
                    ESP_LOGE(TAG, "Error: Item from /v3/permissions has no id");
                    Logger::instance().log("Error: Item from /v3/permissions has no id");
                    continue;
                }
                auto int_id_node = cJSON_GetObjectItem(it, "int_id");
                if (!cJSON_IsNumber(int_id_node))
                {
                    ESP_LOGE(TAG, "Error: Item from /v3/permissions has no int_id");
                    Logger::instance().log("Error: Item from /v3/permissions has no int_id");
                    continue;
                }
                auto name_node = cJSON_GetObjectItem(it, "name");
                if (!cJSON_IsString(name_node))
                {
                    ESP_LOGE(TAG, "Error: Item from /v3/permissions has no name");
                    Logger::instance().log("Error: Item from /v3/permissions has no name");
                    continue;
                }
                auto allowed_node = cJSON_GetObjectItem(it, "allowed");
                if (!cJSON_IsBool(allowed_node))
                {
                    ESP_LOGE(TAG, "Error: Item from /v3/permissions has no allowed");
                    Logger::instance().log("Error: Item from /v3/permissions has no allowed");
                    continue;
                }
                const auto card_id = get_id_from_string(card_id_node->valuestring);
                const auto id = id_node->valueint;
                const auto int_id = int_id_node->valueint;
                const std::string user_name = name_node->valuestring;
                const bool allowed = cJSON_IsTrue(allowed_node);
                new_cache[card_id] = { id, int_id, user_name,
                                       allowed, util::now() };
            }
            // Store
            size = new_cache.size();
            {
                std::lock_guard<std::mutex> g(cache_mutex);
                cache.swap(new_cache);
            }
            ESP_LOGI(TAG, "cardcache update end");
        }
        Logger::instance().log(format("Card cache updated: %d cards", static_cast<int>(size)));

        vTaskDelay(5*60*1000 / portTICK_PERIOD_MS);
    }
}

void card_cache_task(void*)
{
    Card_cache::instance().thread_body();
}

// Local Variables:
// compile-command: "cd .. && idf.py build"
// End:
