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

#include "tls_server.h"

static esp_tcp tcp_params;

static struct espconn server;

static espconn * connection;

static void connect_cb(void * connection);
static void disconnect_cb(void * connection);
static void reconnect_cb(void * connection, sint8 error);

void tls_server_init()
{
	tcp_params.local_port = 443;
	server.type = ESPCONN_TCP;
	server.state = ESPCONN_NONE;
	server.proto.tcp = &tcp_params;
	espconn_secure_set_default_certificate(default_certificate,
		default_certificate_len);
	espconn_secure_set_default_private_key(default_private_key,
		default_private_key_len);
}

void tls_server_start()
{
	espconn_regist_connectcb(&server, connect_cb);
	espconn_regist_disconcb(&server, disconnect_cb);
	espconn_regist_reconcb(&server, reconnect_cb);
	espconn_secure_accept(&server);

	os_printf("Listening for TLS connections...");
}

void tls_server_stop()
{
	if (connection)
		espconn_secure_disconnect(connection);
	connection = NULL;
}

static void connect_cb(void * conn)
{
	connection = (struct espconn *) conn;
	espconn_secure_send(connection, (uint8 *) "Hola desde ESP8266!\n", 20);
	os_printf("Connected.\n");
}

static void disconnect_cb(void * connection)
{
	connection = (struct espconn *) connection;
	os_printf("Disconnected.\n");
}

static void reconnect_cb(void * connection, sint8 error)
{
	os_printf("Connection lost.\n");
}