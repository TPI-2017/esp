#ifndef SETTINGS_H
#define SETTINGS_H

#include "protocolo/Message.h"

class Settings {
public:
	static void loadSettings();
	static void storeSettings();

	static const char *ssid();
	static const char *wifiPassword();
	static uint32_t ip();
	static uint32_t subnetMask();
	static const char *password();
	static const char *text();
	static uint8_t blinkRate();
	static uint16_t slideRate();

	static void setSSID(const char *ssid);
	static void setWiFiPassword(const char *wifiPassword);
	static void setIP(uint32_t ip);
	static void setSubnetMask(uint32_t subnetMask);
	static void setPassword(const char *password);
	static void setText(const char *text);
	static void setBlinkRate(uint8_t blinkRate);
	static void setSlideRate(uint16_t slideRate);

private:

	static char mSSID[Message::WIFI_SSID_SIZE + 1];
	static char mWiFiPassword[Message::WIFI_PASSWORD_SIZE + 1];
	static uint32_t mIP;
	static uint32_t mSubnetMask;
	static char mPassword[Message::PASSWORD_SIZE + 1];
	static char mText[Message::TEXT_SIZE + 1];
	static uint8_t mBlinkRate;
	static uint8_t mSlideRate;
};

#endif
