#ifndef LVGLCORE_HPP
#define LVGLCORE_HPP

/**
 * @file LVGLCore.h
 * @brief Central wrapper for the LVGL runtime, delegating to specialized managers.
 *
 * LVGLCore is the central entry point for the builder application. It initializes
 * the LVGL library, registers all widget types, and provides access to five
 * specialized managers: LVGLDisplay (framebuffer/rendering), LVGLImageManager,
 * LVGLFontManager, LVGLObjectManager (widget instance tracking), and
 * LVGLWidgetRegistry (widget type definitions).
 *
 * Most subsystems access it via the `lvgl` global variable (declared as
 * `extern LVGLCore lvgl`). Only one instance should exist at a time because
 * LVGL 6.x uses internal global state — multiple instances would conflict.
 */

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

/**
 * @class LVGLCore
 * @brief Central manager for the LVGL runtime, providing access to all subsystem managers.
 *
 * Delegates responsibilities to focused managers rather than implementing everything
 * directly. This decomposition was introduced to keep LVGLCore from becoming a
 * god object — each manager owns one concern (display, images, fonts, objects,
 * widget types).
 *
 * Typical usage:
 * @code
 *   lvgl.init(480, 320);                          // initialize display
 *   lvgl.widgetRegistry()->widget("lv_btn");       // look up widget type
 *   lvgl.addObject(obj);                           // register a new widget instance
 *   QPixmap fb = lvgl.framebuffer();               // get rendered frame
 * @endcode
 */
class LVGLCore : public QObject {
  Q_OBJECT
 public:
  LVGLCore(QObject *parent = nullptr);
  ~LVGLCore();

  /**
   * @brief Initializes the LVGL display, loads default assets, and registers all widget types.
   * @param width Display width in pixels.
   * @param height Display height in pixels.
   * Must be called once before any other operation. Sets up the framebuffer, loads
   * the default image, initializes built-in fonts, and registers all ~28 widget types
   * with the widget registry.
   */
  void init(int width, int height);

  /**
   * @brief Resizes the LVGL display to a new resolution.
   * @param size New display dimensions.
   * @return Currently always returns false (return value unused).
   */
  bool changeResolution(QSize size);

  /**
   * @brief Returns the current LVGL framebuffer as a QPixmap.
   * The framebuffer is rendered by LVGLDisplay and reflects the current state of
   * all LVGL objects. Used by LVGLSimulator for canvas painting.
   */
  QPixmap framebuffer() const;

  /**
   * @brief Captures a rectangular region of the framebuffer.
   * @param region The area to capture, in display coordinates.
   * @return QPixmap of the captured region.
   */
  QPixmap grab(const QRect &region) const;

  /**
   * @brief Finds a widget instance by its user-assigned name.
   * @param name The display name of the object.
   * @return Pointer to the matching LVGLObject, or nullptr if not found.
   */
  LVGLObject *findByName(const QString &name);

  /** @brief Display width in pixels. */
  int width() const;
  /** @brief Display height in pixels. */
  int height() const;
  /** @brief Display dimensions as QSize. */
  QSize size() const;

  /** @brief Access the display/framebuffer manager. */
  LVGLDisplay *display() const { return m_display; }
  /** @brief Access the image asset manager. */
  LVGLImageManager *imageManager() const { return m_imageManager; }
  /** @brief Access the font asset manager. */
  LVGLFontManager *fontManager() const { return m_fontManager; }
  /** @brief Access the widget instance tracker. */
  LVGLObjectManager *objectManager() const { return m_objectManager; }
  /** @brief Access the widget type registry. */
  LVGLWidgetRegistry *widgetRegistry() const { return m_widgetRegistry; }

  /**
   * @brief Adds an image from a QImage to the image manager.
   * @param image The source image.
   * @param name Display name for the image in the builder UI.
   * @return Pointer to the created LVGLImageData, owned by the image manager.
   */
  LVGLImageData *addImage(QImage image, QString name);

