#include <iostream>
#include <thread>
#include <chrono>
#include <qapplication.h>
#include "web_socket_server.h"

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	
	auto ws_server = std::make_shared<yk::WebSocketServer>();

	app.exec();

	std::cout << "server exit..." << std::endl;

	return 0;
}

