#include "QYUVOpenGLWidget.h"

#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QOpenGLTexture>
#include <QSurfaceFormat>
#include <chrono>


// 顶点坐标和纹理坐标数据
static const GLfloat coordinate[] = {
    // 顶点坐标，存储4个xyz坐标
    // 坐标范围为[-1,1],中心点为 0,0
    // 二维图像z始终为0
    // GL_TRIANGLE_STRIP的绘制方式：
    // 使用前3个坐标绘制一个三角形，使用后三个坐标绘制一个三角形，正好为一个矩形
    // x     y     z
    -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,

    // 纹理坐标，存储4个xy坐标
    // 坐标范围为[0,1],左下角为 0,0
    0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f};

// 顶点着色器源码
static const QString s_vertShader = R"(
  attribute vec3 vertexIn;    // xyz顶点坐标
  attribute vec2 textureIn;   // xy纹理坐标

  varying vec2 textureOut;    // 传递给片段着色器的纹理坐标

  void main(void)
  {
    gl_Position = vec4(vertexIn, 1.0);
    textureOut = textureIn;
  }
)";

// 片段着色器源码
static QString s_fragShader = R"(
  varying vec2 textureOut;        // 由顶点着色器传递过来的纹理坐标

  uniform sampler2D textureY;     // uniform 纹理单元，利用纹理单元可以使用多个纹理
  uniform sampler2D textureU;     // sampler2D是2D采样器
  uniform sampler2D textureV;     // 声明yuv三个纹理单元

  void main(void)
  {
    vec3 yuv;
    vec3 rgb;

    // SDL2 BT709_SHADER_CONSTANTS
    const vec3 Rcoeff = vec3(1.1644,  0.000,  1.7927);
    const vec3 Gcoeff = vec3(1.1644, -0.2132, -0.5329);
    const vec3 Bcoeff = vec3(1.1644,  2.1124,  0.000);

    // 根据指定的纹理textureY和坐标textureOut来采样
    yuv.x = texture2D(textureY, textureOut).r;
    yuv.y = texture2D(textureU, textureOut).r - 0.5;
    yuv.z = texture2D(textureV, textureOut).r - 0.5;

    // 采样完转为rgb
    // 减少一些亮度
    yuv.x = yuv.x - 0.0625;
    rgb.r = dot(yuv, Rcoeff);
    rgb.g = dot(yuv, Gcoeff);
    rgb.b = dot(yuv, Bcoeff);
    // 输出颜色值
    gl_FragColor = vec4(rgb, 1.0);
  }
)";

QYUVOpenGLWidget::QYUVOpenGLWidget(QWidget* parent) : QOpenGLWidget(parent) {
}

QYUVOpenGLWidget::~QYUVOpenGLWidget() {
  makeCurrent();
  m_vbo.destroy();
  deInitTextures();
  doneCurrent();
}

QSize QYUVOpenGLWidget::minimumSizeHint() const {
  return QSize(50, 50);
}

QSize QYUVOpenGLWidget::sizeHint() const {
  return size();
}

void QYUVOpenGLWidget::setFrameSize(const QSize& size) {
  if (size != m_frameSize) {
    m_frameSize = size;
    m_needUpdate = true;
    repaint();  // 触发重绘
  }
}

const QSize& QYUVOpenGLWidget::frameSize() const {
  return m_frameSize;
}

void QYUVOpenGLWidget::updateTextures(quint8* dataY, quint8* dataU, quint8* dataV,
                                      quint32 lineSizeY, quint32 lineSizeU, quint32 lineSizeV) {
  if (m_textureInitialized) {
    updateTexture(m_texture[0], 0, dataY, lineSizeY);
    updateTexture(m_texture[1], 1, dataU, lineSizeU);
    updateTexture(m_texture[2], 2, dataV, lineSizeV);
    update();  // 更新ui
  }
}

