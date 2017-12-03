extern "C" {
#include "espmissingincludes.h"
#include "osapi.h"
#include "os_type.h"
}

#include "message_handler.h"
#include "settings.h"
#include "strings.h"
#include "server.h"

ICACHE_FLASH_ATTR
void MessageHandler::handle(const Message &msg)
{
	if (msg.empty()) {
		os_printf("El mensaje está vacío\n");
		Server::send(Message::createGenericResponse(Message::ResponseCodePosition::MalformedPacket).data(), Message::MESSAGE_SIZE);
		return;
	}

	if (msg.version() > Message::SUPPORTED_PROTOCOL_VERSION) {
		os_printf("Version de protocolo no soportada\n");
		Server::send(Message::createGenericResponse(Message::ResponseCodePosition::BadProtocolVersion).data(), Message::MESSAGE_SIZE);
		return;
	}

	int cmpResult;
	strcmp_s(msg.password(), Message::PASSWORD_SIZE + 1, Settings::password(), &cmpResult);
	if (cmpResult) {
		os_printf("Password incorrecto\n");
		Server::send(Message::createGenericResponse(Message::ResponseCodePosition::BadPassword).data(), Message::MESSAGE_SIZE);
		return;
	}

	switch(msg.type()) {
		case Message::Type::Auth:
			os_printf("Auth request\n"); 
			Server::send(Message::createGenericResponse(Message::ResponseCodePosition::OK).data(), Message::MESSAGE_SIZE);
			break;

		case Message::Type::SetPassword:
			os_printf("SetPassword request\n");
			Settings::setPassword(msg.newPassword());
			Settings::storeSettings();
			Server::send(Message::createGenericResponse(Message::ResponseCodePosition::OK).data(), Message::MESSAGE_SIZE);
			break;

		case Message::Type::GetText:
			os_printf("GetText request\n");
			Server::send(Message::createGetTextResponse(Settings::blinkRate(), Settings::slideRate(), Settings::text()).data(), Message::MESSAGE_SIZE);
			break;

		case Message::Type::SetText:
			os_printf("SetText request\n");
			Settings::setText(msg.text(), msg.blinkRate(), msg.slideRate());
			Settings::storeSettings();
			Server::send(Message::createGenericResponse(Message::ResponseCodePosition::OK).data(), Message::MESSAGE_SIZE);
			break;

		case Message::Type::GetWiFiConfig:
			os_printf("GetWifiConfig request\n");
			Server::send(Message::createGetWiFiConfigResponse(Settings::ssid(), Settings::wifiPassword(), Settings::ip(), Settings::subnetMask()).data(), Message::MESSAGE_SIZE);
			break;

		case Message::Type::SetWiFiConfig:
			os_printf("SetWifiConfig request\n");
			Settings::setSSID(msg.wifiSSID());
			Settings::setWiFiPassword(msg.wifiPassword());
			Settings::setIP(msg.wifiIP());
			Settings::setSubnetMask(msg.wifiSubnet());
			Settings::storeSettings();
			Server::send(Message::createGenericResponse(Message::ResponseCodePosition::OK).data(), Message::MESSAGE_SIZE);
			break;

		case Message::Type::GenericResponse:
			os_printf("GenericResponse\n");
			Server::send(Message::createGenericResponse(Message::ResponseCodePosition::MalformedPacket).data(), Message::MESSAGE_SIZE);
			break;

		case Message::Type::GetTextResponse:
			os_printf("GetTextResponse\n"); 
			Server::send(Message::createGenericResponse(Message::ResponseCodePosition::MalformedPacket).data(), Message::MESSAGE_SIZE);
			break;

		case Message::Type::GetWiFiConfigResponse:
			os_printf("GetWifiConfigResponse\n");
			Server::send(Message::createGenericResponse(Message::ResponseCodePosition::MalformedPacket).data(), Message::MESSAGE_SIZE);
			break;
	}
}