#include "main_window.h"

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qboxlayout.h>
#include "context.h"
#include "signals_client.h"

namespace yk {

	MainWindow::MainWindow(const std::shared_ptr<Context>& ctx_ptr) : ctx_ptr_(ctx_ptr), QWidget(nullptr) {
		signals_client_ptr_ = std::make_shared<SignalsClient>(ctx_ptr_);
		Init();
		signals_client_ptr_->SayHello();
	}

	MainWindow::~MainWindow() {
	
	}

	void MainWindow::Init() {
		resize(800, 600);
		vbox_main_layout_ = new QVBoxLayout(this);
		hbox_layout_ = new QHBoxLayout();

		addr_lab_ = new QLabel(this);
		addr_lab_->setText(QStringLiteral("地址"));
		addr_edit_ = new QLineEdit(this);
		addr_edit_->setFixedSize(600, 40);
		connect_btn_ = new QPushButton(this);
		connect_btn_->setText(QStringLiteral("连接"));
		hbox_layout_->addStretch(1);
		hbox_layout_->addWidget(addr_lab_);
		hbox_layout_->addSpacing(12);
		hbox_layout_->addWidget(addr_edit_);
		hbox_layout_->addSpacing(12);
		hbox_layout_->addWidget(connect_btn_);
		hbox_layout_->addStretch(1);

		vbox_main_layout_->addStretch(1);
		vbox_main_layout_->addLayout(hbox_layout_);
		vbox_main_layout_->addStretch(1);
	}

	void MainWindow::InitQtSignalChannels() {
		connect(connect_btn_, &QPushButton::clicked, this, [=]() {
			std::string remote_id = addr_edit_->text().trimmed().toStdString();
			signals_client_ptr_->Call(remote_id);
		});
	}

}