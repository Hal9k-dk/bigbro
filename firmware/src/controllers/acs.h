#pragma once

#include <controllers/base.h>
#include "cardreader.h"

/**
 * Access control class for controllers. Implements the functionality to read and verify access cards via the REST API for the access server.
 */
class ACSController: public BaseController
{
public:
    ACSController();

    /**
     * Control loop, handles reading and verification of new cards.
     * Calls BaseController::update() on each run as well.
     */
    void update() override;
    
    /**
     * Returns the state of the relay
     * @return true if the card inserted has permission to use the machine 
     */
    bool relay_check() override;

    /**
     * Send a test request to the REST API
     */
    void test_command() override;

protected:

    /**
     * Reader object handling the RFID reader
     */
    CardReader reader;

    String last_card_id;
    String card_id;
    String name_trunc;

    bool has_allowed_card = false;

    /**
     * @return true if a card is present (valid card ID being read)
     */
    bool has_card();

    /**
     * @return true if the card is different than the last handled card
     */
    bool new_card();

    /**
     * @return the card ID as a string
     */
    String get_card();

    /**
     * @return true if the card has permissions to use the machine
     */
    bool card_allowed();

    /**
     * @param[in] card_id The id of the RFID card, used to query the REST API
     * @param[out] allowed True if the card is allowed to use the machine
     * @param[out] user_name The name of the user associated with the card
     * @param[out] user_id The ID of the user associated with the card
     * @param[out] message A string used to return errors from querying the server
     * @return true if the request was successfull, false on non http errors.
     */
    bool query_permission(const String& card_id,
					  bool& allowed,
					  String& user_name,
					  int& user_id,
					  String& message);

private:
};