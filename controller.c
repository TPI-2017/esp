#include "espmissingincludes.h"
#include "ets_sys.h"
#include "gpio.h"
#include "wifi.h"
#include "controller.h"
#include "tls_server.h"

void controller_accept_event(enum Event event)
{
	switch (event)
	{
	case WiFiAssociated:
		tls_server_start();
		break;
	case WiFiDisassociated:
		break;
	}
}

/* Punto de entrada */
void user_init()
{
	uart_div_modify(0, UART_CLK_FREQ / 115200);
	gpio_init();
	wifi_init();
	tls_server_init();
}
