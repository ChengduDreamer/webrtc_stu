#include "main_window.h"

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qboxlayout.h>
#include "context.h"
#include "signals_client.h"
#include "local_render_widget.h"
#include "remote_render_widget.h"
#include "settings.h"
#include "rtc/rtc_manager.h"

namespace yk {

	MainWindow::MainWindow(const std::shared_ptr<Context>& ctx_ptr) : ctx_ptr_(ctx_ptr), QWidget(nullptr) {


		rtc_manager_ptr_ = std::make_shared<RtcManager>();

		if (rtc_manager_ptr_->InitializePeerConnection()) {
			std::cout << "InitializePeerConnection ok" << std::endl;
		}
		else {
			std::cout << "InitializePeerConnection error" << std::endl;
			return;
		}

		if (rtc_manager_ptr_->CreatePeerConnection()) {
			std::cout << "CreatePeerConnection ok" << std::endl;
		}
		else {
			std::cout << "CreatePeerConnection error" << std::endl;
			return;
		}

	
		Init();

		InitQtSignalChannels();

		

		
	}

	MainWindow::~MainWindow() {
	
	}

	void MainWindow::Init() {
		resize(1200, 800);
		vbox_main_layout_ = new QVBoxLayout(this);

		hbox_settings_layout_ = new QHBoxLayout();
		local_id_lab_ = new QLabel(this);
		local_id_lab_->setText(QStringLiteral("本机ID"));
		local_id_edit_ = new QLineEdit(this);
		local_id_edit_->setFixedSize(100, 40);
		
		addr_lab_ = new QLabel(this);
		addr_lab_->setText(QStringLiteral("地址"));
		host_edit_ = new QLineEdit(this);
		host_edit_->setFixedSize(200, 40);
		port_edit_ = new QLineEdit(this);
		port_edit_->setFixedSize(100, 40);
		sign_connect_btn_ = new QPushButton(this);
		sign_connect_btn_->setText(QStringLiteral("信令连接"));
	
		remote_id_lab_ = new QLabel(this);
		remote_id_lab_->setText(QStringLiteral("远端ID"));
		remote_id_edit_ = new QLineEdit(this);
		remote_id_edit_->setFixedSize(100, 40);

		call_btn_ = new QPushButton(this);
		call_btn_->setText(QStringLiteral("呼叫"));

		hbox_settings_layout_->addWidget(local_id_lab_);
		hbox_settings_layout_->addWidget(local_id_edit_);
		hbox_settings_layout_->addWidget(addr_lab_);
		hbox_settings_layout_->addWidget(host_edit_);
		hbox_settings_layout_->addWidget(port_edit_);
		hbox_settings_layout_->addWidget(sign_connect_btn_);
		hbox_settings_layout_->addStretch();
		hbox_settings_layout_->addWidget(remote_id_lab_);
		hbox_settings_layout_->addWidget(remote_id_edit_);
		hbox_settings_layout_->addWidget(call_btn_);

		vbox_main_layout_->addLayout(hbox_settings_layout_);
		
		hbox_render_layout_ = new QHBoxLayout();
		local_render_widget_ = new LocalRenderWidget(this);
		remote_render_widget_ = new RemoteRenderWidget(this);
		hbox_render_layout_->addWidget(local_render_widget_);
		hbox_render_layout_->addWidget(remote_render_widget_);

		vbox_main_layout_->addLayout(hbox_render_layout_);

		// test
		host_edit_->setText("127.0.0.1");
		port_edit_->setText("21365");



		rtc_manager_ptr_->SetLocalRenderWidget(local_render_widget_);
	}

	void MainWindow::InitQtSignalChannels() {
		connect(sign_connect_btn_, &QPushButton::clicked, this, [=]() {
			Settings::GetInstance()->client_id = local_id_edit_->text().trimmed().toStdString();
			Settings::GetInstance()->signalling_host = host_edit_->text().trimmed().toStdString();
			Settings::GetInstance()->signalling_port = port_edit_->text().trimmed().toStdString();
			
			signals_client_ptr_ = std::make_shared<SignalsClient>(ctx_ptr_);

			signals_client_ptr_->SayHello();

			signals_client_ptr_->SetOnCreatedRoomMsgCallback([=]() {
				rtc_manager_ptr_->AddTracks();
				rtc_manager_ptr_->CreateOffer();
			});
		});

		connect(call_btn_, &QPushButton::clicked, this, [=]() {
			auto remote_id = remote_id_edit_->text().trimmed().toStdString();
			signals_client_ptr_->Call(remote_id);
		});
	}





}