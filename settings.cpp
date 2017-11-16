#include "settings.h"
#include "strings.h"

char Settings::mSSID[Message::WIFI_SSID_SIZE + 1];
char Settings::mWiFiPassword[Message::WIFI_PASSWORD_SIZE + 1];
uint32_t Settings::mIP;
uint32_t Settings::mSubnetMask;
char Settings::mPassword[Message::PASSWORD_SIZE + 1];
char Settings::mText[Message::TEXT_SIZE + 1];
uint8_t Settings::mBlinkRate;
uint8_t Settings::mSlideRate;

void Settings::loadSettings()
{
	#warning Esto se tiene que leer desde memoria no volatile.
	setSSID("Fibertel");
	setWiFiPassword("lipschitzpotato");
	setIP(192 << 24 | 168 << 16 | 0 << 8 | 14);
	setSubnetMask(255 << 24 | 255 << 16 | 255 << 8 | 0);
	setPassword("1234");
	setText("Hello World");
	setBlinkRate(1);
	setSlideRate(23);
}

void Settings::storeSettings()
{
	#warning Implementar para guardar settings.
}

const char *Settings::ssid()
{
	return reinterpret_cast<const char*>(mSSID);
}

const char *Settings::wifiPassword()
{
	return reinterpret_cast<const char*>(mWiFiPassword);
}

uint32_t Settings::ip()
{
	return mIP;
}

uint32_t Settings::subnetMask()
{
	return mSubnetMask;
}

const char *Settings::password()
{
	return reinterpret_cast<const char*>(mPassword);
}

const char *Settings::text()
{
	return reinterpret_cast<const char*>(mText);
}

uint8_t Settings::blinkRate()
{
	return mBlinkRate;
}

uint8_t Settings::slideRate()
{
	return mSlideRate;
}

void Settings::setSSID(const char *ssid)
{
	strcpy_s(mSSID, ssid, Message::WIFI_SSID_SIZE + 1);
}

void Settings::setWiFiPassword(const char *wifiPassword)
{
	strcpy_s(mWiFiPassword, wifiPassword, Message::WIFI_PASSWORD_SIZE + 1);
}

void Settings::setIP(uint32_t ip)
{
	mIP = ip;
}

void Settings::setSubnetMask(uint32_t subnetMask)
{
	mSubnetMask = subnetMask;
}

void Settings::setPassword(const char *password)
{
	strcpy_s(mPassword, password, Message::PASSWORD_SIZE + 1);
}

void Settings::setText(const char *text)
{
	strcpy_s(mText, text, Message::TEXT_SIZE + 1);
}

void Settings::setBlinkRate(uint8_t blinkRate)
{
	mBlinkRate = blinkRate;
}

void Settings::setSlideRate(uint8_t slideRate)
{
	mSlideRate = slideRate;
}
