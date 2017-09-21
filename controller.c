#include "ets_sys.h"
#include "gpio.h"
#include "wifi.h"
#include "controller.h"
#include "espmissingincludes.h"

/* Punto de entrada */
void user_init()
{
	uart_div_modify(0, UART_CLK_FREQ / 115200);
	gpio_init();
	wifi_init();
}
