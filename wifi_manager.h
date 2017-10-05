#ifndef WIFI_H
#define WIFI_H

extern "C" {
#include "user_interface.h"
}
#include "controller.h"

class WifiManager {
public:
	WifiManager();
	static void init();
private:
	static void wifiCallback(System_Event_t *evt);
	static struct station_config stationConfig;
	static struct ip_info ipInfo;
};

#endif
