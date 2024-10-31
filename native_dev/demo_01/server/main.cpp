#include <iostream>
#include <thread>
#include <chrono>
//#include <qapplication.h>
#include "signals_server.h"

// server
using namespace yk;

int main(int argc, char* argv[]) {
	//QApplication app(argc, argv);
	

	auto signals_server = std::make_shared<SignalsServer>();


	//app.exec();
	std::cout << "server exit..." << std::endl;
	return 0;
}

