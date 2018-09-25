#include <ArduinoJson.h>
#include "eeprom_wrapper.h"
#include "acsrestclient.h"
#include "controllers/acs.h"

#define SERIAL_DBG 1

ACSController::ACSController(): 
  BaseController(),
  reader(D5, SW_SERIAL_UNUSED_PIN, D6)
{}

bool ACSController::relay_check()
{
	if (new_card())
	{
		#if SERIAL_DBG
		Serial.print("New card: ");
		Serial.println(get_card());
		#endif

		has_allowed_card = false;
		if(has_card())
		{			
			display.set_status("Card present");
			has_allowed_card = card_allowed();
		}
		else
		{
			display.set_status("No card");
			led.set_colour(CRGB::Green);
			led.set_duty_cycle(1);
			led.set_period(10);
			return false;
		}
	}
	return has_allowed_card;
}

void ACSController::update()
{
	BaseController::update();
	led.update();
	reader.update();
	card_id = reader.get_card_id();
}

bool ACSController::has_card()
{
	return card_id.length() > 0;
}

bool ACSController::new_card()
{
	bool new_card = (card_id != last_card_id);
	last_card_id = card_id;
	return new_card;
}

String ACSController::get_card()
{
	return card_id;
}

bool ACSController::card_allowed()
{
	#if SERIAL_DBG
	Serial.println("Checking card");
	#endif

	display.set_status("Checking card", 1);

	String message, user_name;
	int user_id = 0;
	bool allowed = false;

	if(query_permission(card_id, allowed, user_name, user_id, message))
	{
		// Allowed
		if (allowed)
		{
			led.set_colour(CRGB::Green);
		}
		else
		{	
			// Also Denied
			led.set_colour(CRGB::Red);
		}
	}
	else
	{   
		// Denied due to error
		display.set_status(message);
		led.set_colour(CRGB::Crimson);
	}

	yield();
	led.set_duty_cycle(50);
	led.update();

	String name_trunc = user_name;
	if (name_trunc.length() > 17)
	{
		name_trunc = name_trunc.substring(0, 14) + String("...");
	}
	display.set_status(name_trunc, allowed ? "OK" : "Denied");

	int status;
	if (allowed)
	{
		#if SERIAL_DBG
		Serial.print("Card allowed: "); Serial.println(user_name);
		#endif
		status = log_access("Successful machine access", user_id);
	}
	else
	{
		#if SERIAL_DBG
		Serial.print("Card denied: "); Serial.println(message);
		#endif
		status = log_access("Machine access denied", user_id);
	}
	
	yield();

	// Status of log attempts
	switch(status)
	{
		case 200:
			break;
		case 404:
			display.set_status("Unknown card:", card_id);
			break;
		default:
			String s = "Bad HTTP log reply:";
			s += String(status);
			display.set_status(s);
			break;
	}
	yield();

	#if SERIAL_DBG
	Serial.print("Status: "); Serial.println(status);
	#endif

	return allowed; 
}

bool ACSController::query_permission(const String& card_id,
					  bool& allowed,
					  String& user_name,
					  int& user_id,
					  String& message)
{
	user_name = "";
	allowed = false;
	
	AcsRestClient rc("permissions");
	
	StaticJsonBuffer<200> jsonBuffer;
	auto& root = jsonBuffer.createObject();
	root["api_token"] = Eeprom::get_api_token();
	root["card_id"] = card_id;

	const int status = rc.post(root);
	switch (status) 
	{
		case 200:
		{
			auto resp = rc.get_response();

			// Remove garbage (why is it there?)
			uint16_t i = 0;
			while ((resp[i] != '{') && (i < resp.length()))
				++i;
			uint16_t j = i;
			while ((resp[j] != '}') && (j < resp.length()))
				++j;
			resp = resp.substring(i, j+1);

			StaticJsonBuffer<200> jsonBuffer;
			auto& json_resp = jsonBuffer.parseObject(resp);
			if (!json_resp.success())
			{
				#if SERIAL_DBG
				Serial.print("Bad JSON: "); Serial.println(resp);
				#endif
				message = "Bad JSON";
			}
			else
			{
				allowed = json_resp["allowed"];
				user_name = (const char*) json_resp["name"];
				user_id = json_resp["id"];
			}
			return true;
		}
		
		case 404:
			message = "Unknown card";
			allowed = false;
			return true;
	}

	String s = "Bad HTTP reply:";
	s += String(status);
	message = s;
	return false;
}

void ACSController::test_command()
{
	Serial.println("Sending test request");
	String message, user_name;
	bool allowed = false;
	int user_id = 0;
	if (!query_permission("0000BB96C5", allowed, user_name, user_id, message))
	{
		Serial.print("Error: ");
		Serial.println(message);
	}
	else
	{
		Serial.print("Success: User ");
		Serial.println(user_name);
	}
}