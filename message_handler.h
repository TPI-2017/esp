#pragma once

#include "protocolo/Message.h"

class MessageHandler {
public:
	static void handle(const Message &msg);
};