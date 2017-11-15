#ifndef CONTROLLER_H
#define CONTROLLER_H

class Controller
{
public:
	enum Event{WiFiAssociated, WiFiDisassociated, Connected, Disconnected};
	static void notify(Event event);
};

#endif
