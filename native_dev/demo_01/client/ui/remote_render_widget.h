#pragma once

#include <qwidget.h>

namespace yk {

	class RemoteRenderWidget : public QWidget {
	public:
		RemoteRenderWidget(QWidget* parent = nullptr);
		~RemoteRenderWidget();

		//virtual void StartLocalRenderer(webrtc::VideoTrackInterface* local_video);
		//
		//RenderImplWidget* render_impl_widget = nullptr;
		//
		//std::unique_ptr<VideoRenderer> local_renderer_;
	};


}