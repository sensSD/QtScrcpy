#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class QYUVOpenGLWidget: public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT

public:
  explicit QYUVOpenGLWidget(QWidget *parent = nullptr);
  ~QYUVOpenGLWidget();

public:
  /**
   * @brief 返回控件的推荐最小尺寸
   * 
   * @return QSize 
   */
  QSize minimumSizeHint() const override;

  /**
   * @brief 返回控件的推荐尺寸
   * 
   * @return QSize 
   */
  QSize sizeHint() const override;

  /**
   * @brief 设置视频帧的尺寸
   * 
   * @param size 视频帧的尺寸
   */
  void setFrameSize(const QSize& size);
  
  /**
   * @brief 获取视频帧的尺寸
   * 
   * @return const QSize& 
   */
  const QSize& frameSize() const;

  /**
   * @brief 渲染视频帧
   * 
   * @param dataY Y分量起始数据
   * @param dataU U分量起始数据
   * @param dataV V分量起始数据
   * @param lineSizeY Y分量行大小
   * @param lineSizeU U分量行大小
   * @param lineSizeV V分量行大小
   */
  void updateTextures(quint8* dataY, quint8* dataU, quint8* dataV, quint32 lineSizeY, quint32 lineSizeU, quint32 lineSizeV);

protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

private:
  /**
   * @brief 初始化着色器
   * 
   */
  void initShader();

  /**
   * @brief 初始化纹理
   * 
   */
  void initTextures();

  /**
   * @brief 销毁纹理
   * 
   */
  void deInitTextures();

  /**
   * @brief 更新纹理
   * 
   * @param texture 纹理ID
   * @param textureType 纹理类型（0：Y分量，1：U分量，2：V分量）
   * @param pixels 纹理数据
   * @param stride 行大小
   */
  void updateTexture(GLuint texture, quint32 textureType, quint8* pixels, quint32 stride);

private:
  // 视频帧尺寸
  QSize m_frameSize = {-1, -1};
  // 是否需要更新帧数据
  bool m_needUpdate = false;
  // 是否已经初始化纹理
  bool m_textureInitialized = false;

  // 顶点缓冲对象
  QOpenGLBuffer m_vbo;

  // 着色器程序
  QOpenGLShaderProgram m_shaderProgram;

  // YUV三个分量的纹理ID
  GLuint m_texture[3] = {0}; 
};
