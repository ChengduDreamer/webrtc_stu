#pragma once

#include <fstream>
#include <memory>
#include <string>

class RawImage;

class ImageReader {
public:

	static std::shared_ptr<RawImage> ReadNV12(const std::string& path, int width, int height);
	static std::shared_ptr<RawImage> ReadRGBA(const std::string& path, int width, int height);
	static std::shared_ptr<RawImage> ReadI420(const std::string& path, int width, int height);

	static std::shared_ptr<RawImage> MockI420(int width, int height);
};