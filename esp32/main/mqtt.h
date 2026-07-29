#pragma once

#include <string>

#include "RDM6300.h"
#include "util.h"

#include "mqtt_client.h"

/// MQTT singleton
class Mqtt
{
public:
    using Card_id = RDM6300::Card_id;

    static Mqtt& instance();

    void start(const std::string& mqtt_address);

    void log(const std::string& msg);

    void set_status(const char* data);

    /// Write to panopticon log via gateway
    void log_backend(int user_id, const std::string&);

    /// Add unknown card to panopticon via gateway
    void log_unknown_card(Card_id card_id);

    /// Send a message to Slack via gateway
    void write_slack(const std::string& msg);

private:
    Mqtt() = default;

    ~Mqtt() = default;

    static void event_handler(void* handler_args,
                              esp_event_base_t base,
                              int32_t event_id,
                              void* event_data);

    static bool sign(cJSON* payload, const std::string& message);

    void handle_data(const std::string& topic,
                     const std::string& data);
    
    bool connected = false;
    esp_mqtt_client_handle_t client = 0;
};

// Local Variables:
// compile-command: "cd .. && idf.py build"
// End:
