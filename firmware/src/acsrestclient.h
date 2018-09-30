#pragma once

#include <ArduinoJson.h>
#include <RestClient.h>

#if TEST_SERVER
    #define SERVER "192.168.0.122"
    #define USE_SSL 0
#else
    #define SERVER "panopticon.hal9k.dk"
    #define USE_SSL 1
#endif

class AcsRestClient
{
public:
	AcsRestClient(const char* path);

	void set_param(const char* name, const String& value);

	int post(const JsonObject& obj);

	String get_response() const;

private:
	String path;
	String response;
};
