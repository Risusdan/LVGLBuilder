/**
 * @file LVGLWidget.h
 * @brief Abstract base class defining a widget type in the builder.
 *
 * Each LVGLWidget subclass represents one kind of LVGL widget (button, label,
 * slider, etc.). Subclasses define how to create the underlying lv_obj_t,
 * what properties and styles the widget supports, and how it appears in the
 * widget palette.
 *
 * To add a new widget type to the builder:
 * 1. Create an LVGLWidget subclass in widgets/ implementing the pure virtuals.
 * 2. Register it in LVGLCore::init() via registerWidget().
 * 3. Add the source files to CMakeLists.txt.
 */

#ifndef LVGLWIDGET_H
#define LVGLWIDGET_H

#include <QJsonObject>
#include <QPixmap>

#include "LVGLProperty.h"
#include "LVGLStyle.h"

/**
 * @class LVGLWidget
 * @brief Defines a widget type's creation logic, properties, and style interface.
 *
 * This is a type-level class (one instance per widget kind), not an
 * instance-level class. LVGLObject wraps individual widget instances.
 * The separation exists because type-level data (property definitions,
 * style declarations, icons) is shared across all instances of the same
 * widget kind.
 */
class LVGLWidget {
 public:
  LVGLWidget();
  virtual ~LVGLWidget();

  /**
   * @brief Enumerates all supported widget types.
   *
   * Used as a stable identifier for widget types across serialization,
   * code generation, and type-specific logic (e.g., special handling
   * for TabView pages).
   */
  enum Type {
    Arc,
    Bar,
    Button,
    ButtonMatrix,
    Calendar,
    Canvas,
    CheckBox,
    Chart,
    Container,
    ColorPicker,
    DropDownList,
    Gauge,
    Image,
    ImageButton,
    ImageSlider,
    ImageBar,
    Keyboard,
    Label,
    LED,
    Line,
    List,
    LineMeter,
    MessageBox,
    Page,
    Preloader,
    Roller,
    Slider,
    SpinBox,
    Switch,
    TabView,
    TextArea,
    TileView,
    Window,
    None
  };

  /** @brief Human-readable name shown in the widget palette (e.g., "Button"). */
  virtual QString name() const = 0;
  /** @brief LVGL C API class name (e.g., "lv_btn"). Used in code generation. */
  virtual QString className() const = 0;
  /** @brief Returns the Type enum value for this widget kind. */
  virtual Type type() const = 0;
  /** @brief Icon shown in the widget palette. */
  virtual QIcon icon() const = 0;
  /** @brief Minimum allowed dimensions when resizing on the canvas. */
  virtual QSize minimumSize() const = 0;

  /**
   * @brief Creates a new lv_obj_t of this widget type.
   * @param parent The LVGL parent object.
   * @return The newly created LVGL object handle.
   *
   * Each subclass calls the appropriate lv_xxx_create() function.
   * The returned lv_obj_t is then wrapped by LVGLObject.
   */
  virtual lv_obj_t *newObject(lv_obj_t *parent) const = 0;

  /**
   * @brief Returns the names of style types this widget supports.
   *
   * For example, a button returns {"LV_BTN_STYLE_REL", "LV_BTN_STYLE_PR", ...}.
   * The index into this list corresponds to the style type parameter used
   * in style()/setStyle()/defaultStyle().
   */
  virtual QStringList styles() const = 0;

  /** @brief Gets the current style for the given type index from an lv_obj_t. */
  virtual lv_style_t *style(lv_obj_t *obj, int type) const = 0;
  /** @brief Applies a style to an lv_obj_t for the given type index. */
  virtual void setStyle(lv_obj_t *obj, int type, lv_style_t *style) const = 0;
  /** @brief Returns the LVGL default style for the given type index. */
  virtual lv_style_t *defaultStyle(int type) const = 0;

  /** @brief Returns a preview pixmap for the widget palette. */
  virtual QPixmap preview() const;
  /** @brief Sets the cached preview pixmap. */
  void setPreview(QPixmap preview);

  /** @brief Returns all properties available for this widget type. */
  QVector<LVGLProperty *> properties() const;

  /**
   * @brief Returns all properties except Name, Accessible, Locked, and Geometry.
   *
   * Used during C code generation for TabView page objects. TabView pages
   * are created by lv_tabview_add_tab() rather than the normal create path,
   * so they skip regular property generation and use this filtered list instead.
   */
  QList<LVGLProperty *> specialProperties() const;

  /** @brief Looks up a property by name. */
  LVGLProperty *property(QString name) const;

  /**
   * @brief Returns which style parts (body, text, image, line) are editable for a style type.
   * @param type Style type index.
   */
  LVGL::StyleParts editableStyles(int type) const;

  /** @brief Returns the geometry property (position + size), shared by all widget types. */
  LVGLProperty *geometryProp() const;

 protected:
  LVGLProperty *m_geometryProp;
  QVector<LVGLProperty *> m_properties;
  QList<LVGL::StyleParts> m_editableStyles;
  QPixmap m_preview;
};
Q_DECLARE_METATYPE(LVGLWidget *)
Q_DECLARE_METATYPE(const LVGLWidget *)

#endif  // LVGLWIDGET_H
