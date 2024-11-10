#pragma once

#include <memory>

enum RawImageFormat {
	kRGB,
	kRGBA,
	kNV12,
	kI420
};

class RawImage {
public:

	static std::shared_ptr<RawImage> Make(char* data, int size, int width, int height, int ch, RawImageFormat format);
	static std::shared_ptr<RawImage> MakeRGB(char* data, int size, int width, int height);
	static std::shared_ptr<RawImage> MakeRGBA(char* data, int size, int width, int height);
	static std::shared_ptr<RawImage> MakeNV12(char* data, int size, int width, int height);
	static std::shared_ptr<RawImage> MakeI420(char* data, int size, int width, int height);

	RawImage(char* data, int size, int width, int height, int ch, RawImageFormat format);
	~RawImage();

	char* Data();
	int Size();

public:

	char*	img_buf = nullptr;
	int		img_buf_size = 0;
	int		img_width = 0;
	int		img_height = 0;
	int		img_ch = -1;
	RawImageFormat img_format;
};

typedef std::shared_ptr<RawImage> RawImagePtr;