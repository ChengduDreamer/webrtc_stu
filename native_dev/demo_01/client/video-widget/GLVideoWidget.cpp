#include <QDebug>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>

#include <fstream>
#include <iostream>

#include "GLVideoWidget.h"
#include "GLVideoShader.h"

PlaceHolder::PlaceHolder(QWidget* parent) : QWidget(parent) {

}

PlaceHolder::~PlaceHolder() {

}

void PlaceHolder::paintEvent(QPaintEvent* event) {
	QWidget::paintEvent(event);

	QPainter painter(this);

	if (!pixmap.isNull()) {
		painter.setPen(Qt::NoPen);
		painter.drawPixmap((width() - pixmap.width()) / 2, (height() - pixmap.height()) / 2, pixmap);
	}
}

void PlaceHolder::SetPixmap(const QString& url) {
	if (url.isEmpty()) {
		return;
	}
	pixmap = QPixmap(url);
}

/// ---

GLVideoWidget::GLVideoWidget(RawImageFormat format, QWidget* parent) : QOpenGLWidget(parent)
{
	raw_image_format = format;
	timer = new QTimer(this);
	QObject::connect(timer, &QTimer::timeout, this, [=]() {
		this->update();
	});
	timer->start(40);

	InitPlaceHolder();
}

GLVideoWidget::~GLVideoWidget()
{
	if (timer && timer->isActive()) {
		timer->stop(); 
	}

	ReleaseNV12Buffer();
	ReleaseRGBBuffer();
	ReleaseI420Buffer();

	makeCurrent();
	if (vao) {
		glDeleteVertexArrays(1, &vao);
	}
	if (vbo) {
		glDeleteBuffers(1, &vbo);
	}
	if (shader) {
		glDeleteProgram(shader);
	}
	if (y_texture_id) {
		glDeleteTextures(1, &y_texture_id);
	}
	if (uv_texture_id) {
		glDeleteTextures(1, &uv_texture_id);
	}
	if (u_texture_id) {
		glDeleteTextures(1, &u_texture_id);
	}
	if (v_texture_id) {
		glDeleteTextures(1, &v_texture_id);
	}
	if (rgb_texture_id) {
		glDeleteTextures(1, &rgb_texture_id);
	}
	doneCurrent();
}

void GLVideoWidget::initializeGL()
{
	initializeOpenGLFunctions();

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &kVertexShader, NULL);
	glCompileShader(vertex_shader);
	checkShaderError(vertex_shader, "vertex");

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	if (raw_image_format == RawImageFormat::kNV12) {
		glShaderSource(fragment_shader, 1, &kNV12FragmentShader, NULL);
	}
	else if (raw_image_format == RawImageFormat::kRGB || raw_image_format == RawImageFormat::kRGBA) {
		glShaderSource(fragment_shader, 1, &kRGBFragmentShader, NULL);
	}
	else if (raw_image_format == RawImageFormat::kI420) {
		glShaderSource(fragment_shader, 1, &kI420FragmentShader, NULL);
	}

	glCompileShader(fragment_shader);
	checkShaderError(fragment_shader, "fragment");

	shader = glCreateProgram();

	glAttachShader(shader, vertex_shader);
	glAttachShader(shader, fragment_shader);

	glLinkProgram(shader);

	float vertices[] = {
		-1.0f, -1.0f, 0.0f, 1.0f, 0, 0,  0, 0,
		1.0f, -1.0f, 0.0f, 0, 1.0f, 0,  1, 0,
		1.0f,  1.0f, 0.0f, 0, 0, 1.0f,  1, 1,
		-1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0, 0, 1
	};

	int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	int posLoc = glGetAttribLocation(shader, "aPos");
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, false, 8 * 4, 0);
	glEnableVertexAttribArray(posLoc);

	int colorLoc = glGetAttribLocation(shader, "aColor");
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, false, 8 * 4, (void*)(3 * 4));
	glEnableVertexAttribArray(colorLoc);

	int texLoc = glGetAttribLocation(shader, "aTex");
	glVertexAttribPointer(texLoc, 2, GL_FLOAT, false, 8 * 4, (void*)(6 * 4));
	glEnableVertexAttribArray(texLoc);

	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

}

void GLVideoWidget::UpdateNV12Image(RawImagePtr image) {
	if (!image || !image->img_buf || image->img_format != RawImageFormat::kNV12
		|| image->img_buf_size <= 0) {
		return;
	}

	int y_size = image->img_width * image->img_height;
	UpdateNV12Buffer(image->Data(), y_size, image->Data() + y_size, y_size / 2, image->img_width, image->img_height);
}

