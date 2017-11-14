#ifndef SERVER_H
#define SERVER_H

extern "C" {
#include "ip_addr.h"
#include "user_interface.h"
#include "espconn.h"
}

#include "buffer.h"
#include "protocolo/Message.h"

class Server {
public:
	static void init();
	static void listen();
	static void close();
	static uint16_t send(const void *data, uint16_t len);
private:
	static esp_tcp tcpParams;
	static espconn server;
	static espconn *connection;
	static bool readyToSend;
	static Buffer<Message::MESSAGE_SIZE> rxBuffer;
	static Buffer<Message::MESSAGE_SIZE> txBuffer;
	static void connectCallback(void *conn);
	static void disconnectCallback(void *conn);
	static void reconnectCallback(void *conn, sint8 error);
	static void receiveCallback(void *conn, char *data, uint16 size);
	static void sentCallback(void *conn);
	static void doSend(uint8_t *data, uint16_t len);
};

#endif
