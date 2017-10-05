#include "espmissingincludes.h"
#include "osapi.h"
#include "os_type.h"
#include "ip_addr.h"
#include "user_interface.h"
#include "espconn.h"
#include "tls_server.h"
#include "cert.h"
#include "private_key.h"

static struct _esp_tcp tcp_params = {
	.local_port = 443
};

static struct espconn server = {
	.type = ESPCONN_TCP,
	.state = ESPCONN_NONE,
	.proto = &tcp_params
};

static espconn * connection;

static void connect_cb(void * connection);
static void disconnect_cb(void * connection);
static void reconnect_cb(void * connection, sint8 error);

void tls_server_init()
{
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

static void connect_cb(void * connection)
{
	connection = (struct espconn *) connection;
	espconn_secure_send(connection, (uint8 *) "Hola desde ESP8266!\n", 20);
	os_printf("Connected.");
}


static void reconnect_cb(void * connection, sint8 error)
{
	os_printf("Connection lost.");
}