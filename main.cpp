extern "C" {
#include "espmissingincludes.h"
#include "osapi.h"
#include "user_interface.h"
}

#include "wifi_manager.h"

extern "C" {
/* Punto de entrada */
void user_init()
{
	uart_div_modify(0, UART_CLK_FREQ / 115200);
	gpio_init();
	WifiManager::init();
	Server::init();
}

}