void GLVideoWidget::UpdateNV12Buffer(const char* y_buf, int y_buf_size, const char* uv_buf, int uv_buf_size, int width, int height) {
	std::lock_guard<std::mutex> guard(buf_mtx);
	if (width != tex_width || height != tex_height) {
		ReleaseNV12Buffer();
	}
	if (!y_buffer) {
		y_buffer = (char*)malloc(y_buf_size);
	}
	if (!uv_buffer) {
		uv_buffer = (char*)malloc(uv_buf_size);
	}

	if (tex_width != width || tex_height != height) {
		need_create_texture = true;
	}
	memcpy(y_buffer, y_buf, y_buf_size);
	memcpy(uv_buffer, uv_buf, uv_buf_size);
	tex_width = width;
	tex_height = height;
}

void GLVideoWidget::UpdateRGBImage(std::shared_ptr<RawImage> image) {
	assert(image->Size() == image->img_width * image->img_height * image->img_ch);
	UpdateRGBBuffer(image->Data(), image->img_width, image->img_height, image->img_ch);
}

void GLVideoWidget::UpdateRGBBuffer(const char* buf, int width, int height, int channel) {
	std::lock_guard<std::mutex> guard(buf_mtx);
	int size = width * height * channel;
	if (width != tex_width || height != tex_height) {
		ReleaseRGBBuffer();
	}
	if (!rgb_buffer) {
		rgb_buffer = (char*)malloc(size);
	}
	if (tex_width != width || tex_height != height) {
		need_create_texture = true;
	}
	memcpy(rgb_buffer, buf, size);
	tex_width = width;
	tex_height = height;
	tex_channel = channel;
}

void GLVideoWidget::ClearI420Image() {
	std::lock_guard<std::mutex> guard(buf_mtx);
	if (tex_width <= 0 || tex_height <= 0) {
		return;
	}
	if (!y_buffer || !u_buffer || !v_buffer) {
		return;
	}

	int y_buf_size = tex_width * tex_height;
	int u_buf_size = y_buf_size / 4;
	int v_buf_size = y_buf_size / 4;
	if (!placeholder_raw_image) {
		memset(y_buffer, 0, y_buf_size);
		memset(u_buffer, 0, u_buf_size);
		memset(v_buffer, 0, v_buf_size);
	}
	else {
		if (placeholder_raw_image->img_width == tex_width && placeholder_raw_image->img_height == tex_height) {
			memcpy(y_buffer, placeholder_raw_image->Data(), y_buf_size);
			memcpy(u_buffer, placeholder_raw_image->Data() + y_buf_size, u_buf_size);
			memcpy(u_buffer, placeholder_raw_image->Data() + y_buf_size + u_buf_size, v_buf_size);
		}
	}
}

void GLVideoWidget::UpdateI420Image(std::shared_ptr<RawImage> image) {
	int y_buf_size = image->img_width * image->img_height;
	int uv_buf_size = y_buf_size / 4;
	char* buf = image->Data();

	UpdateI420Buffer(buf, y_buf_size, // y
		buf + y_buf_size, uv_buf_size, // u
		buf + y_buf_size + uv_buf_size, uv_buf_size, // v
		image->img_width, image->img_height
	);
}

void GLVideoWidget::UpdateI420Buffer(const char* y_buf, int y_buf_size,
	const char* u_buf, int u_buf_size,
	const char* v_buf, int v_buf_size,
	int width, int height) {
	
	if (width != tex_width || height != tex_height) {
		ReleaseI420Buffer();
	}

	std::lock_guard<std::mutex> guard(buf_mtx);
	if (!y_buffer) {
		y_buffer = (char*)malloc(y_buf_size);
	}
	if (!u_buffer) {
		u_buffer = (char*)malloc(u_buf_size);
	}
	if (!v_buffer) {
		v_buffer = (char*)malloc(v_buf_size);
	}

	if (tex_width != width || tex_height != height) {
		need_create_texture = true;
	}
	memcpy(y_buffer, y_buf, y_buf_size);
	memcpy(u_buffer, u_buf, u_buf_size);
	memcpy(v_buffer, v_buf, v_buf_size);
	tex_width = width;
	tex_height = height;
}

