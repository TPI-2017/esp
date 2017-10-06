#ifndef SERVER_H
#define SERVER_H

extern "C" {
#include "ip_addr.h"
#include "user_interface.h"
#include "espconn.h"
}

#include "buffer.h"

class Server {
public:
	static void init();
	static void listen();
	static void close();
private:
	static esp_tcp tcpParams;
	static espconn server;
	static espconn *connection;
	static Buffer<> rxBuffer;
	static void connectCallback(void *conn);
	static void disconnectCallback(void *conn);
	static void reconnectCallback(void *conn, sint8 error);
};

#endif
