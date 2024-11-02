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

    // ��I420Buffer�е�YUV����д���ļ�
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
            std::cout << "YUV�����ѳɹ�д���ļ� '" << filename << "'." << std::endl;
        }
        else {
            std::cerr << "�޷����ļ���д�����ݡ�" << std::endl;
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
����δ����У�������һ��ö������VideoRotation�����ڱ�ʾ��Ƶ��ת�ĽǶȣ�����kVideoRotation_0��0�ȣ���kVideoRotation_90��90�ȣ���kVideoRotation_180��180�ȣ���kVideoRotation_270��270�ȣ���
rtc::scoped_refptr<webrtc::I420BufferInterface> buffer(video_frame.video_frame_buffer()->ToI420());���д��봴����һ��I420��ʽ����Ƶ�����������洢��buffer�С�
���ţ�if (video_frame.rotation() != webrtc::kVideoRotation_0)���д�������Ƶ֡����ת�Ƕ��Ƿ񲻵���0�ȣ������Ҫ��ת��Ƶ֡����ִ������Ĳ�����
����ת��Ƶ֡�Ĳ��֣�ʹ��webrtc::I420Buffer::Rotate(*buffer, video_frame.rotation())����ת��Ƶ֡������������ݸ�������ת�Ƕ�����ת��Ƶ֡���������ݡ�
           */


            rtc::scoped_refptr<webrtc::I420BufferInterface> buffer(
                video_frame.video_frame_buffer()->ToI420());
            if (video_frame.rotation() != webrtc::kVideoRotation_0) {   // VideoRotation ��ʲô
                buffer = webrtc::I420Buffer::Rotate(*buffer, video_frame.rotation());
            }
            

            std::cout << "buffer->width() = " << buffer->width() << " buffer->height() = " << buffer->height() << std::endl;

            //SetSize(buffer->width(), buffer->height());
           
            //RTC_DCHECK(image_.get() != NULL);

            //auto destRGBA = GMemoryPool->UniqueAlloc(frame->width * frame->height * 4);

            auto rgb_buffer = std::make_unique<uint8_t[]>(buffer->width() * buffer->height() *4);  // ���ﵽ����rgba ���� argb

            libyuv::I420ToARGB(buffer->DataY(), buffer->StrideY(), buffer->DataU(), buffer->StrideU(), buffer->DataV(), buffer->StrideV(), rgb_buffer.get(),
                buffer->width() * 4, buffer->width(), buffer->height());
            // ����Ϊʲô�ǿ�� * 4 ��Ҫ�о���

            std::lock_guard<std::mutex> lck{ argb_mutex_ };
            s_argb_data = std::move(rgb_buffer);

            // ��YUV����д���ļ�
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
        // ����QImage����RGB�������������
        QImage image(s_argb_data.get(), 3840, 2160, QImage::Format_ARGB32);

        // ��QImage���Ƶ�QWidget��
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