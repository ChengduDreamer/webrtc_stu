#include "RawImage.h"


std::shared_ptr<RawImage> RawImage::Make(char* data, int size, int width, int height, int ch, RawImageFormat format) {
	return std::make_shared<RawImage>(data, size, width, height, ch, format);
}

std::shared_ptr<RawImage> RawImage::MakeRGB(char* data, int size, int width, int height) {
	return std::make_shared<RawImage>(data, size, width, height, 3, RawImageFormat::kRGB);
}

std::shared_ptr<RawImage> RawImage::MakeRGBA(char* data, int size, int width, int height) {
	return std::make_shared<RawImage>(data, size, width, height, 4, RawImageFormat::kRGBA);
}

std::shared_ptr<RawImage> RawImage::MakeNV12(char* data, int size, int width, int height) {
	return std::make_shared<RawImage>(data, size, width, height, -1, RawImageFormat::kNV12);
}

std::shared_ptr<RawImage> RawImage::MakeI420(char* data, int size, int width, int height) {
	return std::make_shared<RawImage>(data, size, width, height, -1, RawImageFormat::kI420);
}

RawImage::RawImage(char* data, int size, int width, int height, int ch, RawImageFormat format) {
	img_buf = (char*)malloc(size);
	memcpy(img_buf, data, size);
	img_buf_size = size;
	img_width = width;
	img_height = height;
	img_ch = ch;
	img_format = format;
}

RawImage::~RawImage() {
	if (img_buf) {
		free(img_buf);
	}
}

char* RawImage::Data() {
	return img_buf;
}

int RawImage::Size() {
	return img_buf_size;
}