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
#include "controller.h"
#include "message_handler.h"

esp_tcp Server::tcpParams;
espconn Server::server;
espconn *Server::connection = nullptr;
Buffer<Message::MESSAGE_SIZE> Server::rxBuffer;
Buffer<Message::MESSAGE_SIZE> Server::txBuffer;
bool Server::readyToSend = false;

void Server::init()
{
	os_memset(&tcpParams, 0, sizeof(esp_tcp));
	os_memset(&server, 0, sizeof(espconn));
	tcpParams.local_port = 443;
	server.type = ESPCONN_TCP;
	server.state = ESPCONN_NONE;
	server.proto.tcp = &tcpParams;
}

void Server::listen()
{
	// TODO si algo anda mal, mirar aca.
	espconn_secure_set_default_certificate(default_certificate, default_certificate_len);
	espconn_secure_set_default_private_key(default_private_key, default_private_key_len);

	espconn_regist_connectcb(&server, Server::connectCallback);
	espconn_regist_disconcb(&server, Server::disconnectCallback);
	espconn_regist_reconcb(&server, Server::reconnectCallback);
	espconn_regist_sentcb(&server, Server::sentCallback);
	espconn_regist_recvcb(&server, (espconn_recv_callback)Server::receiveCallback);
	espconn_set_opt(&server, ESPCONN_KEEPALIVE);
	espconn_clear_opt(&server, ESPCONN_COPY);
	espconn_secure_accept(&server);
}

uint16_t Server::send(const void *data, uint16_t len)
{
	#warning Implementar send.
	if (false) {
		uint8_t *ptr = reinterpret_cast<uint8_t*>(const_cast<void*>(data));
		if (readyToSend) {
			doSend(ptr, len);
			readyToSend = false;
			return len;
		} else {
			return txBuffer.write(data, len);
		}
	}
}

void Server::close()
{
	if (connection)
		espconn_secure_disconnect(connection);
	connection = nullptr;
}

void Server::connectCallback(void *conn)
{
	connection = static_cast<espconn*>(conn);
	readyToSend = true;
	Controller::notify(Controller::Connected);
}

void Server::disconnectCallback(void *conn)
{
	Controller::notify(Controller::Disconnected);
}

void Server::reconnectCallback(void *conn, sint8 error)
{
	Controller::notify(Controller::Disconnected);
}

void Server::receiveCallback(void *conn, char *data, uint16 size)
{
	if (rxBuffer.write(data, size)) {
		if (rxBuffer.full()) {
			Message msg = Message::createMessage(rxBuffer.data());
			#warning Desactivar timeout;
			rxBuffer.clear();
			MessageHandler::handle(msg);			
		} else {
			#warning Implementar timeout;
		}
	} else {
		os_printf("Buffer overflow\n");
		#warning Desactivar timeout;
		rxBuffer.clear();
	}
}

void Server::sentCallback(void *conn)
{
	#warning Implementar sentcallback.
	//if(txBuffer.empty()) {
		//readyToSend = true;
	//} else {
		//uint8_t block[128];
		//uint8_t blockSize = txBuffer.read(block, 128);
		//doSend(block, blockSize);
	//}
}

void Server::doSend(uint8_t *data, uint16_t len)
{
	int8_t res = espconn_secure_send(connection, data, len);

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
		}
	}
}
