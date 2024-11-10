#include "settings.h"
#include <qcommandlineparser.h>

namespace yk {

	Settings::Settings() {
	
	}

	Settings::~Settings() {
	
	}

	void Settings::ParseArgs(const QApplication& app) {
		// to do 以下暂不使用，目前使用界面设置 client id 与 remote id
		QCommandLineParser parser;
		parser.setApplicationDescription("yk Remote Desktop");
		parser.addHelpOption();
		parser.addVersionOption();
	
		QCommandLineOption opt_client_id("client_id", "current client id", "9 numbers", "");
		//QCommandLineOption opt_remote_id("remote_id", "remote client id", "9 numbers", "");
		parser.addOption(opt_client_id);
		//parser.addOption(opt_remote_id);
		parser.process(app);
	
		std::string current_client_id = parser.value(opt_client_id).toStdString();
		//std::string remote_client_id = parser.value(opt_remote_id).toStdString();
	
		client_id = current_client_id;
	}

}