extern "C" {
#include "espmissingincludes.h"
#include "osapi.h"
#include "user_interface.h"
}

#include "wifi_manager.h"
#include "protocolo/Message.h"
#include "settings.h"
#include "strings.h"
#include "server.h"
#include <stdint.h>

struct station_config WifiManager::stationConfig;
struct ip_info WifiManager::ipInfo;

void ICACHE_FLASH_ATTR WifiManager::wifiCallback(System_Event_t *evt)
{
	if (evt->event == EVENT_STAMODE_GOT_IP) {
		os_printf("WiFi connection established\n");
		Server::listen();
	} else if (evt->event == EVENT_STAMODE_DISCONNECTED) {
		os_printf("WiFi disconnected\n");
		Server::close();
	}
}

void ICACHE_FLASH_ATTR WifiManager::init()
{
	strcpy_s(&stationConfig.ssid, Settings::ssid(), Message::WIFI_SSID_SIZE + 1);
	strcpy_s(&stationConfig.password, Settings::wifiPassword(), Message::WIFI_PASSWORD_SIZE + 1);
	stationConfig.bssid_set = 0;

	uint32_t ip = Settings::ip();
	uint32_t subnet = Settings::subnetMask();
	IP4_ADDR(&ipInfo.ip, (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);
	IP4_ADDR(&ipInfo.gw, 192, 168, 0, 1);
	IP4_ADDR(&ipInfo.netmask, (subnet >> 24) & 0xFF, (subnet >> 16) & 0xFF, (subnet >> 8) & 0xFF, subnet & 0xFF);

	wifi_set_opmode_current(1);

	wifi_set_event_handler_cb(wifiCallback);

	wifi_station_dhcpc_stop();
	wifi_set_ip_info(STATION_IF, &ipInfo);
	wifi_station_set_config_current(&stationConfig);
}


