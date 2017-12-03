#include "settings.h"
#include "strings.h"
#include "led_sign.h"

extern "C" {
#include "osapi.h"
#include "user_interface.h"
#include "eagle_soc.h"
#include "gpio.h"
#include "spi_flash.h"
#include "espmissingincludes.h"
}
#include <stdbool.h>

#define FLASH_SEC_SIZE 4096

FlashSettings Settings::mSettings;
uint32_t Settings::mFlashStorage;
os_timer_t Settings::mResetButtonTimer;
uint8_t Settings::mResetCounter;

static uint32_t checksum(const FlashSettings *settings)
{
	const uint8_t *ptr = reinterpret_cast<const uint8_t*>(settings);
	uint32_t sum = 0;

	for (size_t i = 4; i < sizeof(FlashSettings); i++)
		sum += ptr[i];
	
	return sum;
}

void Settings::init()
{
	mResetCounter = INIT_COUNT;
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
	GPIO_DIS_OUTPUT(PERIPHS_IO_MUX_GPIO2_U);
	PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO2_U);

	os_timer_disarm(&mResetButtonTimer);
	os_timer_setfn(&mResetButtonTimer, resetButtonTaskCb, nullptr);
	os_timer_arm(&mResetButtonTimer, 1000, true);

	// Inicializar puntero a estructura en la flash
	uint32 addr = 0;

	switch (system_get_flash_size_map()) {
	case FLASH_SIZE_4M_MAP_256_256:
		addr = 0x3C000;
		break;

	case FLASH_SIZE_8M_MAP_512_512:
	case FLASH_SIZE_16M_MAP_512_512:
	case FLASH_SIZE_32M_MAP_512_512:
		addr = 0x7C000;
		break;

	case FLASH_SIZE_16M_MAP_1024_1024:
	case FLASH_SIZE_32M_MAP_1024_1024:
		addr = 0xFC000;
		break;
	default:
		addr = 0; // crashearemos
		break;
	}
	mFlashStorage = addr;
}

void Settings::loadSettings()
{
	uint32_t *ramAddr = reinterpret_cast<uint32_t*>(&mSettings);
	int result = spi_flash_read(mFlashStorage, ramAddr, sizeof(FlashSettings));
	if (result != SPI_FLASH_RESULT_OK) {
		os_printf("Flash read failure.\n");
		return;
	}

	os_printf("Flash read.\n");
	if (mSettings.checksum != checksum(&mSettings)) {
		os_printf("Settings corrupt, using defaults...");
		loadDefaultSettings();
	}

	LEDSign::messageChanged(mSettings.text, mSettings.blinkRate, mSettings.slideRate);
}

void Settings::storeSettings()
{
	int result;
	uint32_t flashAddr = mFlashStorage;
	uint32_t *ramAddr = reinterpret_cast<uint32_t*>(&mSettings);

	uint32_t sector = (flashAddr / FLASH_SEC_SIZE);
	result = spi_flash_erase_sector(sector);
	if (result != SPI_FLASH_RESULT_OK) {
		os_printf("Flash erase failed.\n");
		return;
	}

	mSettings.checksum = checksum(&mSettings);
	result = spi_flash_write(flashAddr, ramAddr, sizeof(mSettings));
	if (result != SPI_FLASH_RESULT_OK)
		os_printf("Flash write failed.\n");
	else
		os_printf("Flash written.\n");
}

void Settings::loadDefaultSettings()
{
	setSSID("Fibertel");
	setWiFiPassword("lipschitzpotato");
	setIP(192 << 24 | 168 << 16 | 0 << 8 | 14);
	setSubnetMask(255 << 24 | 255 << 16 | 255 << 8 | 0);
	setPassword("1234");
	setText("TPI 1 G7 2017", 1 << 3, -1);
	storeSettings();
	os_printf("Default settings loaded!\n");
}

const char *Settings::ssid()
{
	return mSettings.ssid;
}

const char *Settings::wifiPassword()
{
	return mSettings.wifiPassword;
}

uint32_t Settings::ip()
{
	return mSettings.ip;
}

uint32_t Settings::subnetMask()
{
	return mSettings.subnet;
}

const char *Settings::password()
{
	return mSettings.password;
}

const char *Settings::text()
{
	return mSettings.text;
}

uint8_t Settings::blinkRate()
{
	return mSettings.blinkRate;
}

uint8_t Settings::slideRate()
{
	return mSettings.slideRate;
}

void Settings::setSSID(const char *ssid)
{
	strcpy_s(mSettings.ssid, Message::WIFI_SSID_SIZE + 1, ssid);
}

void Settings::setWiFiPassword(const char *wifiPassword)
{
	strcpy_s(mSettings.wifiPassword, Message::WIFI_PASSWORD_SIZE + 1, wifiPassword);
}

void Settings::setIP(uint32_t ip)
{
	mSettings.ip = ip;
}

void Settings::setSubnetMask(uint32_t subnetMask)
{
	mSettings.subnet = subnetMask;
}

void Settings::setPassword(const char *password)
{
	strcpy_s(mSettings.password, Message::PASSWORD_SIZE + 1, password);
}

void Settings::setText(const char *text, uint8_t blinkRate, int8_t slideRate)
{
	strcpy_s(mSettings.text, Message::TEXT_SIZE + 1, text);
	mSettings.blinkRate = blinkRate;
	mSettings.slideRate = slideRate;
	LEDSign::messageChanged(text, blinkRate, slideRate);
}

void Settings::resetButtonTaskCb(void *args)
{
	if (GPIO_INPUT_GET(2) == 0)
		mResetCounter--;
	else
		mResetCounter = INIT_COUNT;

	if (mResetCounter == 0) {
		loadDefaultSettings();
		system_restore();
		system_restart();
		mResetCounter = INIT_COUNT;
	}
}
