#include <iostream>
#include <string>
#include <qapplication.h>

#include "web_socket_client.h"
#include "context.h"
#include "common/common.h"
#pragma comment(lib, "winmm.lib ")
#pragma comment(lib, "Iphlpapi.lib ")
#pragma comment(lib, "wmcodecdspuuid.lib ")
#pragma comment(lib, "dmoguids.lib ")
#pragma comment(lib, "Msdmo.lib ")
#pragma comment(lib, "Secur32.lib ")

/* 
{
	1.Conductor
	2.window
	3.signals
}
*/


// client 
int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	auto ctx = yk::Context::Make();

	auto ws_client = yk::WebSocketClient::Make("client_" + std::to_string(TimeExt::GetCurrentTimestamp()));
	ws_client->Connect(*ctx->GetIoPool().get(), "127.0.0.1", "21365", "/");

	ws_client->AsyncSendBin("abc123");



	app.exec();
	return 0;
}