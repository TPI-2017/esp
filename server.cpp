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

esp_tcp Server::tcpParams;
espconn Server::server;
espconn *Server::connection = nullptr;

void Server::init()
{
	// TODO: inicializar cada campo
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
	espconn_secure_set_default_certificate(default_certificate,
		default_certificate_len);
	espconn_secure_set_default_private_key(default_private_key,
		default_private_key_len);

	espconn_regist_connectcb(&server, Server::connectCallback);
	espconn_regist_disconcb(&server, Server::disconnectCallback);
	espconn_regist_reconcb(&server, Server::reconnectCallback);
	espconn_secure_accept(&server);

	os_printf("Listening for TLS connections...");
}

void Server::close()
{
	if (connection)
		espconn_secure_disconnect(connection);
	connection = nullptr;
}

void Server::connectCallback(void *conn)
{
	connection = (espconn *) conn;
	espconn_secure_send(connection, (uint8 *) "Hola desde ESP8266!\n", 20);
	os_printf("Connected.\n");
}

void Server::disconnectCallback(void *conn)
{
	os_printf("Disconnected.\n");
}

void Server::reconnectCallback(void *conn, sint8 error)
{
	os_printf("Connection lost.\n");
}
