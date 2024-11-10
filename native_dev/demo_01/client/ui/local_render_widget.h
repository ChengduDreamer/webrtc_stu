#pragma once
#include <qwidget.h>
#include <qpainter.h>
#include <qevent.h>
#include <functional>
#include "api/media_stream_interface.h"
#include "api/video/video_frame.h"
#include "examples/peerconnection/client/peer_connection_client.h"
#include "media/base/media_channel.h"
#include "media/base/video_common.h"
#if defined(WEBRTC_WIN)
#include "rtc_base/win32.h"
#endif  // WEBRTC_WIN


class GLVideoWidget;

namespace yk {

    class VideoRenderer : public rtc::VideoSinkInterface<webrtc::VideoFrame>{
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

        std::function<void(void)> on_frame_cbk = nullptr;

        void SetOnFrameCallback(std::function<void(void)>&& cbk) {
        
            on_frame_cbk = std::move(cbk);
        }

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

#if 0
    class RenderImplWidget : public QWidget {
    public:
        RenderImplWidget(QWidget* parent = nullptr);
        ~RenderImplWidget();
        void paintEvent(QPaintEvent*) override;

       
    };
#endif


	class LocalRenderWidget : public QWidget {
	public:
		LocalRenderWidget(QWidget* parent = nullptr);
		~LocalRenderWidget();

        virtual void StartLocalRenderer(webrtc::VideoTrackInterface* local_video);

    //    RenderImplWidget* render_impl_widget = nullptr;

        GLVideoWidget* gl_video_widget_ = nullptr;

        std::unique_ptr<VideoRenderer> local_renderer_;
	};
}