#pragma once

class Controller
{
public:
	enum Event{WiFiAssociated, WiFiDisassociated, Connected, Disconnected};
	static void notify(Event event);
};