void GLVideoWidget::resizeEvent(QResizeEvent* event) {
	QOpenGLWidget::resizeEvent(event);
}

void GLVideoWidget::paintGL() {

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(vao);
	glUseProgram(shader);

	if (raw_image_format == RawImageFormat::kNV12) {

		std::lock_guard<std::mutex> guard(buf_mtx);

		// 如果还没创建texture，在这里创建或者texture大小变了，重新创建
		if (y_buffer && uv_buffer && need_create_texture) {
			need_create_texture = false;
			InitNV12Texture();
		}
		if (y_buffer) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, y_texture_id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, tex_width, tex_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, y_buffer);
		}
		if (uv_buffer) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, uv_texture_id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, tex_width / 2, tex_height / 2, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, uv_buffer);
		}
	}
	else if (raw_image_format == RawImageFormat::kRGBA || raw_image_format == RawImageFormat::kRGB) {

		std::lock_guard<std::mutex> guard(buf_mtx);

		if (rgb_buffer && need_create_texture) {
			need_create_texture = false;
			InitRGBATexture();
		}
		if (rgb_buffer) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, rgb_texture_id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgb_buffer);
		}
	}
	else if (raw_image_format == RawImageFormat::kI420) {

		std::lock_guard<std::mutex> guard(buf_mtx);

		if (y_buffer && u_buffer && v_buffer && need_create_texture) {
			need_create_texture = false;
			InitI420Texture();
		}

		if (y_buffer) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, y_texture_id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, tex_width, tex_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, y_buffer);
		}
		if (u_buffer) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, u_texture_id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, tex_width / 2, tex_height / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, u_buffer);
		}
		if (v_buffer) {
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, v_texture_id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, tex_width / 2, tex_height / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, v_buffer);
		}
	}

	if (raw_image_format == RawImageFormat::kRGB || raw_image_format == RawImageFormat::kRGBA) {
		glUniform1i(glGetUniformLocation(shader, "image1"), 0);
	}
	else if (raw_image_format == RawImageFormat::kNV12) {
		glUniform1i(glGetUniformLocation(shader, "image1"), 0);
		glUniform1i(glGetUniformLocation(shader, "image2"), 1);
	}
	else if (raw_image_format == RawImageFormat::kI420) {
		glUniform1i(glGetUniformLocation(shader, "imageY"), 0);
		glUniform1i(glGetUniformLocation(shader, "imageU"), 1);
		glUniform1i(glGetUniformLocation(shader, "imageV"), 2);
	}

	if (flip_x) {
		glUniform1i(glGetUniformLocation(shader, "flipX"), 1);
	}
	else {
		glUniform1i(glGetUniformLocation(shader, "flipX"), 0);
	}

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void GLVideoWidget::InitNV12Texture() {
	if (y_texture_id) {
		glDeleteTextures(1, &y_texture_id);
	}
	if (uv_texture_id) {
		glDeleteTextures(1, &uv_texture_id);
	}
	glGenTextures(1, &y_texture_id);
	glBindTexture(GL_TEXTURE_2D, y_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, tex_width, tex_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, nullptr);

	glGenTextures(1, &uv_texture_id);
	glBindTexture(GL_TEXTURE_2D, uv_texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, tex_width / 2, tex_height / 2, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void GLVideoWidget::InitRGBATexture() {
	if (rgb_texture_id) {
		glDeleteTextures(1, &rgb_texture_id);
	}
	glGenTextures(1, &rgb_texture_id);
	glBindTexture(GL_TEXTURE_2D, rgb_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
}

void GLVideoWidget::InitI420Texture() {
	auto create_luminance_texture = [this](GLuint& tex_id, int width, int height, bool is_uv) {
		glGenTextures(1, &tex_id);
		glBindTexture(GL_TEXTURE_2D, tex_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, is_uv ? width / 2 : width, is_uv ? height / 2 : height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	};
	if (y_texture_id) {
		glDeleteTextures(1, &y_texture_id);
	}
	if (u_texture_id) {
		glDeleteTextures(1, &u_texture_id);
	}
	if (v_texture_id) {
		glDeleteTextures(1, &v_texture_id);
	}
	create_luminance_texture(y_texture_id, tex_width, tex_height, false);
	create_luminance_texture(u_texture_id, tex_width, tex_height, true);
	create_luminance_texture(v_texture_id, tex_width, tex_height, true);
}

void GLVideoWidget::resizeGL(int width, int height) {
	glViewport(0, 0, width, height);

	placeholder->resize(width, height);
	placeholder->setGeometry(0, 0, width, height);
}

void GLVideoWidget::FlipXCoord() {
	flip_x = true;
}

void GLVideoWidget::ResetXCoord() {
	flip_x = false;
}

void GLVideoWidget::checkShaderError(GLuint id, const QString& type) {
	int check_flag;
	char check_info[1024];
	if (type != "program") {
		glGetShaderiv(id, GL_COMPILE_STATUS, &check_flag);
		if (!check_flag) {
			glGetShaderInfoLog(id, 1024, NULL, check_info);
			std::cout << type.toStdString() << " error:" << check_info << std::endl;;
		}
	}
	else {
		glGetShaderiv(id, GL_LINK_STATUS, &check_flag);
		if (!check_flag) {
			glGetProgramInfoLog(id, 1024, NULL, check_info);
			std::cout << type.toStdString() << " error:" << check_info << std::endl;
		}
	}
}

void GLVideoWidget::ReleaseNV12Buffer() {
	if (y_buffer) {
		free(y_buffer);
		y_buffer = nullptr;
	}
	if (uv_buffer) {
		free(uv_buffer);
		uv_buffer = nullptr;
	}
}

void GLVideoWidget::ReleaseRGBBuffer() {
	if (rgb_buffer) {
		free(rgb_buffer);
		rgb_buffer = nullptr;
	}
}

void GLVideoWidget::ReleaseI420Buffer() {
	std::lock_guard<std::mutex> guard(buf_mtx);
	if (y_buffer) {
		free(y_buffer);
		y_buffer = nullptr;
	}
	if (u_buffer) {
		free(u_buffer);
		u_buffer = nullptr;
	}
	if (v_buffer) {
		free(v_buffer);
		v_buffer = nullptr;
	}
}

void GLVideoWidget::mouseMoveEvent(QMouseEvent* e) {
	QOpenGLWidget::mouseMoveEvent(e);
}

void GLVideoWidget::mousePressEvent(QMouseEvent* e) {
	QOpenGLWidget::mousePressEvent(e);
}

void GLVideoWidget::mouseReleaseEvent(QMouseEvent* e) {
	QOpenGLWidget::mouseReleaseEvent(e);
}

void GLVideoWidget::SetPlaceHolderIcon(const QString& url) {
	if (placeholder_icon) {
		placeholder_icon->setPixmap(QPixmap(url));
	}
	placeholder_url = url;
	placeholder->SetPixmap(placeholder_url);
}

void GLVideoWidget::ShowPlaceHolder() {
	placeholder->setVisible(true);
}

void GLVideoWidget::HidePlaceHolder() {
	placeholder->setVisible(false);
}

void GLVideoWidget::InitPlaceHolder() {
	placeholder = new PlaceHolder(this);
	placeholder->setStyleSheet(R"(QWidget{background-color:#424242; })");
	placeholder->SetPixmap(placeholder_url);
	placeholder->resize(width(), height());
	 
	//placeholder_icon = new SKLabel(placeholder);
	//placeholder_icon->skSetFixedSize(QSize(54, 54));
	//auto v_layout = new QVBoxLayout();
	//v_layout->setSpacing(0);
	//v_layout->setMargin(0);
	//v_layout->addStretch();
	//
	//auto h_layout = new QHBoxLayout();
	//h_layout->setSpacing(0);
	//h_layout->setMargin(0);
	//h_layout->addStretch();
	//h_layout->addWidget(placeholder_icon);
	//h_layout->addStretch();

	//v_layout->addLayout(h_layout);

	//v_layout->addStretch();
	//placeholder->setLayout(v_layout);

	HidePlaceHolder();
}

void GLVideoWidget::SetPlaceholderRawImage(RawImagePtr img) {
	placeholder_raw_image = img;
	UpdateI420Image(img);
}

void GLVideoWidget::ResetAsPlaceholder() {

}

void GLVideoWidget::BindToUid(const QString& uid) {
	belong_to_uid = uid;
}

QString GLVideoWidget::GetBindUid() {
	return belong_to_uid;
}

uint64_t GLVideoWidget::GetLastUpdateTime() {
	return last_update_time;
}

void GLVideoWidget::UpdateLastTime() {
	last_update_time = QDateTime::currentDateTime().toMSecsSinceEpoch();
}

void GLVideoWidget::Update() {
	QMetaObject::invokeMethod(this, [this]() {
		this->update();
	});
}