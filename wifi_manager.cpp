extern "C" {
#include "espmissingincludes.h"
#include "osapi.h"
#include "user_interface.h"
}
#include "wifi_manager.h"

struct station_config WifiManager::stationConfig;
struct ip_info WifiManager::ipInfo;

void ICACHE_FLASH_ATTR WifiManager::wifiCallback(System_Event_t * evt)
{
	if (evt->event == EVENT_STAMODE_GOT_IP) {
		os_printf("Got IP!\n");
		Controller::notify(Controller::WiFiAssociated);
	}
}

void ICACHE_FLASH_ATTR WifiManager::init()
{
	os_memcpy(&stationConfig.ssid, SSID, 32);
	os_memcpy(&stationConfig.password, PASSWORD, 64);
	stationConfig.bssid_set = 0;

	// Setear IP
	IP4_ADDR(&ipInfo.ip, 192, 168, 0, 14);
	IP4_ADDR(&ipInfo.gw, 192, 168, 0, 1);
	IP4_ADDR(&ipInfo.netmask, 255, 255, 255, 0);

	os_printf("wifi_init: Setting up 802.11 to station mode ... ");

	if (!wifi_set_opmode_current(1))
		os_printf("Failed to set 802.11 station mode!\n");

	wifi_set_event_handler_cb(wifiCallback);

	wifi_station_dhcpc_stop();
	wifi_set_ip_info(STATION_IF, &ipInfo);

	if (!wifi_station_set_config_current(&stationConfig))
		os_printf("Failed to set 802.11 config!\n");
}


