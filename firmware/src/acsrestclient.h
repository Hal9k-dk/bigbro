#pragma once

#include <ArduinoJson.h>
#include <RestClient.h>

#if TEST_SERVER
    #define SERVER "10.42.3.32"
    #define USE_SSL 1
#else
    #define SERVER "panopticon.hal9k.dk"
    #define USE_SSL 1
#endif

///Helper class wrapping restclient and json functionality, to communicate with the ACS REST API
class AcsRestClient
{
public:

	/**
	 * @param[in] path the path after API endpoint
	 */
	AcsRestClient(const char* path);

	/**
	 * Send a post request to the API
	 * @param[in] obj the json object to post to the API
	 */
	int post(const JsonObject& obj);

	///Returns the body of the response from the API
	String get_response() const;

private:
	String path;
	String response;
};