  /**
   * @brief Loads an image from a file path.
   * @param fileName Path to the image file.
   * @param name Optional display name; derived from filename if empty.
   * @return Pointer to the created LVGLImageData, or nullptr on failure.
   */
  LVGLImageData *addImage(QString fileName, QString name = QString());

  /** @brief Registers a pre-constructed LVGLImageData with the manager. */
  void addImage(LVGLImageData *image);
  /** @brief Returns display names of all registered images. */
  QStringList imageNames() const;
  /** @brief Returns all registered image data objects. */
  QList<LVGLImageData *> images() const;

  /**
   * @brief Looks up an LVGL image descriptor by name.
   * @param name The image display name.
   * @return Pointer to the lv_img_dsc_t, or nullptr if not found.
   */
  lv_img_dsc_t *image(QString name);
  /** @brief Returns the default placeholder image descriptor. */
  lv_img_dsc_t *defaultImage() const;
  /** @brief Reverse-looks up an image name from its LVGL descriptor. */
  QString nameByImage(const lv_img_dsc_t *img_dsc) const;
  /** @brief Reverse-looks up an LVGLImageData from its LVGL descriptor. */
  LVGLImageData *imageByDesc(const lv_img_dsc_t *img_dsc) const;
  /** @brief Removes and deletes an image. Returns true on success. */
  bool removeImage(LVGLImageData *img);
  /** @brief Removes and deletes all registered images. */
  void removeAllImages();

  /** @brief Returns display names of all LVGL built-in symbols. */
  QStringList symbolNames() const;

  /**
   * @brief Returns the LVGL symbol string for a given symbol name.
   * @param name Symbol display name (e.g., "Ok", "Close").
   * @return The LVGL symbol constant (e.g., LV_SYMBOL_OK), or nullptr.
   */
  const char *symbol(const QString &name) const;

  /**
   * @brief Advances the LVGL tick and task handler.
   * Called periodically (via timer) to let LVGL process animations, input events,
   * and pending redraws. After polling, the framebuffer reflects the updated display state.
   */
  void poll();

  /**
   * @brief Clears the internal object tracking list.
   * Only clears the LVGLObjectManager's list — does not call lv_obj_del() on the
   * tracked objects. Use removeAllObjects() for full cleanup.
   */
  void objsclear();

  /**
   * @brief Injects a mouse event into the LVGL input driver.
   * @param x X coordinate in display pixels.
   * @param y Y coordinate in display pixels.
   * @param pressed true if the mouse button is held down.
   */
  void sendMouseEvent(int x, int y, bool pressed);

  /**
   * @brief Computes the absolute screen position of an LVGL object.
   * @param lv_obj The LVGL object pointer.
   * @return Position relative to the display origin.
   * Walks up the parent chain to accumulate offsets. Needed because LVGL stores
   * positions relative to the parent, but Qt scene coordinates are absolute.
   */
  QPoint get_absolute_position(const lv_obj_t *lv_obj) const;

  /** @brief Returns the width and height of an LVGL object. */
  QSize get_object_size(const lv_obj_t *lv_obj) const;

  /** @brief Returns the absolute bounding rectangle of an LVGL object. */
  QRect get_object_rect(const lv_obj_t *lv_obj) const;

  /**
   * @brief Registers a widget instance with the object manager.
   * @param object The LVGLObject to track.
   * The manager takes ownership and will delete the object when removeObject() is called.
   */
  void addObject(LVGLObject *object);

  /**
   * @brief Recursively removes and deletes a widget and all its children.
   * @param object The widget to remove. Pointer is invalid after this call.
   */
  void removeObject(LVGLObject *object);

  /** @brief Recursively removes and deletes all tracked widget instances. */
  void removeAllObjects();

  /** @brief Replaces the entire object tracking list (used when switching screens). */
  void setAllObjects(QList<LVGLObject *> objs);

