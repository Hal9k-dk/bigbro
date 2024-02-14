#pragma once

#include <deque>
#include <mutex>
#include <string>

#include "util.h"

extern "C" void slack_task(void*);

/// Slack_writer singleton
class Slack_writer
{
public:
    static Slack_writer& instance();

    void set_token(const std::string& token);
    
    void set_params(bool test_mode);

    void send_message(const std::string& message, bool include_general = false);

private:
    Slack_writer() = default;

    ~Slack_writer() = default;

    void send_to_channel(const std::string& channel,
                         const std::string& message);
    
    void thread_body();
    
    struct Item {
        std::string channel;
        std::string message;
    };
    std::deque<Item> q;
    std::mutex mutex;
    bool is_test_mode = false;
    std::string api_token;

    friend void slack_task(void*);
};
