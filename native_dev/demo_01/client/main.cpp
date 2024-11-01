#include <windows.h>
#include <shellapi.h>  // must come after windows.h

#include <iostream>
#include <string>
#include <qapplication.h>

//#include <absl/flags/parse.h>
//#include <rtc_base/ssl_adapter.h>
//#include <rtc_base/win32_socket_init.h>


#include "absl/flags/parse.h"
//#include "examples/peerconnection/client/conductor.h"
//#include "examples/peerconnection/client/flag_defs.h"
//#include "examples/peerconnection/client/main_wnd.h"
//#include "examples/peerconnection/client/peer_connection_client.h"
#include "rtc_base/checks.h"
#include "rtc_base/ssl_adapter.h"
#include "rtc_base/string_utils.h"  // For ToUtf8
#include "rtc_base/win32_socket_init.h"
#include "rtc_base/physical_socket_server.h"
#include "system_wrappers/include/field_trial.h"

#include "web_socket_client.h"
#include "context.h"
#include "common/common.h"
//#include "ui/main_window.h"
//#include "settings.h"

#include "rtc/rtc_manager.h"


#pragma comment(lib, "winmm.lib ")
#pragma comment(lib, "Iphlpapi.lib ")
#pragma comment(lib, "wmcodecdspuuid.lib ")
#pragma comment(lib, "dmoguids.lib ")
#pragma comment(lib, "Msdmo.lib ")
#pragma comment(lib, "Secur32.lib ")

using namespace yk;
// client 
int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	std::cout << "start ..." << std::endl;

	rtc::WinsockInitializer winsock_init;
	rtc::PhysicalSocketServer ss;
	rtc::AutoSocketServerThread main_thread(&ss);

	auto ctx = yk::Context::Make();
	//Settings::GetInstance()->ParseArgs(app);

	rtc::InitializeSSL();

	auto rtc_manager = std::make_shared<RtcManager>();

	if (rtc_manager->InitializePeerConnection()) {
		std::cout << "InitializePeerConnection ok" << std::endl;
	}
	else {
		std::cout << "InitializePeerConnection error" << std::endl;
		return -1;
	}

	if (rtc_manager->CreatePeerConnection()) {
		std::cout << "CreatePeerConnection ok" << std::endl;
	}
	else {
		std::cout << "CreatePeerConnection error" << std::endl;
		return -1;
	}

	//MainWindow main_window{ ctx };
	//main_window.show();
	//
	app.exec();
	return 0;
}