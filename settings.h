#ifndef SETTINGS_H
#define SETTINGS_H

#include "protocolo/Message.h"

class Settings {
public:
	static char    ssid[Message::WIFI_SSID_SIZE + 1];
	static char    wifiPassword[Message::WIFI_PASSWORD_SIZE + 1];
	static char    password[Message::PASSWORD_SIZE + 1];
	static char    text[Message::TEXT_SIZE + 1];
	static uint8_t blinkRate;
	static uint8_t slideRate;

	static void loadSettings();
	static void storeSettings();
};

#endif
