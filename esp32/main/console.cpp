#include "console.h"
#include "defs.h"

#include <string>

#include "esp_system.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"

#include <driver/uart.h>
#include <nvs.h>
#include <nvs_flash.h>

#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"

struct
{
    struct arg_str* ssid;
    struct arg_str* password;
    struct arg_end* end;
} add_wifi_credentials_args;

int add_wifi_credentials(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &add_wifi_credentials_args);
    if (nerrors != 0)
    {
        arg_print_errors(stderr, add_wifi_credentials_args.end, argv[0]);
        return 1;
    }
    const auto ssid = add_wifi_credentials_args.ssid->sval[0];
    const auto password = add_wifi_credentials_args.password->sval[0];
    if (strlen(ssid) < 1)
    {
        printf("ERROR: Invalid SSID value\n");
        return 1;
    }
    nvs_handle my_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &my_handle));
    std::string creds;
    char buf[256];
    auto size = sizeof(buf);
    if (nvs_get_str(my_handle, WIFI_KEY, buf, &size) == ESP_OK)
    {
        creds = std::string(buf);
        if (!creds.empty())
            creds += std::string(":");
    }
    creds += std::string(ssid) + std::string(":") + std::string(password);
    ESP_ERROR_CHECK(nvs_set_str(my_handle, WIFI_KEY, creds.c_str()));
    nvs_close(my_handle);
    printf("OK: Added WiFi credentials %s/%s\n", ssid, password);
    return 0;
}

int clear_wifi_credentials(int, char**)
{
    nvs_handle my_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &my_handle));
    ESP_ERROR_CHECK(nvs_set_str(my_handle, WIFI_KEY, ""));
    nvs_close(my_handle);
    printf("OK: WiFi credentials cleared\n");
    return 0;
}

struct
{
    struct arg_str* token;
    struct arg_end* end;
} set_acs_credentials_args;

int set_acs_credentials(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &set_acs_credentials_args);
    if (nerrors != 0)
    {
        arg_print_errors(stderr, set_acs_credentials_args.end, argv[0]);
        return 1;
    }
    const auto token = set_acs_credentials_args.token->sval[0];
    if (strlen(token) < 32)
    {
        printf("ERROR: Invalid token\n");
        return 1;
    }
    nvs_handle my_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &my_handle));
    ESP_ERROR_CHECK(nvs_set_str(my_handle, ACS_TOKEN_KEY, token));
    nvs_close(my_handle);
    printf("OK: ACS token set to %s\n", token);
    return 0;
}

struct
{
    struct arg_str* instance;
    struct arg_end* end;
} set_instance_args;

static int set_instance(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &set_instance_args);
    if (nerrors != 0)
    {
        arg_print_errors(stderr, set_instance_args.end, argv[0]);
        return 1;
    }
    const auto instance = set_instance_args.instance->sval[0];
    if (strlen(instance) < 2)
    {
        printf("ERROR: Invalid instance\n");
        return 1;
    }
    nvs_handle my_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &my_handle));
    ESP_ERROR_CHECK(nvs_set_str(my_handle, INSTANCE_KEY, instance));
    nvs_close(my_handle);
    printf("OK: Instance set to %s\n", instance);
    return 0;
}

