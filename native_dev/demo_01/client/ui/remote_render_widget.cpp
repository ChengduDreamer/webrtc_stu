#include "remote_render_widget.h"

namespace yk {

	RemoteRenderWidget::RemoteRenderWidget(QWidget* parent) : QWidget(parent) {
		setStyleSheet(".QWidget {border: 1px solid #000000;}");
		//setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		setFixedSize(600, 400);
	}
	RemoteRenderWidget::~RemoteRenderWidget() {
	
	
	}

}