#ifndef CONTROLLER_H
#define CONTROLLER_H

enum Event{WiFiAssociated, WiFiDisassociated};

void controller_accept_event(enum Event event);

#endif
