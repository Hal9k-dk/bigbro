#include "cardcache.h"
#include "console.h"
#include "defs.h"
#include "display.h"
#include "format.h"
#include "hw.h"
#include "logger.h"
#include "nvs.h"
#include "reader.h"
#include "slack.h"

#include <string>

#include "esp_system.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"

#include <driver/ledc.h>
#include <driver/uart.h>

#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"

static Display* display = nullptr;

static int test_reader(int, char**)
{
    printf("Running reader test\n");

    const auto id = get_and_clear_last_cardid();
    printf("Card ID " CARD_ID_FORMAT "\n", id);
    
    return 0;
}

static int test_card_cache(int, char**)
{
    printf("Running card cache test\n");

    const auto result = Card_cache::instance().has_access(0x13006042CF);
    printf("Access: %d\n", static_cast<int>(result.access));
    printf("User:   %d\n", result.user_id);

    return 0;
}

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
    add_wifi_credentials(ssid, password);
    printf("OK: Added WiFi credentials %s/%s\n", ssid, password);
    return 0;
}

int list_wifi_creds(int argc, char** argv)
{
    const auto creds = get_wifi_creds();
    for (const auto& c : creds)
    {
        printf("%-20s %s\n", c.first.c_str(),
               c.second.empty() ? "" : "***");
    }
    printf("OK: Listed %d WiFi credentials\n", static_cast<int>(creds.size()));
    return 0;
}

int clear_wifi_credentials(int, char**)
{
    clear_wifi_credentials();
    printf("OK: WiFi credentials cleared\n");
    return 0;
}

struct
{
    struct arg_str* identifier;
    struct arg_end* end;
} set_identifier_args;

int set_identifier(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &set_identifier_args);
    if (nerrors != 0)
    {
        arg_print_errors(stderr, set_identifier_args.end, argv[0]);
        return 1;
    }
    const auto identifier = set_identifier_args.identifier->sval[0];
    if (strlen(identifier) < 2)
    {
        printf("ERROR: Invalid identifier\n");
        return 1;
    }
    set_identifier(identifier);
    printf("OK: Identifier set to %s\n", identifier);
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
    set_acs_token(token);
    return 0;
}

struct
{
    struct arg_str* token;
    struct arg_end* end;
} set_gw_credentials_args;

int set_gw_credentials(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &set_gw_credentials_args);
    if (nerrors != 0)
    {
        arg_print_errors(stderr, set_gw_credentials_args.end, argv[0]);
        return 1;
    }
    const auto token = set_gw_credentials_args.token->sval[0];
    if (strlen(token) < 32)
    {
        printf("ERROR: Invalid token\n");
        return 1;
    }
    set_gateway_token(token);
    printf("OK: Gateway token set to %s\n", token);
    return 0;
}

struct
{
    struct arg_str* token;
    struct arg_end* end;
} set_slack_credentials_args;

int set_slack_credentials(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &set_slack_credentials_args);
    if (nerrors != 0)
    {
        arg_print_errors(stderr, set_slack_credentials_args.end, argv[0]);
        return 1;
    }
    const auto token = set_slack_credentials_args.token->sval[0];
    if (strlen(token) < 32)
    {
        printf("ERROR: Invalid token\n");
        return 1;
    }
    set_slack_token(token);
    printf("OK: Slack token set to %s\n", token);
    return 0;
}

static int reboot(int, char**)
{
    printf("Reboot...\n");
    esp_restart();
    return 0;
}

static int read_switch(int, char**)
{
    for (int n = 0; n < 10; ++n)
    {
        vTaskDelay(500/portTICK_PERIOD_MS);
        printf("Switch closed: %d\n", read_switch());
    }
    printf("done\n");
    return 0;
}

static int read_rfid(int, char**)
{
    for (int n = 0; n < 100; ++n)
    {
        vTaskDelay(500/portTICK_PERIOD_MS);
        printf("RFID %" PRId64 "\n", get_and_clear_last_cardid());
    }
    printf("done\n");
    return 0;
}

static int toggle_relay(int, char**)
{
    for (int n = 0; n < 10; ++n)
    {
        vTaskDelay(500/portTICK_PERIOD_MS);
        set_relay(true);
        vTaskDelay(500/portTICK_PERIOD_MS);
        set_relay(false);
    }
    printf("done\n");
    return 0;
}

static int test_display(int, char**)
{
    printf("Running display test\n");
    set_backlight(255);

    display->clear();
    display->show_message("My name is Inigo Montoya", RED);
    
    return 0;
}

static int test_logger(int, char**)
{
    printf("Running logger test\n");

    Logger::instance().set_log_to_gateway(true);
    Logger::instance().log("BigBro test log: normal");
    Logger::instance().log_verbose("BigBro test log: verbose");
    Logger::instance().log_backend(42, "BigBro test log: backend");
    Logger::instance().log_unknown_card(0x12345678);

    return 0;
}

static int test_slack(int, char**)
{
    printf("Running Slack test\n");

    Slack_writer::instance().send_message(format("BigBro (%s) says hi",
                                                 get_identifier().c_str()));

    return 0;
}

