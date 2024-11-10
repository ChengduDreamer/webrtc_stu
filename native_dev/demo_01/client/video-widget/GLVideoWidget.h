#pragma once

#include <QTimer>
#include <qwidget.h>

#include <QtOpenGLWidgets/qopenglwidget.h>
#include <QtOpenGL/qopenglfunctions_3_3_core.h>
#include <QtOpenGL/qopenglwindow.h>

#include <QResizeEvent>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <mutex>
#include <memory>

#include "RawImage.h"



class PlaceHolder : public QWidget {
public:

	PlaceHolder(QWidget* parent = nullptr);
	~PlaceHolder();

	void paintEvent(QPaintEvent* event) override;

	void SetPixmap(const QString& url);

private:

	QPixmap		pixmap;
};

class GLVideoWidget : public QOpenGLWidget, public QOpenGLFunctions_3_3_Core {
public:

	explicit GLVideoWidget(RawImageFormat format, QWidget* parent = nullptr);
	~GLVideoWidget();

	void UpdateNV12Image(std::shared_ptr<RawImage> image);
	void UpdateNV12Buffer(const char* y_buf, int y_buf_size, const char* uv_buf, int uv_buf_size, int width, int height);
	void UpdateRGBImage(std::shared_ptr<RawImage> image);
	void UpdateRGBBuffer(const char* buf, int width, int height, int channel);
	void UpdateI420Image(std::shared_ptr<RawImage> image);
	void UpdateI420Buffer(const char* y_buf, int y_buf_size, const char* u_buf, int u_buf_size, const char* v_buf, int v_buf_size, int width, int height);
	void ClearI420Image();

	void FlipXCoord();
	void ResetXCoord();

	void SetPlaceHolderIcon(const QString& url);
	void ShowPlaceHolder();
	void HidePlaceHolder();

	void SetPlaceholderRawImage(RawImagePtr img);
	void BindToUid(const QString& uid);
	QString GetBindUid();
	void UpdateLastTime();
	uint64_t GetLastUpdateTime();

protected:
	void resizeEvent(QResizeEvent* event) override;
	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int width, int height) override;

	void mouseMoveEvent(QMouseEvent*) override;
	void mousePressEvent(QMouseEvent*) override;
	void mouseReleaseEvent(QMouseEvent*) override;

private:
	void checkShaderError(GLuint id, const QString& type);
	void InitNV12Texture();
	void InitRGBATexture();
	void InitI420Texture();

	void ReleaseNV12Buffer();
	void ReleaseRGBBuffer();
	void ReleaseI420Buffer();

	void InitPlaceHolder();
	void ResetAsPlaceholder();

	void Update();

private:
	GLuint			shader{ 0 };
	GLuint			vao{ 0 };
	GLuint			vbo{ 0 };

	char*			y_buffer = nullptr;
	char*			uv_buffer = nullptr;
	GLuint			y_texture_id = 0;
	GLuint			uv_texture_id = 0;
	bool		    need_create_texture = true;
	int				tex_width = 0;
	int				tex_height = 0;
	// 如果是RGB，可能是RGB可能是RGBA，YUV不用这个变量
	int				tex_channel;
	char*			rgb_buffer = nullptr;
	GLuint			rgb_texture_id = 0;

	// I420
	char*			u_buffer = nullptr;
	char*			v_buffer = nullptr;
	GLuint			u_texture_id = 0;
	GLuint			v_texture_id = 0;

	std::mutex		buf_mtx;

	RawImageFormat  raw_image_format;

	QTimer*			timer = nullptr;

	bool			flip_x = false;

	PlaceHolder*    placeholder = nullptr;
	QLabel*		    placeholder_icon = nullptr;
	QString			placeholder_url;
	RawImagePtr     placeholder_raw_image = nullptr;

	QString			belong_to_uid;

	uint64_t		last_update_time = 0;
};