void initialize_console()
{
    /* Disable buffering on stdin */
    setvbuf(stdin, NULL, _IONBF, 0);

    /* Minicom, screen, idf_monitor send CR when ENTER key is pressed */
    esp_vfs_dev_uart_port_set_rx_line_endings(0, ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    esp_vfs_dev_uart_port_set_tx_line_endings(0, ESP_LINE_ENDINGS_CRLF);

    /* Configure UART. Note that REF_TICK is used so that the baud rate remains
     * correct while APB frequency is changing in light sleep mode.
     */
    uart_config_t uart_config;
    memset(&uart_config, 0, sizeof(uart_config));
    uart_config.baud_rate = CONFIG_ESP_CONSOLE_UART_BAUDRATE;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.source_clk = UART_SCLK_REF_TICK;
    ESP_ERROR_CHECK(uart_param_config((uart_port_t) CONFIG_ESP_CONSOLE_UART_NUM, &uart_config));

    /* Install UART driver for interrupt-driven reads and writes */
    ESP_ERROR_CHECK(uart_driver_install((uart_port_t) CONFIG_ESP_CONSOLE_UART_NUM,
                                         256, 0, 0, NULL, 0));

    /* Tell VFS to use UART driver */
    esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);

    /* Initialize the console */
    esp_console_config_t console_config;
    memset(&console_config, 0, sizeof(console_config));
    console_config.max_cmdline_args = 8;
    console_config.max_cmdline_length = 256;
#if CONFIG_LOG_COLORS
    console_config.hint_color = atoi(LOG_COLOR_CYAN);
#endif
    ESP_ERROR_CHECK(esp_console_init(&console_config));

    /* Configure linenoise line completion library */
    /* Enable multiline editing. If not set, long commands will scroll within
     * single line.
     */
    linenoiseSetMultiLine(1);

    /* Tell linenoise where to get command completions and hints */
    linenoiseSetCompletionCallback(&esp_console_get_completion);
    linenoiseSetHintsCallback((linenoiseHintsCallback*) &esp_console_get_hint);

    /* Set command history size */
    linenoiseHistorySetMaxLen(100);
}

void run_console()
{
    initialize_console();

    esp_console_register_help_command();

    add_wifi_credentials_args.ssid = arg_str1(NULL, NULL, "<ssid>", "SSID");
    add_wifi_credentials_args.password = arg_strn(NULL, NULL, "<password>", 0, 1, "Password");
    add_wifi_credentials_args.end = arg_end(2);
    const esp_console_cmd_t add_wifi_credentials_cmd = {
        .command = "wifi",
        .help = "Add WiFi credentials",
        .hint = nullptr,
        .func = &add_wifi_credentials,
        .argtable = &add_wifi_credentials_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&add_wifi_credentials_cmd));

    const esp_console_cmd_t clear_wifi_credentials_cmd = {
        .command = "clearwifi",
        .help = "Clear WiFi credentials",
        .hint = nullptr,
        .func = &clear_wifi_credentials,
        .argtable = nullptr
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&clear_wifi_credentials_cmd));

    set_acs_credentials_args.token = arg_str1(NULL, NULL, "<token>", "ACS token");
    set_acs_credentials_args.end = arg_end(2);
    const esp_console_cmd_t set_acs_credentials_cmd = {
        .command = "acs",
        .help = "Set ACS credentials",
        .hint = nullptr,
        .func = &set_acs_credentials,
        .argtable = &set_acs_credentials_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&set_acs_credentials_cmd));

    set_instance_args.instance = arg_str1(NULL, NULL, "<instance>", "Instance");
    set_instance_args.end = arg_end(2);
    const esp_console_cmd_t set_instance_cmd = {
        .command = "set_instance",
        .help = "Set instance",
        .hint = nullptr,
        .func = &set_instance,
        .argtable = &set_instance_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&set_instance_cmd));

    const char* prompt = LOG_COLOR_I "bigbro> " LOG_RESET_COLOR;
    int probe_status = linenoiseProbe();
    if (probe_status)
    {
        printf("\n"
               "Your terminal application does not support escape sequences.\n"
               "Line editing and history features are disabled.\n"
               "On Windows, try using Putty instead.\n");
        linenoiseSetDumbMode(1);
#if CONFIG_LOG_COLORS
        /* Since the terminal doesn't support escape sequences,
         * don't use color codes in the prompt.
         */
        prompt = "esp32> ";
#endif //CONFIG_LOG_COLORS
    }

    while (true)
    {
        char* line = linenoise(prompt);
        if (!line)
            continue;

        linenoiseHistoryAdd(line);

        int ret;
        esp_err_t err = esp_console_run(line, &ret);
        if (err == ESP_ERR_NOT_FOUND)
            printf("Unrecognized command\n");
        else if (err == ESP_ERR_INVALID_ARG)
            ; // command was empty
        else if (err == ESP_OK && ret != ESP_OK)
            printf("Command returned non-zero error code: 0x%x (%s)\n", ret, esp_err_to_name(err));
        else if (err != ESP_OK)
            printf("Internal error: %s\n", esp_err_to_name(err));

        linenoiseFree(line);
    }
}

// Local Variables:
// compile-command: "(cd ..; idf.py build)"
// End:
