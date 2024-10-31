#pragma once
#include <string>
//#include <QApplication>
namespace yk {

	class Settings {
	public:
		static Settings* GetInstance() {
			static Settings self;
			return &self;
		}
		//void ParseArgs(const QApplication& app);
		std::string client_id;
	private:
		Settings();
		~Settings();
		
	};

}