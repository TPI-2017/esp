extern "C" {
#include "osapi.h"
#include "espmissingincludes.h"
}

#include "controller.h"
#include "server.h"

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
		break;
	case Disconnected:
		break;
	}
}
