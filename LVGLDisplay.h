#ifndef LVGLDISPLAY_H
#define LVGLDISPLAY_H

/**
 * @file LVGLDisplay.h
 * @brief LVGL display driver wrapper managing the framebuffer and input driver.
 */

#include <lvgl/lvgl.h>

#include <QColor>
#include <QElapsedTimer>
#include <QJsonObject>
#include <QObject>
#include <QPixmap>
#include <QTimer>
#include <vector>

/**
 * @class LVGLDisplay
 * @brief Owns the LVGL display buffer, input driver, and low-level display operations.
 *
 * Manages the lv_color_t framebuffer that LVGL renders into (converted to
 * QPixmap on demand via framebuffer()), the display and input driver
 * registration, resolution changes, screen color, color conversions between
 * Qt and LVGL formats, and symbol name lookups. Extracted from LVGLCore to
 * isolate display concerns.
 */
class LVGLDisplay : public QObject {
  Q_OBJECT
 public:
  explicit LVGLDisplay(QObject *parent = nullptr);
  ~LVGLDisplay();

  void init(int width, int height);

  bool changeResolution(QSize size);

  QPixmap framebuffer() const;
  QPixmap grab(const QRect &region) const;

  int width() const;
  int height() const;
  QSize size() const;

  void poll();
  void objsclear();
  void sendMouseEvent(int x, int y, bool pressed);

  QPoint get_absolute_position(const lv_obj_t *lv_obj) const;
  QSize get_object_size(const lv_obj_t *lv_obj) const;
  QRect get_object_rect(const lv_obj_t *lv_obj) const;

  void setScreenColor(QColor color);
  QColor screenColor() const;
  bool screenColorChanged() const;

  QString baseStyleName(const lv_style_t *style) const;

  QColor toColor(lv_color_t c) const;
  lv_color_t fromColor(QColor c) const;
  lv_color_t fromColor(QVariant v) const;
  QJsonObject colorToJson(lv_color_t c) const;
  lv_color_t colorFromJson(QJsonObject obj) const;

  QStringList symbolNames() const;
  const char *symbol(const QString &name) const;

 private slots:
  void tick();

 private:
  void flushHandler(lv_disp_drv_t *disp, const lv_area_t *area,
                    lv_color_t *color_p);
  bool inputHandler(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);

  static void flushCb(lv_disp_drv_t *disp, const lv_area_t *area,
                      lv_color_t *color_p);
  static bool inputCb(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
  static void logCb(lv_log_level_t level, const char *file, uint32_t line,
                    const char *dsc);

  QTimer m_timer;
  QElapsedTimer m_time;
  lv_style_t m_screenStyle;

  std::vector<lv_color_t> m_dispFrameBuf;
  std::vector<lv_color_t> m_buf1;
  std::vector<lv_color_t> m_buf2;
  lv_disp_buf_t m_dispBuf;
  lv_disp_drv_t m_dispDrv;

  lv_indev_data_t m_inputData;
};

#endif  // LVGLDISPLAY_H
