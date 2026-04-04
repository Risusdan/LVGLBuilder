#ifndef LVGLCORE_HPP
#define LVGLCORE_HPP

#include <lvgl/lvgl.h>

#include <QHash>
#include <QList>
#include <QObject>
#include <QPixmap>

#include "LVGLDisplay.h"
#include "LVGLImageData.h"
#include "LVGLImageManager.h"
#include "LVGLFontManager.h"
#include "LVGLObjectManager.h"
#include "LVGLWidgetRegistry.h"
#include "widgets/LVGLWidget.h"

class LVGLObject;
class LVGLFontData;

class LVGLCore : public QObject {
  Q_OBJECT
 public:
  LVGLCore(QObject *parent = nullptr);
  ~LVGLCore();

  void init(int width, int height);

  bool changeResolution(QSize size);

  QPixmap framebuffer() const;
  QPixmap grab(const QRect &region) const;

  LVGLObject *findByName(const QString &name);

  int width() const;
  int height() const;
  QSize size() const;

  LVGLDisplay *display() const { return m_display; }
  LVGLImageManager *imageManager() const { return m_imageManager; }
  LVGLFontManager *fontManager() const { return m_fontManager; }
  LVGLObjectManager *objectManager() const { return m_objectManager; }
  LVGLWidgetRegistry *widgetRegistry() const { return m_widgetRegistry; }

  LVGLImageData *addImage(QImage image, QString name);
  LVGLImageData *addImage(QString fileName, QString name = QString());
  void addImage(LVGLImageData *image);
  QStringList imageNames() const;
  QList<LVGLImageData *> images() const;
  lv_img_dsc_t *image(QString name);
  lv_img_dsc_t *defaultImage() const;
  QString nameByImage(const lv_img_dsc_t *img_dsc) const;
  LVGLImageData *imageByDesc(const lv_img_dsc_t *img_dsc) const;
  bool removeImage(LVGLImageData *img);
  void removeAllImages();

  QStringList symbolNames() const;
  const char *symbol(const QString &name) const;

  void poll();
  void objsclear();
  void sendMouseEvent(int x, int y, bool pressed);

  QPoint get_absolute_position(const lv_obj_t *lv_obj) const;
  QSize get_object_size(const lv_obj_t *lv_obj) const;
  QRect get_object_rect(const lv_obj_t *lv_obj) const;

  void addObject(LVGLObject *object);
  void removeObject(LVGLObject *object);
  void removeAllObjects();

  void setAllObjects(QList<LVGLObject *> objs);

  QList<LVGLObject *> allObjects() const;
  QList<LVGLObject *> topLevelObjects() const;
  QList<LVGLObject *> objectsByType(QString className) const;
  LVGLObject *object(QString name) const;
  LVGLObject *object(lv_obj_t *obj) const;

  QColor toColor(lv_color_t c) const;
  lv_color_t fromColor(QColor c) const;
  lv_color_t fromColor(QVariant v) const;
  QJsonObject colorToJson(lv_color_t c) const;
  lv_color_t colorFromJson(QJsonObject obj) const;

  LVGLFontData *addFont(const QString &fileName, uint8_t size);
  void addFont(LVGLFontData *font);
  bool removeFont(LVGLFontData *font);
  QStringList fontNames() const;
  QStringList fontCodeNames() const;
  const lv_font_t *font(int index) const;
  const lv_font_t *font(const QString &name,
                        Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
  int indexOfFont(const lv_font_t *font) const;
  QString fontName(const lv_font_t *font) const;
  QString fontCodeName(const lv_font_t *font) const;
  QList<const LVGLFontData *> customFonts() const;
  void removeCustomFonts();

  QString baseStyleName(const lv_style_t *style) const;

  void setScreenColor(QColor color);
  QColor screenColor() const;
  bool screenColorChanged() const;

  QList<const LVGLWidget *> widgets() const;
  const LVGLWidget *widget(const QString &name) const;

  static const char *DEFAULT_DAYS[7];
  static const char *DEFAULT_MONTHS[12];

 private:
  void registerWidget(LVGLWidget *w);

  LVGLDisplay *m_display;
  LVGLImageManager *m_imageManager;
  LVGLFontManager *m_fontManager;
  LVGLObjectManager *m_objectManager;
  LVGLWidgetRegistry *m_widgetRegistry;
};

extern LVGLCore lvgl;

// cast helpers for internal models
union LVGLImageDataCast {
  LVGLImageData *ptr;
  qintptr i;
};

union LVGLFontDataCast {
  LVGLFontData *ptr;
  qintptr i;
};

union LVGLWidgetCast {
  LVGLWidget *ptr;
  qintptr i;
};

union LVGLObjectCast {
  LVGLObject *ptr;
  qintptr i;
};

#endif  // LVGLCORE_HPP
