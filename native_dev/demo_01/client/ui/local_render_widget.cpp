#include "local_render_widget.h"
#include <iostream>
#include <fstream>
#include <mutex>
#include <libyuv.h>
#include <qimage.h>
#include <qsizepolicy.h>
#include "api/video/i420_buffer.h"
#include "rtc/defaults.h"
#include "rtc_base/arraysize.h"
#include "rtc_base/checks.h"
#include "rtc_base/logging.h"
#include "video-widget/GLVideoWidget.h"
//#include "third_party/libyuv/include/libyuv/convert_argb.h"

namespace yk {

    static std::unique_ptr<uint8_t[]> s_argb_data = nullptr;

    static std::mutex argb_mutex_;

    static std::shared_ptr<RawImage> s_raw_image = nullptr;


    static std::mutex s_yuv_mutex;

    // 将I420Buffer中的YUV数据写入文件
    void WriteYUVToFile(const webrtc::I420BufferInterface* buffer, const std::string& filename) {
        //std::ofstream outputFile(filename, std::ios::app | std::ios::binary);

        std::ofstream outputFile(filename, std::ios::out | std::ios::binary);

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


    void WriteYUVToFile2(const char* data, uint64_t data_size, const std::string& filename) {
        std::ofstream outputFile(filename, std::ios::app | std::ios::binary);

        //std::ofstream outputFile(filename, std::ios::out | std::ios::binary);

        if (outputFile.is_open()) {
            outputFile.write(data, data_size);
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
       // std::cout << "OnFrame" << std::endl;


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
            

           // std::cout << "buffer->width() = " << buffer->width() << " buffer->height() = " << buffer->height() << std::endl;

            //SetSize(buffer->width(), buffer->height());
           
            //RTC_DCHECK(image_.get() != NULL);

            //auto destRGBA = GMemoryPool->UniqueAlloc(frame->width * frame->height * 4);
            // 
#if 0
            // 将yuv420 转为rgb
            auto rgb_buffer = std::make_unique<uint8_t[]>(buffer->width() * buffer->height() *4);  // 这里到底是rgba 还是 argb

            libyuv::I420ToARGB(buffer->DataY(), buffer->StrideY(), buffer->DataU(), buffer->StrideU(), buffer->DataV(), buffer->StrideV(), rgb_buffer.get(),
                buffer->width() * 4, buffer->width(), buffer->height());
            // 这里为什么是宽度 * 4 需要研究下

            std::lock_guard<std::mutex> lck{ argb_mutex_ };
            s_argb_data = std::move(rgb_buffer);

            if (on_frame_cbk) {
                on_frame_cbk();
            }
            // 将YUV数据写入文件
            // WriteYUVToFile(buffer.get(), "output.yuv");
#endif
            

            int y_size = buffer->width() * buffer->height();
            int u_size = y_size / 4;
            int v_size = u_size;

            int total_size = y_size + u_size + v_size;
            char* img_buf = (char*)malloc(total_size);
          /*  memcpy(img_buf, video_frame->y, y_size);
            memcpy(img_buf + y_size, video_frame->u, u_size);
            memcpy(img_buf + y_size + u_size, video_frame->v, v_size);*/

            const uint8_t* dataY = buffer->DataY();
            const uint8_t* dataU = buffer->DataU();
            const uint8_t* dataV = buffer->DataV();

            int strideY = buffer->StrideY();
            int strideU = buffer->StrideU();
            int strideV = buffer->StrideV();
            int width = buffer->width();
            int height = buffer->height();

            // to do 这里 后面考虑copy连续内存，将内存对齐的步长也copy过去，尝试在shader

            // 这里最好固定好分辨率，防止内存对齐差异
            for (int y = 0; y < height; y++) {
                //outputFile.write(reinterpret_cast<const char*>(&dataY[y * strideY]), width);
                memcpy(img_buf + y * width, reinterpret_cast<const char*>(&dataY[y * strideY]), width);
            }
            for (int y = 0; y < height / 2; y++) {
                //outputFile.write(reinterpret_cast<const char*>(&dataU[y * strideU]), width / 2);

                memcpy(img_buf + height * width + y * (width / 2), reinterpret_cast<const char*>(&dataU[y * strideU]), width / 2);

            }
            for (int y = 0; y < height / 2; y++) {
                //outputFile.write(reinterpret_cast<const char*>(&dataV[y * strideV]), width / 2);

                memcpy(img_buf + height * width  + (height / 2) * (width / 2) + y * (width / 2), reinterpret_cast<const char*>(&dataV[y * strideV]), width / 2);
            }

            {
                std::lock_guard<std::mutex> lck{ s_yuv_mutex };
                s_raw_image = RawImage::MakeI420(img_buf, total_size, buffer->width(), buffer->height());
            }

          //  WriteYUVToFile2(img_buf, total_size, "output2.yuv");

            free(img_buf);

            if (on_frame_cbk) {
                on_frame_cbk();
            }

            

        }
        //InvalidateRect(wnd_, NULL, TRUE);
    }

#if 0

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
#endif
	LocalRenderWidget::LocalRenderWidget(QWidget* parent) : QWidget(parent) {
#if 0
	  	render_impl_widget = new RenderImplWidget(this);

       // render_impl_widget->setFixedSize(3840, 2160);
//        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        
#endif

        setStyleSheet(".QWidget {border: 1px solid #000000;}");
        setFixedSize(600, 400);


        gl_video_widget_ = new GLVideoWidget(RawImageFormat::kI420, this);
        gl_video_widget_->setFixedSize(600, 400);
	}


	LocalRenderWidget::~LocalRenderWidget() {

	}


    void LocalRenderWidget::StartLocalRenderer(webrtc::VideoTrackInterface* local_video) {
#if 0
        local_renderer_.reset(new VideoRenderer((HWND)render_impl_widget->winId(), 1, 1, local_video));

        local_renderer_->SetOnFrameCallback([=]() {
            QMetaObject::invokeMethod(this, [=]() {
                render_impl_widget->update();
            });
        });
#endif

        QMetaObject::invokeMethod(this, [=]() {
            local_renderer_.reset(new VideoRenderer((HWND)gl_video_widget_->winId(), 1, 1, local_video));  //注意: local_renderer_ 一定要在UI线程中创建， 否则 无法渲染画面，也不报任何错误



            local_renderer_->SetOnFrameCallback([=]() {
                QMetaObject::invokeMethod(this, [=]() {
                    //gl_video_widget_->update();
                    std::lock_guard<std::mutex> lck{ s_yuv_mutex };
                     gl_video_widget_->UpdateI420Image(s_raw_image);
                     gl_video_widget_->update();


                    //std::cout << "local render UpdateI420Image" << std::endl;

                    }, Qt::QueuedConnection);
                });

            }, Qt::QueuedConnection);


        

        
  
    }
}