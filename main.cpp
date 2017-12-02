extern "C" {
#include "espmissingincludes.h"
#include "osapi.h"
#include "user_interface.h"
}

#include "settings.h"
#include "wifi_manager.h"
#include "server.h"
#include "led_sign.h"

extern "C" {
/* Punto de entrada */
void user_init()
{
	uart_div_modify(0, UART_CLK_FREQ / 115200);
	os_printf("\n\n");
	gpio_init();
	LEDSign::init();
	Settings::init();
	Settings::loadSettings();
	WifiManager::init();
	Server::init();
}

}
