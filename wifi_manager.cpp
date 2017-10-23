extern "C" {
#include "espmissingincludes.h"
#include "osapi.h"
#include "user_interface.h"
}
#include "wifi_manager.h"
#include "user_config.h"

struct station_config WifiManager::stationConfig;
struct ip_info WifiManager::ipInfo;

void ICACHE_FLASH_ATTR WifiManager::wifiCallback(System_Event_t * evt)
{
	if (evt->event == EVENT_STAMODE_GOT_IP) {
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

	wifi_set_opmode_current(1);

	wifi_set_event_handler_cb(wifiCallback);

	wifi_station_dhcpc_stop();
	wifi_set_ip_info(STATION_IF, &ipInfo);
	wifi_station_set_config_current(&stationConfig);
}


