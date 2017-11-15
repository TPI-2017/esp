extern "C" {
#include "espmissingincludes.h"
#include "osapi.h"
#include "os_type.h"
}

#include "message_handler.h"
#include "settings.h"
#include "strings.h"
#include "server.h"

void MessageHandler::handle(const Message &msg)
{
	if (msg.empty()) {
		os_printf("El mensaje está vacío\n");
		Server::send(Message::createGenericResponse(Message::ResponseCodePosition::MalformedPacket), Message::MESSAGE_SIZE);
		return;
	}

	if (msg.version() > Message::SUPPORTED_PROTOCOL_VERSION) {
		os_printf("Version de protocolo no soportada\n");
		Server::send(Message::createGenericResponse(Message::ResponseCodePosition::BadProtocolVersion), Message::MESSAGE_SIZE);
		return;
	}

	if (strcmp_s(msg.password(), Settings::password(), Message::PASSWORD_SIZE + 1)) {
		os_printf("Password incorrecto\n");
		Server::send(Message::createGenericResponse(Message::ResponseCodePosition::BadPassword), Message::MESSAGE_SIZE);
		return;
	}

	switch(msg.type()) {
		case Message::Type::Auth:
			os_printf("Auth request\n"); 
			Server::send(Message::createGenericResponse(Message::ResponseCodePosition::OK), Message::MESSAGE_SIZE);
			break;

		case Message::Type::SetPassword:
			os_printf("SetPassword request\n");
			Settings::setPassword(msg.newPassword());
			Server::send(Message::createGenericResponse(Message::ResponseCodePosition::OK), Message::MESSAGE_SIZE);
			break;

		case Message::Type::GetText:
			os_printf("GetText request\n");
			Server::send(Message::createGetTextResponse(Settings::blinkRate(), Settings::slideRate(), Settings::text()), Message::MESSAGE_SIZE);
			break;

		case Message::Type::SetText:
			os_printf("SetText request\n");
			Settings::setBlinkRate(msg.blinkRate());
			Settings::setSlideRate(msg.slideRate());
			Settings::setText(msg.text());
			Server::send(Message::createGenericResponse(Message::ResponseCodePosition::OK), Message::MESSAGE_SIZE);
			break;

		case Message::Type::GetWiFiConfig:
			os_printf("GetWifiConfig request\n");
			Server::send(Message::createGetWiFiConfigResponse(Settings::ssid(), Settings::wifiPassword(), Settings::ip(), Settings::subnetMask()), Message::MESSAGE_SIZE);
			break;

		case Message::Type::SetWiFiConfig:
			os_printf("SetWifiConfig request\n");
			Settings::setSSID(msg.ssid());
			Settings::setWiFiPassword(msg.wifiPassword());
			Settings::setIP(msg.ip());
			#warning Comprobar ip valida.
			Settings::setSubnetMask(msg.subnetMask());
			#warning Comprobar subnetMask válida.
			Server::send(Message::createGenericResponse(Message::ResponseCodePosition::OK), Message::MESSAGE_SIZE);
			break;

		case Message::Type::GenericResponse:
			os_printf("GenericResponse\n");
			Server::send(Message::createGenericResponse(Message::ResponseCodePosition::MalformedPacket), Message::MESSAGE_SIZE);
			break;

		case Message::Type::GetTextResponse:
			os_printf("GetTextResponse\n"); 
			Server::send(Message::createGenericResponse(Message::ResponseCodePosition::MalformedPacket), Message::MESSAGE_SIZE);
			break;

		case Message::Type::GetWiFiConfigResponse:
			os_printf("GetWifiConfigResponse\n");
			Server::send(Message::createGenericResponse(Message::ResponseCodePosition::MalformedPacket), Message::MESSAGE_SIZE);
			break;
	}
}