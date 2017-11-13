#include "settings.h"

void Settings::loadSettings()
{
	setSSID("Fibertel");
	mWiFiPassword.write("lipschitzpotato", mWiFiPassword.capacity());
	mIP = 0;
	mSubnetMask = 0;
	mPassword.write("1234", mPassword.capacity());
	mText.write("Hello World!", mText.capacity());
	mBlinkRate = 0;
	mSlideRate = 0;
}

void Settings::storeSettings()
{
	#warning No implementado.
}

const char *Settings::ssid()
{
	return reinterpret_cast<const char*>(mSSID.data());
}

const char *Settings::wifiPassword()
{
	return reinterpret_cast<const char*>(mWiFiPassword.data());
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
	return reinterpret_cast<const char*>(mPassword.data());
}

const char *Settings::text()
{
	return reinterpret_cast<const char*>(mText.data());
}

uint8_t Settings::blinkRate()
{
	return mBlinkRate;
}

uint16_t Settings::slideRate()
{
	return mSlideRate;
}

void Settings::setSSID(const char *ssid)
{
	mSSID.clear();
	mSSID.write(ssid, mSSID.capacity());
}

void Settings::setWiFiPassword(const char *wifiPassword)
{
	mWiFiPassword.clear();
	mWiFiPassword.write(wifiPassword, mWiFiPassword.capacity());
}

void Settings::setIP(uint32_t ip)
{
	mIP = ip;
}

void Settings::setSubnetMask(uint32_t subnetMask)
{
	mSubnetMask = subnetMask;
}

void Settings::password(const char *password)
{
	mPassword.clear();
	mPassword.write(password, mPassword.capacity());
}

void Settings::setText(const char *text)
{
	mText.clear();
	mText.write(text, mText.capacity());
}

void Settings::setBlinkRate(uint8_t blinkRate)
{
	mBlinkRate = blinkRate;
}

void Settings::setSlideRate(uint16_t slideRate)
{
	mSlideRate = slideRate;
}