  /** @brief Returns all tracked widget instances (flat list, all levels). */
  QList<LVGLObject *> allObjects() const;
  /** @brief Returns only root-level widget instances (no parent). */
  QList<LVGLObject *> topLevelObjects() const;
  /** @brief Returns all widget instances of a given class name. */
  QList<LVGLObject *> objectsByType(QString className) const;
  /** @brief Finds a widget instance by name. @return The matching object, or nullptr. */
  LVGLObject *object(QString name) const;
  /** @brief Finds the LVGLObject wrapping a given lv_obj_t. @return The matching object, or nullptr. */
  LVGLObject *object(lv_obj_t *obj) const;

  /** @brief Converts an LVGL color to a Qt color. */
  QColor toColor(lv_color_t c) const;
  /** @brief Converts a Qt color to an LVGL color. */
  lv_color_t fromColor(QColor c) const;
  /** @brief Converts a QVariant (containing QColor) to an LVGL color. */
  lv_color_t fromColor(QVariant v) const;
  /** @brief Serializes an LVGL color to a JSON object with r/g/b fields. */
  QJsonObject colorToJson(lv_color_t c) const;
  /** @brief Deserializes an LVGL color from a JSON object with r/g/b fields. */
  lv_color_t colorFromJson(QJsonObject obj) const;

  /**
   * @brief Loads a font from a TTF file using FreeType and registers it.
   * @param fileName Path to the .ttf file.
   * @param size Font size in points.
   * @return Pointer to the created LVGLFontData, or nullptr on failure.
   */
  LVGLFontData *addFont(const QString &fileName, uint8_t size);

  /** @brief Registers a pre-constructed LVGLFontData. */
  void addFont(LVGLFontData *font);
  /** @brief Removes and deletes a custom font. Returns true on success. */
  bool removeFont(LVGLFontData *font);
  /** @brief Returns display names of all fonts (built-in + custom). */
  QStringList fontNames() const;
  /** @brief Returns C code names of all fonts. */
  QStringList fontCodeNames() const;
  /** @brief Looks up a font by its index in the font list. */
  const lv_font_t *font(int index) const;

  /**
   * @brief Looks up a font by its display name.
   * @param name Font display name.
   * @param cs Case sensitivity for the lookup.
   */
  const lv_font_t *font(const QString &name,
                        Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

  /** @brief Returns the index of a font in the font list. */
  int indexOfFont(const lv_font_t *font) const;
  /** @brief Returns the display name for a font. */
  QString fontName(const lv_font_t *font) const;
  /** @brief Returns the C code name for a font. */
  QString fontCodeName(const lv_font_t *font) const;
  /** @brief Returns all user-added (non-built-in) fonts. */
  QList<const LVGLFontData *> customFonts() const;
  /** @brief Removes all user-added fonts. */
  void removeCustomFonts();

  /**
   * @brief Returns the LVGL base style name (e.g., "lv_style_plain") for a style pointer.
   * @param style Pointer to an LVGL style.
   * @return The style's code name, or empty string if not a known base style.
   */
  QString baseStyleName(const lv_style_t *style) const;

  /** @brief Sets the screen background color. */
  void setScreenColor(QColor color);
  /** @brief Returns the current screen background color. */
  QColor screenColor() const;
  /** @brief Returns true if the screen color differs from the default. */
  bool screenColorChanged() const;

  /** @brief Returns all registered widget type definitions. */
  QList<const LVGLWidget *> widgets() const;

  /**
   * @brief Looks up a widget type by its LVGL class name.
   * @param name The LVGL class name (e.g., "lv_btn", "lv_label", "lv_page").
   * @return The widget type definition, or nullptr if not found.
   */
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

/**
 * @brief Union helpers for storing typed pointers in Qt model item data.
 *
 * Qt's model/view framework passes data as QVariant/qintptr. These unions
 * provide a safe, readable way to cast between typed pointers and qintptr
 * without reinterpret_cast scattered through the model code.
 */
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