void QYUVOpenGLWidget::initializeGL() {
  initializeOpenGLFunctions();

  // 关闭深度测试
  glDisable(GL_DEPTH_TEST);

  // 创建并绑定顶点缓冲对象
  m_vbo.create();
  m_vbo.bind();
  // 顶点数组复制到缓冲对象中
  m_vbo.allocate(coordinate, sizeof(coordinate));

  // 初始化着色器
  initShader();

  // 设置背景清理色为黑色
  glClearColor(0.0, 0.0, 0.0, 0.0);
  // 清理颜色背景
  glClear(GL_COLOR_BUFFER_BIT);
}

void QYUVOpenGLWidget::resizeGL(int w, int h) {
  glViewport(0, 0, w, h);
  repaint();
}

void QYUVOpenGLWidget::paintGL() {
  m_shaderProgram.bind();

  if (m_needUpdate) {
    deInitTextures();
    initTextures();
    m_needUpdate = false;
  }

  if (m_textureInitialized) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture[0]);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture[1]);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_texture[2]);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }

  m_shaderProgram.release();

  static QElapsedTimer timer;
  static bool first = true;
  if (first) {
    timer.start();
    first = false;
  }

  static int cnt = 0;
  if (++cnt % 30 == 0) {
    qDebug() << "[OpenGL] Avg paintGL time:" << timer.elapsed() / 30.0 << "ms";
    timer.restart();
    cnt = 0;
  }
}

void QYUVOpenGLWidget::initShader() {
  if (QCoreApplication::testAttribute(Qt::AA_UseOpenGLES)) {
    s_fragShader.prepend(R"(
      precision mediump int;
      precision mediump float;
      )");
  }

  // 编译并链接着色器程序
  m_shaderProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, s_vertShader);
  m_shaderProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, s_fragShader);
  m_shaderProgram.link();
  m_shaderProgram.bind();

  // 指定顶点坐标在vbo中的访问方式
  // 参数：顶点坐标在shader中的参数名称，顶点坐标为float，起始偏移，顶点坐标类型为vec3，步幅
  m_shaderProgram.setAttributeBuffer("vertexIn", GL_FLOAT, 0, 3, 3 * sizeof(float));
  // 启用顶点坐标属性数组
  m_shaderProgram.enableAttributeArray("vertexIn");

  // 指定纹理坐标在vbo中的访问方式
  m_shaderProgram.setAttributeBuffer("textureIn", GL_FLOAT, 12 * sizeof(float), 2,
                                     2 * sizeof(float));
  m_shaderProgram.enableAttributeArray("textureIn");

  // 设置纹理单元与shader中uniform变量的对应关系
  m_shaderProgram.setUniformValue("textureY", 0);
  m_shaderProgram.setUniformValue("textureU", 1);
  m_shaderProgram.setUniformValue("textureV", 2);
}

void QYUVOpenGLWidget::initTextures() {
  // YUV三个分量的大小：Y是全尺寸，U/V是半尺寸
  const QSize sizes[] = {m_frameSize, m_frameSize / 2, m_frameSize / 2};

  for (int i = 0; i < 3; ++i) {
    glGenTextures(1, &m_texture[i]);
    glBindTexture(GL_TEXTURE_2D, m_texture[i]);

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // 创建纹理
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, sizes[i].width(), sizes[i].height(), 0,
                 GL_LUMINANCE, GL_UNSIGNED_BYTE, nullptr);
  }

  m_textureInitialized = true;
}

void QYUVOpenGLWidget::deInitTextures() {
  if (QOpenGLFunctions::isInitialized(QOpenGLFunctions::d_ptr)) {
    glDeleteTextures(3, m_texture);
  }

  memset(m_texture, 0, sizeof(m_texture));
  m_textureInitialized = false;
}

void QYUVOpenGLWidget::updateTexture(GLuint texture, quint32 textureType, quint8* pixels,
                                     quint32 stride) {
  if (!pixels) return;

  QSize size = 0 == textureType ? m_frameSize : m_frameSize / 2;

  makeCurrent();
  glBindTexture(GL_TEXTURE_2D, texture);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, static_cast<GLint>(stride));
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.width(), size.height(), GL_LUMINANCE,
                  GL_UNSIGNED_BYTE, pixels);
  doneCurrent();
}