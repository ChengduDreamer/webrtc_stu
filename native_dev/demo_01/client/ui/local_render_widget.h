#pragma once
#include <qwidget.h>

#include "api/media_stream_interface.h"
#include "api/video/video_frame.h"
#include "examples/peerconnection/client/peer_connection_client.h"
#include "media/base/media_channel.h"
#include "media/base/video_common.h"
#if defined(WEBRTC_WIN)
#include "rtc_base/win32.h"
#endif  // WEBRTC_WIN

namespace yk {

    class VideoRenderer : public rtc::VideoSinkInterface<webrtc::VideoFrame> {
    public:
        VideoRenderer(HWND wnd,
            int width,
            int height,
            webrtc::VideoTrackInterface* track_to_render);
        virtual ~VideoRenderer();

       // void Lock() { ::EnterCriticalSection(&buffer_lock_); }
       //
       // void Unlock() { ::LeaveCriticalSection(&buffer_lock_); }

        // VideoSinkInterface implementation
        void OnFrame(const webrtc::VideoFrame& frame) override;

       // const BITMAPINFO& bmi() const { return bmi_; }
       // const uint8_t* image() const { return image_.get(); }



    protected:
        void SetSize(int width, int height);

        enum {
            SET_SIZE,
            RENDER_FRAME,
        };

        HWND wnd_;
        //BITMAPINFO bmi_;
        //std::unique_ptr<uint8_t[]> image_;
        //CRITICAL_SECTION buffer_lock_;
        rtc::scoped_refptr<webrtc::VideoTrackInterface> rendered_track_;
    };


	class LocalRenderWidget : public QWidget {
	public:
		LocalRenderWidget(QWidget* parent = nullptr);
		~LocalRenderWidget();

        virtual void StartLocalRenderer(webrtc::VideoTrackInterface* local_video);

		QWidget* render_impl_widget = nullptr;

        std::unique_ptr<VideoRenderer> local_renderer_;
	};
}