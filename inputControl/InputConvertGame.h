#pragma once

#include <QObject>
#include <QPointF>

#include "InputConvertNormal.h"

#define MULTI_TOUCH_MAX_NUM 10

class InputConvertGame : public QObject, public InputConvertNormal {
  Q_OBJECT

 public:
  explicit InputConvertGame(QObject* parent = nullptr);
  ~InputConvertGame() override = default;

 public:
  virtual void mouseEvent(const QMouseEvent* from, const QSize& frameSize,
                          const QSize& showSize) override;
  virtual void wheelEvent(const QWheelEvent* from, const QSize& frameSize,
                          const QSize& showSize) override;
  virtual void keyEvent(const QKeyEvent* from, const QSize& frameSize,
                        const QSize& showSize) override;

 signals:
  /**
   * @brief 抓取鼠标信号，用于控制鼠标指针
   *
   */
  void grabCursor(bool grab);

 protected:
  /**
   * @brief 更新窗口大小
   *
   * @param frameSize
   * @param showSize
   */
  void updateSize(const QSize& frameSize, const QSize& showSize);

  /**
   * @brief 发送触摸按下事件
   *
   * @param id
   * @param pos
   */
  void sendTouchDownEvent(int id, QPointF pos);

  /**
   * @brief 发送触摸移动事件
   *
   * @param id
   * @param pos
   */
  void sendTouchMoveEvent(int id, QPointF pos);

  /**
   * @brief 发送触摸抬起事件
   *
   * @param id
   * @param pos
   */
  void sendTouchUpEvent(int id, QPointF pos);

  /**
   * @brief 触摸事件
   *
   * @param id
   * @param pos
   * @param action
   */
  void sendTouchEvent(int id, QPointF pos, AndroidMotioneventAction action);

  /**
   * @brief 计算触摸点在游戏窗口中的绝对坐标
   *
   * @param relativePos
   * @return QPointF
   */
  QPointF calcFrameAbsolutePos(QPointF relativePos);

  // 多点触控ID管理
  /**
   * @brief 绑定触摸点ID
   *
   * @param key
   * @return int
   */
  int attachTouchID(int key);

  /**
   * @brief 解绑触摸点ID
   *
   * @param key
   */
  void detachTouchID(int key);

  /**
   * @brief 获取触摸点ID
   *
   * @param key
   * @return int
   */
  int getTouchID(int key);

  // 游戏摇杆方向盘功能
  /**
   * @brief 是否为方向键
   *
   * @param from
   * @return true
   * @return false
   */
  bool isSteerWheelKeys(const QKeyEvent* from);

  /**
   * @brief 处理摇杆方向盘操作
   *
   * @param from
   */
  void processSteerWheel(const QKeyEvent* from);

  /**
   * @brief 更新摇杆按键状态
   *
   * @param from
   * @param keyPress1
   * @param keyPress2
   * @return int
   */
  int updateSteerWheelKeysPress(const QKeyEvent* from, int& keyPress1, int& keyPress2);

  /**
   * @brief 摇杆移动操作
   *
   * @param keysNum
   * @param keyPress1
   * @param keyPress2
   */
  void steerWheelMove(int keysNum, int keyPress1, int keyPress2);

  // 键盘点击处理
  /**
   * @brief 处理键盘点击
   *
   * @param from
   * @return true
   * @return false
   */
  bool processKeyClick(const QKeyEvent* from);

  // 鼠标处理
  /**
   * @brief 处理鼠标点击
   *
   * @param from
   * @return true
   * @return false
   */
  bool processMouseClick(const QMouseEvent* from);

  /**
   * @brief 处理鼠标移动
   *
   * @param from
   * @return true
   * @return false
   */
  bool processMouseMove(const QMouseEvent* from);

  /**
   * @brief 移动鼠标指针到起始位置
   *
   * @param from
   */
  void moveCursorToStart(const QMouseEvent* from);

  /**
   * @brief 移动鼠标指针到指定位置
   *
   * @param from
   * @param pos
   */
  void moveCursorTo(const QMouseEvent* from, const QPoint& pos);

  /**
   * @brief 启动鼠标移动定时器
   *
   */
  void startMouseMoveTimer();

  /**
   * @brief 停止鼠标移动定时器
   *
   */
  void stopMouseMoveTimer();

  /**
   * @brief 开始鼠标移动触摸
   *
   * @param from
   */
  void mouseMoveStartTouch(const QMouseEvent* from);

  /**
   * @brief 结束鼠标移动触摸
   *
   */
  void mouseMoveStopTouch();

  /**
   * @brief 切换游戏映射模式
   *
   * @return true
   * @return false
   */
  bool switchGameMap();

  /**
   * @brief 检查鼠标位置
   *
   * @param from
   * @return true
   * @return false
   */
  bool checkCursorPos(const QMouseEvent* from);

  /**
   * @brief 处理滑动超时
   *
   * @param event
   */
  void timerEvent(QTimerEvent* event) override;

 private:
  enum SteerWheelDirection {
    SWD_UP = 0,
    SWD_RIGHT,
    SWD_DOWN,
    SWD_LEFT,
  };

  QSize m_frameSize;
  QSize m_showSize;

  // 映射模式标准
  bool m_gameMap = false;

  // 多点触控ID
  int multiTouchID[MULTI_TOUCH_MAX_NUM] = {0};

  // 摇杆方向盘位置和偏移
  QPointF m_steerWheelPos = {0.16f, 0.75f};
  QRectF m_steerWheelOffset = {QPointF(0.1f, 0.27f), QPointF(0.1f, 0.2f)};

  // 方向盘按键及状态
  int m_steerWheelKeys[4] = {Qt::Key_W, Qt::Key_D, Qt::Key_S, Qt::Key_A};
  bool m_steerWheelKeysPress[4] = {false};
  int m_steerWheelKeysNum = 0;
  int m_steerWheelFirstTouchKey = 0;

  // 鼠标移动相关变量
  QPointF m_mouseMoveStartPos = {0.57f, 0.26f};
  QPointF m_mouseMoveLastConverPos = m_mouseMoveStartPos;
  QPointF m_mouseMoveLastPos = {0.0f, 0.0f};
  bool m_mouseMovePress = false;
  int m_mouseMoveTimer = 0;

  bool m_needSwitchGameAgain = false;
};
