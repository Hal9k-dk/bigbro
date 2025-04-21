#include "nvs.h"

#include "defs.h"

#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_spiffs.h>
#include <esp_vfs.h>
#include <nvs_flash.h>

static char identifier[20];
static char acs_token[80];
static char gateway_token[80];
static char slack_token[80];
static wifi_creds_t wifi_creds;
static bool current_sense_enabled;

void clear_wifi_credentials()
{
    nvs_handle my_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &my_handle));
    ESP_ERROR_CHECK(nvs_set_str(my_handle, WIFI_KEY, ""));
    ESP_ERROR_CHECK(nvs_commit(my_handle));
    nvs_close(my_handle);
    wifi_creds.clear();
}

void add_wifi_credentials(const char* ssid, const char* password)
{
    nvs_handle my_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &my_handle));
    std::string creds;
    char buf[256];
    auto size = sizeof(buf);
    if (nvs_get_str(my_handle, WIFI_KEY, buf, &size) == ESP_OK)
        creds = std::string(buf);
    creds += std::string(ssid) + std::string(":") + std::string(password) + std::string(":");
    ESP_ERROR_CHECK(nvs_set_str(my_handle, WIFI_KEY, creds.c_str()));
    ESP_ERROR_CHECK(nvs_commit(my_handle));
    nvs_close(my_handle);
}

void set_identifier(const char* identifier_arg)
{
    nvs_handle my_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &my_handle));
    ESP_ERROR_CHECK(nvs_set_str(my_handle, IDENTIFIER_KEY, identifier_arg));
    ESP_ERROR_CHECK(nvs_commit(my_handle));
    nvs_close(my_handle);
    strcpy(identifier, identifier_arg);
}

void set_acs_token(const char* token)
{
    nvs_handle my_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &my_handle));
    ESP_ERROR_CHECK(nvs_set_str(my_handle, ACS_TOKEN_KEY, token));
    ESP_ERROR_CHECK(nvs_commit(my_handle));
    nvs_close(my_handle);
}

void set_gateway_token(const char* token)
{
    nvs_handle my_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &my_handle));
    ESP_ERROR_CHECK(nvs_set_str(my_handle, GATEWAY_TOKEN_KEY, token));
    ESP_ERROR_CHECK(nvs_commit(my_handle));
    nvs_close(my_handle);
}

void set_slack_token(const char* token)
{
    nvs_handle my_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &my_handle));
    ESP_ERROR_CHECK(nvs_set_str(my_handle, SLACK_TOKEN_KEY, token));
    ESP_ERROR_CHECK(nvs_commit(my_handle));
    nvs_close(my_handle);
}

void set_current_sense_enabled(bool enabled)
{
    nvs_handle my_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &my_handle));
    ESP_ERROR_CHECK(nvs_set_u8(my_handle, CUR_SENSE_KEY, static_cast<uint8_t>(enabled)));
    ESP_ERROR_CHECK(nvs_commit(my_handle));
    nvs_close(my_handle);
}

bool get_nvs_string(nvs_handle my_handle, const char* key, char* buf, size_t buf_size)
{
    auto err = nvs_get_str(my_handle, key, buf, &buf_size);
    switch (err)
    {
    case ESP_OK:
        return true;
    case ESP_ERR_NVS_NOT_FOUND:
        printf("%s: not found\n", key);
        break;
    default:
        printf("%s: NVS error %d\n", key, err);
        break;
    }
    return false;
}

std::vector<std::pair<std::string, std::string>> parse_wifi_credentials(char* buf)
{
    std::vector<std::pair<std::string, std::string>> v;
    bool is_ssid = true;
    std::string ssid;
    char* p = buf;
    while (1)
    {
        char* token = strsep(&p, ":");
        if (!token)
            break;
        if (is_ssid)
            ssid = std::string(token);
        else
            v.push_back(std::make_pair(ssid, std::string(token)));
        is_ssid = !is_ssid;
    }
    return v;
}

std::string get_acs_token()
{
    return acs_token;
}

std::string get_gateway_token()
{
    return gateway_token;
}

std::string get_identifier()
{
    if (identifier[0])
        return identifier;
    return "[not set]";
}

std::string get_slack_token()
{
    return slack_token;
}

wifi_creds_t get_wifi_creds()
{
    return wifi_creds;
}

bool get_current_sense_enabled()
{
    return current_sense_enabled;
}

void init_nvs()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    nvs_handle my_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &my_handle));
    char buf[256];
    if (!get_nvs_string(my_handle, IDENTIFIER_KEY, identifier, sizeof(identifier)))
        identifier[0] = 0;
    if (get_nvs_string(my_handle, WIFI_KEY, buf, sizeof(buf)))
        wifi_creds = parse_wifi_credentials(buf);
    if (!get_nvs_string(my_handle, ACS_TOKEN_KEY, acs_token, sizeof(acs_token)))
        acs_token[0] = 0;
    if (!get_nvs_string(my_handle, GATEWAY_TOKEN_KEY, gateway_token, sizeof(gateway_token)))
        gateway_token[0] = 0;
    if (!get_nvs_string(my_handle, SLACK_TOKEN_KEY, slack_token, sizeof(slack_token)))
        slack_token[0] = 0;
    uint8_t value = 0;
    if (nvs_get_u8(my_handle, CUR_SENSE_KEY, &value) != ESP_OK)
    {
        ESP_LOGW(TAG, "NVS value %s not found", CUR_SENSE_KEY);
        current_sense_enabled = false;
    }
    else
        current_sense_enabled = value;
    ESP_LOGI(TAG, "Current sense: %d", current_sense_enabled);
    nvs_close(my_handle);
}

void mount_spiffs(const char* path, const char* label, size_t max_files)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = path,
        .partition_label = label,
        .max_files = max_files,
        .format_if_mount_failed = true
    };

    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));

    size_t total = 0, used = 0;
    ESP_ERROR_CHECK(esp_spiffs_info(conf.partition_label, &total, &used));
    ESP_LOGI(TAG, "Mounted %s to %s", path, label);
    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
}

void list_spiffs(const char* path)
{
    DIR* dir = opendir(path);
    assert(dir != NULL);
    while (true)
    {
        struct dirent*pe = readdir(dir);
        if (!pe)
            break;
        ESP_LOGI(__FUNCTION__,
                 "d_name=%s d_ino=%d d_type=%x", pe->d_name,
                 pe->d_ino, pe->d_type);
    }
    closedir(dir);
}

// Local Variables:
// compile-command: "(cd ..; idf.py build)"
// End:
