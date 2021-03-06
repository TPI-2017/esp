extern "C" {
#include "espmissingincludes.h"
#include "osapi.h"
#include "os_type.h"
#include "ip_addr.h"
#include "user_interface.h"
#include "espconn.h"
#include "cert.h"
#include "private_key.h"
}

#include "server.h"
#include "message_handler.h"

esp_tcp Server::tcpParams;
espconn Server::server;
espconn *Server::connection = nullptr;
Buffer<Message::MESSAGE_SIZE> Server::rxBuffer;
os_timer_t Server::timeoutTimer;

ICACHE_FLASH_ATTR
void Server::init()
{
	os_memset(&tcpParams, 0, sizeof(esp_tcp));
	os_memset(&server, 0, sizeof(espconn));
	tcpParams.local_port = 443;
	server.type = ESPCONN_TCP;
	server.state = ESPCONN_NONE;
	server.proto.tcp = &tcpParams;
}

ICACHE_FLASH_ATTR
void Server::listen()
{
	// TODO si algo anda mal, mirar aca.
	espconn_secure_set_default_certificate(default_certificate, default_certificate_len);
	espconn_secure_set_default_private_key(default_private_key, default_private_key_len);

	// Server callbacks
	espconn_regist_connectcb(&server, Server::connectCallback);
	espconn_regist_disconcb(&server, Server::disconnectCallback);
	espconn_regist_reconcb(&server, Server::reconnectCallback);
	espconn_regist_sentcb(&server, Server::sentCallback);
	espconn_regist_recvcb(&server, (espconn_recv_callback)Server::receiveCallback);

	espconn_set_opt(&server, ESPCONN_KEEPALIVE);
	espconn_clear_opt(&server, ESPCONN_COPY);
	
	espconn_tcp_set_max_con(1);
	espconn_secure_accept(&server);

	// Timer callback
	os_timer_setfn(&timeoutTimer, timeoutCallback, nullptr);
}

ICACHE_FLASH_ATTR
void Server::timeoutCallback(void *args)
{
	disconnect();
	os_printf("Timeout!\n");
}

ICACHE_FLASH_ATTR
void Server::disconnect()
{
	if (connection) {
		espconn_secure_disconnect(connection);
		connection = nullptr;
	}
	connection = nullptr;
	os_timer_disarm(&timeoutTimer);
}

ICACHE_FLASH_ATTR
void Server::connectCallback(void *conn)
{
	connection = static_cast<espconn*>(conn);
	rxBuffer.clear();
	os_timer_arm(&timeoutTimer, TIMEOUT_MS, false);
}

ICACHE_FLASH_ATTR
void Server::reconnectCallback(void *conn, sint8 error)
{
	disconnectCallback(conn);
}

ICACHE_FLASH_ATTR
void Server::disconnectCallback(void *conn)
{
	disconnect();
}

ICACHE_FLASH_ATTR
void Server::receiveCallback(void *conn, char *data, uint16 size)
{
	if (rxBuffer.write(data, size)) {
		if (rxBuffer.full()) {
			Message msg = Message::createMessage(rxBuffer.data());
			MessageHandler::handle(msg);
		}
	} else {
		os_printf("Buffer overflow\n");
	}
}

ICACHE_FLASH_ATTR
void Server::send(const void *data, uint16_t len)
{
	uint8_t *ptr = reinterpret_cast<uint8_t*>(const_cast<void*>(data));
	int8_t res = espconn_secure_send(connection, ptr, len);
	if (res) {
		os_printf("espconn_secure_send:\n");
		switch (res) {
			case ESPCONN_MEM:
				os_printf("Out of memory.\n");
				break;
			case ESPCONN_ARG:
				os_printf("Illegal argument.\n");
				break;
			case ESPCONN_MAX:
				os_printf("Outgoing buffer full.\n");
				break;
			case ESPCONN_IF:
				os_printf("UDP failed. (?)\n");
				break;
			default:
				os_printf("Returned %d\n", static_cast<int>(res));
				break;
		}
	}
}

ICACHE_FLASH_ATTR
void Server::sentCallback(void *conn)
{
	rxBuffer.clear();
}
