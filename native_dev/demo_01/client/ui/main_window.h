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

	class LocalRenderWidget;

	class RemoteRenderWidget;

	class RtcManager;

	class MainWindow : public QWidget {
	public:
		MainWindow(const std::shared_ptr<Context>& ctx_ptr);
		~MainWindow();
	private:
		void Init();
		void InitQtSignalChannels();
	private:
		QVBoxLayout* vbox_main_layout_ = nullptr;
		// settings
		QHBoxLayout* hbox_settings_layout_ = nullptr;
		QLabel* local_id_lab_ = nullptr;
		QLineEdit* local_id_edit_ = nullptr;
		
		QLabel* addr_lab_ = nullptr;
		QLineEdit* host_edit_ = nullptr;
		QLineEdit* port_edit_ = nullptr;
		QPushButton* sign_connect_btn_ = nullptr;

		QLabel* remote_id_lab_ = nullptr;
		QLineEdit* remote_id_edit_ = nullptr;

		QPushButton* call_btn_ = nullptr;

		// render
		QHBoxLayout* hbox_render_layout_ = nullptr;
		LocalRenderWidget* local_render_widget_ = nullptr;
		RemoteRenderWidget* remote_render_widget_ = nullptr;

	private:
		std::shared_ptr<Context> ctx_ptr_ = nullptr;
		std::shared_ptr<SignalsClient> signals_client_ptr_ = nullptr;
		std::shared_ptr<RtcManager> rtc_manager_ptr_ = nullptr;
	};

}