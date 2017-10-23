#include "controller.h"
#include "server.h"

extern "C" {
#include "osapi.h"
#include "espmissingincludes.h"
}

static os_timer_t timer;
static void callback(void *arg)
{
	Server::send("Hola\n", 5);
	Server::send("Hola\n", 5);
}

void Controller::notify(Event event)
{
	switch (event)
	{
	case WiFiAssociated:
		Server::listen();
		break;
	case WiFiDisassociated:
		Server::close();
		break;
	case Connected:
		os_timer_disarm(&timer);
		os_timer_setfn(&timer, callback, NULL);
		os_timer_arm(&timer, 1000, 1);
		break;
	case Disconnected:
		os_timer_disarm(&timer);
		break;
	}
}
