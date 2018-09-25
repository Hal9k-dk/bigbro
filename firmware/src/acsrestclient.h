#pragma once

#include <ArduinoJson.h>
#include <RestClient.h>

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
