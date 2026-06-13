#include "logger.h"

#include "defs.h"
#include "nvs.h"
#include "http.h"
#include "util.h"

#include "cJSON.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_netif.h"

#include <string.h>

static constexpr const char* TAG = "log";

Logger& Logger::instance()
{
    static Logger the_instance;
    return the_instance;
}

void Logger::set_api_token(const std::string& token)
{
    api_token = token;
}

time_t Logger::make_timestamp(char* stamp, bool include_date)
{
    time_t current = 0;
    time(&current);
    make_timestamp(current, stamp, include_date);
    return current;
}

void Logger::make_timestamp(time_t t, char* stamp, bool include_date)
{
    struct tm timeinfo;
    gmtime_r(&t, &timeinfo);
    strftime(stamp, TIMESTAMP_SIZE,
             include_date ? "%Y-%m-%d %H:%M:%S" : "%H:%M:%S",
             &timeinfo);
}

void Logger::log_backend(int user_id, const std::string& s)
{
    char stamp[Logger::TIMESTAMP_SIZE];
    make_timestamp(stamp);

    Item item{ Item::Type::Backend, user_id };
    strncpy(item.stamp, stamp, std::min<size_t>(Item::STAMP_SIZE, strlen(stamp)));
    strncpy(item.text, s.c_str(), std::min<size_t>(Item::MAX_SIZE, s.size()));
    std::lock_guard<std::mutex> g(mutex);
    if (q.size() > 100)
    {
        ESP_LOGE(TAG, "Logger: Queue overflow");
        return;
    }
    q.push_front(item);
}

void Logger::log_unknown_card(Card_id card_id)
{
    Item item{ Item::Type::Unknown_card };
    sprintf(item.text, CARD_ID_FORMAT, card_id);
    std::lock_guard<std::mutex> g(mutex);
    if (q.size() > 100)
    {
        ESP_LOGE(TAG, "Logger: Queue overflow");
        return;
    }
    q.push_front(item);
}

void Logger::thread_body()
{
    ESP_LOGI(TAG, "Logger thread started");
    Item item;
    while (1)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);

        /*
        static int count = 0;
        if (++count > 1000)
        {
            count = 0;
            ESP_LOGI(TAG, "Logger thread running");
        }
        */
        if (q.empty())
            continue;
        item = q.back();
        q.pop_back();
        // ESP_LOGI(TAG, "Got log item");

        switch (item.type)
        {
        case Item::Type::Backend:
            {
                if (api_token.empty())
                    break;

                esp_http_client_config_t config {
                    .host = "panopticon.hal9k.dk",
                    .path = "/api/v1/logs",
                    .event_handler = http_event_handler,
                    .transport_type = HTTP_TRANSPORT_OVER_SSL,
                    .crt_bundle_attach = esp_crt_bundle_attach,
                };
                //ESP_LOGI(TAG, "Backend get mutex");
                std::lock_guard<std::mutex> g(http_mutex);
                esp_http_client_handle_t client = esp_http_client_init(&config);
                Http_client_wrapper w(client);

                esp_http_client_set_method(client, HTTP_METHOD_POST);
                auto payload = cJSON_CreateObject();
                cJSON_wrapper jw(payload);
                auto jtoken = cJSON_CreateString(api_token.c_str());
                cJSON_AddItemToObject(payload, "api_token", jtoken);
                auto log = cJSON_CreateObject();
                auto user = cJSON_CreateNumber(item.user_id);
                cJSON_AddItemToObject(log, "user_id", user);
                auto text = cJSON_CreateString(item.text);
                cJSON_AddItemToObject(log, "message", text);
                cJSON_AddItemToObject(payload, "log", log);

                char* data = cJSON_Print(payload);
                if (!data)
                {
                    ESP_LOGE(TAG, "Logger: cJSON_Print() returned nullptr");
                    break;
                }
                cJSON_Print_wrapper pw(data);
                esp_http_client_set_post_field(client, data, strlen(data));

                const char* content_type = "application/json";
                esp_http_client_set_header(client, "Content-Type", content_type);
                esp_err_t err = esp_http_client_perform(client);

                if (err == ESP_OK)
                    ESP_LOGI(TAG, "logs: HTTP %d", esp_http_client_get_status_code(client));
                else
                    ESP_LOGE(TAG, "logs: error %s", esp_err_to_name(err));
            }
            break;

        case Item::Type::Unknown_card:
            {
                if (api_token.empty())
                    break;

                esp_http_client_config_t config {
                    .host = "panopticon.hal9k.dk",
                    .path = "/api/v1/unknown_cards",
                    .event_handler = http_event_handler,
                    .transport_type = HTTP_TRANSPORT_OVER_SSL,
                    .crt_bundle_attach = esp_crt_bundle_attach,
                };
                //ESP_LOGI(TAG, "unknown_cards get mutex");
                std::lock_guard<std::mutex> g(http_mutex);
                esp_http_client_handle_t client = esp_http_client_init(&config);
                Http_client_wrapper w(client);

                esp_http_client_set_method(client, HTTP_METHOD_POST);
                auto payload = cJSON_CreateObject();
                cJSON_wrapper jw(payload);
                auto jtoken = cJSON_CreateString(api_token.c_str());
                cJSON_AddItemToObject(payload, "api_token", jtoken);
                auto text = cJSON_CreateString(item.text);
                cJSON_AddItemToObject(payload, "card_id", text);

                char* data = cJSON_Print(payload);
                if (!data)
                {
                    ESP_LOGE(TAG, "Logger: cJSON_Print() returned nullptr");
                    break;
                }
                cJSON_Print_wrapper pw(data);
                esp_http_client_set_post_field(client, data, strlen(data));

                const char* content_type = "application/json";
                esp_http_client_set_header(client, "Content-Type", content_type);
                esp_err_t err = esp_http_client_perform(client);

                if (err == ESP_OK)
                    ESP_LOGI(TAG, "unknown_cards: HTTP %d", esp_http_client_get_status_code(client));
                else
                    ESP_LOGE(TAG, "unknown_cards: error %s", esp_err_to_name(err));
            }
            break;
        }
    }
}

void logger_task(void*)
{
    Logger::instance().thread_body();
}

// Local Variables:
// compile-command: "cd .. && idf.py build"
// End:
