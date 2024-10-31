#pragma once
#include <qwidget.h>

class QLabel;
class QPushButton;
class QLineEdit;
class QVBoxLayout;
class QHBoxLayout;


namespace yk {

	class Context;

	class SignalsClient;

	class MainWindow : public QWidget {
	public:
		MainWindow(const std::shared_ptr<Context>& ctx_ptr);
		~MainWindow();
	private:
		void Init();
		void InitQtSignalChannels();
	private:
		QVBoxLayout* vbox_main_layout_ = nullptr;

		QHBoxLayout* hbox_layout_ = nullptr;
		QLabel* addr_lab_ = nullptr;
		QLineEdit* addr_edit_ = nullptr;
		QPushButton* connect_btn_ = nullptr;

	private:
		std::shared_ptr<Context> ctx_ptr_ = nullptr;
		std::shared_ptr<SignalsClient> signals_client_ptr_ = nullptr;
	};

}