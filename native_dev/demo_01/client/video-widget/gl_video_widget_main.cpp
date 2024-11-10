#if 0

#include <QApplication>
#include <QSurfaceFormat>

#include "GLVideoWidget.h"
#include "GLImageReader.h"
#include "RawImage.h"

int main(int argc, char** argv)
{
    QSurfaceFormat fmt;
    //fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    QSurfaceFormat::setDefaultFormat(fmt);

	QApplication app(argc, argv);

    GLVideoWidget view(RawImageFormat::kNV12);
    view.resize(1000, 700);
    view.show();

    auto nv12_raw_image = ImageReader::ReadNV12("frame_12.yuv", 1920, 1080);
    view.UpdateNV12Image(nv12_raw_image);

    //
    GLVideoWidget rgb_view(RawImageFormat::kRGBA);
    rgb_view.setWindowTitle("RGBA");
    rgb_view.resize(1000, 700);
    rgb_view.show();

    auto rgba_raw_image = ImageReader::ReadRGBA("z_image_10.rgba", 1920, 1080);
    rgb_view.UpdateRGBImage(rgba_raw_image);

	return app.exec();
}

#endif