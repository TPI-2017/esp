#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "wifi_manager.h"
#include "server.h"

class Controller
{
public:
	enum Event{WiFiAssociated, WiFiDisassociated, Connected, Disconnected};
	static void notify(Event event);
};

#endif
