#include "local_render_widget.h"
#include <iostream>
#include <fstream>
#include <mutex>
#include <libyuv.h>
#include <qimage.h>
#include "api/video/i420_buffer.h"
#include "rtc/defaults.h"
#include "rtc_base/arraysize.h"
#include "rtc_base/checks.h"
#include "rtc_base/logging.h"
//#include "third_party/libyuv/include/libyuv/convert_argb.h"

namespace yk {

    static std::unique_ptr<uint8_t[]> s_argb_data = nullptr;

    static std::mutex argb_mutex_;

    // 将I420Buffer中的YUV数据写入文件
    void WriteYUVToFile(const webrtc::I420BufferInterface* buffer, const std::string& filename) {
        std::ofstream outputFile(filename, std::ios::app | std::ios::binary);

        if (outputFile.is_open()) {
            const uint8_t* dataY = buffer->DataY();
            const uint8_t* dataU = buffer->DataU();
            const uint8_t* dataV = buffer->DataV();
            int strideY = buffer->StrideY();
            int strideU = buffer->StrideU();
            int strideV = buffer->StrideV();
            int width = buffer->width();
            int height = buffer->height();

            for (int y = 0; y < height; y++) {
                outputFile.write(reinterpret_cast<const char*>(&dataY[y * strideY]), width);
            }
            for (int y = 0; y < height / 2; y++) {
                outputFile.write(reinterpret_cast<const char*>(&dataU[y * strideU]), width / 2);
            }
            for (int y = 0; y < height / 2; y++) {
                outputFile.write(reinterpret_cast<const char*>(&dataV[y * strideV]), width / 2);
            }

            outputFile.close();
            std::cout << "YUV数据已成功写入文件 '" << filename << "'." << std::endl;
        }
        else {
            std::cerr << "无法打开文件以写入数据。" << std::endl;
        }
    }


    VideoRenderer::VideoRenderer(HWND wnd, int width, int height, webrtc::VideoTrackInterface* track_to_render) : wnd_(wnd), rendered_track_(track_to_render) {
      //  ::InitializeCriticalSection(&buffer_lock_);
      /*  ZeroMemory(&bmi_, sizeof(bmi_));
        bmi_.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi_.bmiHeader.biPlanes = 1;
        bmi_.bmiHeader.biBitCount = 32;
        bmi_.bmiHeader.biCompression = BI_RGB;
        bmi_.bmiHeader.biWidth = width;
        bmi_.bmiHeader.biHeight = -height;
        bmi_.bmiHeader.biSizeImage = width * height * (bmi_.bmiHeader.biBitCount >> 3);*/
        rendered_track_->AddOrUpdateSink(this, rtc::VideoSinkWants());
    }

    VideoRenderer::~VideoRenderer() {
        rendered_track_->RemoveSink(this);
      //  ::DeleteCriticalSection(&buffer_lock_);
    }

    void VideoRenderer::SetSize(int width, int height) {
        //AutoLock<VideoRenderer> lock(this);
        //
        //if (width == bmi_.bmiHeader.biWidth && height == bmi_.bmiHeader.biHeight) {
        //    return;
        //}
        //
        //bmi_.bmiHeader.biWidth = width;
        //bmi_.bmiHeader.biHeight = -height;
        //bmi_.bmiHeader.biSizeImage =
        //    width * height * (bmi_.bmiHeader.biBitCount >> 3);
        //image_.reset(new uint8_t[bmi_.bmiHeader.biSizeImage]);
    }

    void VideoRenderer::OnFrame(const webrtc::VideoFrame& video_frame) {
        std::cout << "OnFrame" << std::endl;


        {
           // AutoLock<VideoRenderer> lock(this);
           //
           /*
在这段代码中，定义了一个枚举类型VideoRotation，用于表示视频旋转的角度，包括kVideoRotation_0（0度）、kVideoRotation_90（90度）、kVideoRotation_180（180度）和kVideoRotation_270（270度）。
rtc::scoped_refptr<webrtc::I420BufferInterface> buffer(video_frame.video_frame_buffer()->ToI420());这行代码创建了一个I420格式的视频缓冲区，并存储在buffer中。
接着，if (video_frame.rotation() != webrtc::kVideoRotation_0)这行代码检查视频帧的旋转角度是否不等于0度，如果需要旋转视频帧，则执行下面的操作。
在旋转视频帧的部分，使用webrtc::I420Buffer::Rotate(*buffer, video_frame.rotation())来旋转视频帧。这个方法根据给定的旋转角度来旋转视频帧的像素数据。
           */


            rtc::scoped_refptr<webrtc::I420BufferInterface> buffer(
                video_frame.video_frame_buffer()->ToI420());
            if (video_frame.rotation() != webrtc::kVideoRotation_0) {   // VideoRotation 是什么
                buffer = webrtc::I420Buffer::Rotate(*buffer, video_frame.rotation());
            }
            

            std::cout << "buffer->width() = " << buffer->width() << " buffer->height() = " << buffer->height() << std::endl;

            //SetSize(buffer->width(), buffer->height());
           
            //RTC_DCHECK(image_.get() != NULL);

            //auto destRGBA = GMemoryPool->UniqueAlloc(frame->width * frame->height * 4);

            auto rgb_buffer = std::make_unique<uint8_t[]>(buffer->width() * buffer->height() *4);  // 这里到底是rgba 还是 argb

            libyuv::I420ToARGB(buffer->DataY(), buffer->StrideY(), buffer->DataU(), buffer->StrideU(), buffer->DataV(), buffer->StrideV(), rgb_buffer.get(),
                buffer->width() * 4, buffer->width(), buffer->height());
            // 这里为什么是宽度 * 4 需要研究下

            std::lock_guard<std::mutex> lck{ argb_mutex_ };
            s_argb_data = std::move(rgb_buffer);

            // 将YUV数据写入文件
            //WriteYUVToFile(buffer.get(), "output.yuv");
        }
        //InvalidateRect(wnd_, NULL, TRUE);
    }



    RenderImplWidget::RenderImplWidget(QWidget* parent) : QWidget(parent) {
    
    }

    RenderImplWidget::~RenderImplWidget() {
    
    }

    void RenderImplWidget::paintEvent(QPaintEvent* event)  {
        QPainter painter(this);


        std::lock_guard<std::mutex> lck{ argb_mutex_ };
        if (!s_argb_data) {
            QWidget::paintEvent(event);
        }
        // 创建QImage并从RGB数据中填充像素
        QImage image(s_argb_data.get(), 3840, 2160, QImage::Format_ARGB32);

        // 将QImage绘制到QWidget上
        painter.drawImage(0, 0, image);
    }

	LocalRenderWidget::LocalRenderWidget(QWidget* parent) : QWidget(parent) {
		
		render_impl_widget = new RenderImplWidget(this);

        render_impl_widget->setFixedSize(3840, 2160);

	}


	LocalRenderWidget::~LocalRenderWidget() {

	}


    void LocalRenderWidget::StartLocalRenderer(webrtc::VideoTrackInterface* local_video) {
        local_renderer_.reset(new VideoRenderer((HWND)render_impl_widget->winId(), 1, 1, local_video));
    }
}