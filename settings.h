#ifndef SETTINGS_H
#define SETTINGS_H

#include "protocolo/Message.h"

extern "C" {
#include "osapi.h"
#include "user_interface.h"
}

struct FlashSettings {
	uint32_t checksum;
	uint32_t ip;
	uint32_t subnet;
	uint8_t blinkRate;
	uint8_t slideRate;
	char password[Message::TEXT_SIZE + 1];
	char ssid[Message::WIFI_SSID_SIZE + 1];
	char wifiPassword[Message::WIFI_SSID_SIZE + 1];
	char text[Message::TEXT_SIZE + 1];
};

class Settings {
public:
	static void init();
	static void loadSettings();
	static void storeSettings();

	static const char *ssid();
	static const char *wifiPassword();
	static uint32_t ip();
	static uint32_t subnetMask();
	static const char *password();
	static const char *text();
	static uint8_t blinkRate();
	static uint8_t slideRate();

	static void setSSID(const char *ssid);
	static void setWiFiPassword(const char *wifiPassword);
	static void setIP(uint32_t ip);
	static void setSubnetMask(uint32_t subnetMask);
	static void setPassword(const char *password);
	static void setText(const char *text, uint8_t blinkRate, int8_t slideRate);

private:
	static void loadDefaultSettings();

	static FlashSettings mSettings;

	static constexpr uint8_t INIT_COUNT = 5;
	static uint32_t mFlashStorage;
	static os_timer_t mResetButtonTimer;
	static uint8_t mResetCounter;
	static void resetButtonTaskCb(void *args);
};

#endif