// static
int test_backlight(int, char**)
{
    printf("Running backlight test\n");

    if (0)
    for (uint16_t i = 1; i < 3; i++)
        for (int j = 0; j < 8; ++j)
        {
            const int backlight = 31 + j * 32;
            printf("Backlight %d\n", backlight);
            set_backlight(backlight);
            vTaskDelay(500/portTICK_PERIOD_MS);
        }

    printf("Backlight fade from zero\n");
    set_backlight(0);
    vTaskDelay(500/portTICK_PERIOD_MS);
    fade_backlight(255, 5000);
    vTaskDelay(6000/portTICK_PERIOD_MS);
    printf("Backlight fade to zero\n");
    fade_backlight(0, 5000);
    vTaskDelay(6000/portTICK_PERIOD_MS);
    
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

void run_console(Display& display_arg)
{
    display = &display_arg;
    
    initialize_console();

    esp_console_register_help_command();

    const esp_console_cmd_t test_reader_cmd = {
        .command = "test_reader",
        .help = "Test card reader",
        .hint = nullptr,
        .func = &test_reader,
        .argtable = nullptr
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&test_reader_cmd));

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

    const esp_console_cmd_t list_wifi_credentials_cmd = {
        .command = "list_wifi",
        .help = "List WiFi credentials",
        .hint = nullptr,
        .func = &list_wifi_creds,
        .argtable = nullptr,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&list_wifi_credentials_cmd));

    const esp_console_cmd_t clear_wifi_credentials_cmd = {
        .command = "clearwifi",
        .help = "Clear WiFi credentials",
        .hint = nullptr,
        .func = &clear_wifi_credentials,
        .argtable = nullptr
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&clear_wifi_credentials_cmd));

    set_identifier_args.identifier = arg_str1(NULL, NULL, "<ident>", "Identifier");
    set_identifier_args.end = arg_end(2);
    const esp_console_cmd_t set_identifier_cmd = {
        .command = "ident",
        .help = "Set identifier",
        .hint = nullptr,
        .func = &set_identifier,
        .argtable = &set_identifier_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&set_identifier_cmd));

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

    set_gw_credentials_args.token = arg_str1(NULL, NULL, "<token>", "Gateway token");
    set_gw_credentials_args.end = arg_end(2);
    const esp_console_cmd_t set_gw_credentials_cmd = {
        .command = "gw",
        .help = "Set gateway credentials",
        .hint = nullptr,
        .func = &set_gw_credentials,
        .argtable = &set_gw_credentials_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&set_gw_credentials_cmd));

    set_slack_credentials_args.token = arg_str1(NULL, NULL, "<token>", "Slack token");
    set_slack_credentials_args.end = arg_end(2);
    const esp_console_cmd_t set_slack_credentials_cmd = {
        .command = "slack",
        .help = "Set Slack credentials",
        .hint = nullptr,
        .func = &set_slack_credentials,
        .argtable = &set_slack_credentials_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&set_slack_credentials_cmd));

    const esp_console_cmd_t reboot_cmd = {
        .command = "reboot",
        .help = "Reboot",
        .hint = nullptr,
        .func = &reboot,
        .argtable = nullptr
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&reboot_cmd));
    
    const esp_console_cmd_t test_card_cache_cmd = {
        .command = "test_card_cache",
        .help = "Test card cache",
        .hint = nullptr,
        .func = &test_card_cache,
        .argtable = nullptr
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&test_card_cache_cmd));

    const esp_console_cmd_t test_logger_cmd = {
        .command = "test_logger",
        .help = "Test logger",
        .hint = nullptr,
        .func = &test_logger,
        .argtable = nullptr
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&test_logger_cmd));

    const esp_console_cmd_t test_slack_cmd = {
        .command = "test_slack",
        .help = "Test Slack",
        .hint = nullptr,
        .func = &test_slack,
        .argtable = nullptr
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&test_slack_cmd));

    const esp_console_cmd_t read_switch_cmd = {
        .command = "switch",
        .help = "Read switch",
        .hint = nullptr,
        .func = &read_switch,
        .argtable = nullptr
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&read_switch_cmd));

    const esp_console_cmd_t read_rfid_cmd = {
        .command = "rfid",
        .help = "Read RFID",
        .hint = nullptr,
        .func = &read_rfid,
        .argtable = nullptr
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&read_rfid_cmd));

    const esp_console_cmd_t toggle_relay_cmd = {
        .command = "relay",
        .help = "Toggle relay",
        .hint = nullptr,
        .func = &toggle_relay,
        .argtable = nullptr
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&toggle_relay_cmd));

    const esp_console_cmd_t test_display_cmd = {
        .command = "display",
        .help = "Test display",
        .hint = nullptr,
        .func = &test_display,
        .argtable = nullptr
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&test_display_cmd));

    const esp_console_cmd_t test_backlight_cmd = {
        .command = "backlight",
        .help = "Test backlight",
        .hint = nullptr,
        .func = &test_backlight,
        .argtable = nullptr
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&test_backlight_cmd));

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
