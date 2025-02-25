#include <windows.h>
#include <shellapi.h>  // must come after windows.h

#include <iostream>
#include <string>
#include <qapplication.h>
#include <qsurfaceformat.h>
#include <qwidget.h>
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
#include "ui/main_window.h"
#include "settings.h"
#include "yk_logger.h"

#include "rtc/rtc_manager.h"
#include "ui/local_render_widget.h"


#pragma comment(lib, "winmm.lib ")
#pragma comment(lib, "Iphlpapi.lib ")
#pragma comment(lib, "wmcodecdspuuid.lib ")
#pragma comment(lib, "dmoguids.lib ")
#pragma comment(lib, "Msdmo.lib ")
#pragma comment(lib, "Secur32.lib ")

using namespace yk;
// client 
int main(int argc, char* argv[]) {


	QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
	QSurfaceFormat fmt;
	//fmt.setVersion(3, 3);
	fmt.setProfile(QSurfaceFormat::CoreProfile);
	fmt.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	fmt.setSwapInterval(0);
	QSurfaceFormat::setDefaultFormat(fmt);

	QApplication app(argc, argv);
	
	YK_LOGI("client start...");

	rtc::WinsockInitializer winsock_init;
	rtc::PhysicalSocketServer ss;
	rtc::AutoSocketServerThread main_thread(&ss);

	auto ctx = yk::Context::Make();
	Settings::GetInstance()->ParseArgs(app);

	rtc::InitializeSSL();
#if 0
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

	auto local_render_widget = new LocalRenderWidget();
	local_render_widget->show();
	
	rtc_manager->SetLocalRenderWidget(local_render_widget);
	
	
	rtc_manager->AddTracks();
	
	
	rtc_manager->CreateOffer();
#endif

	MainWindow main_window{ ctx };
	main_window.show();
	
	app.exec();
	return 0;
}