# Doxygen In-Source Documentation Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add Doxygen-format comments to LVGLBuilder source files so maintainers and new contributors can understand what each component is, how to use it, and why it's designed that way.

**Architecture:** Top-down approach — fully document 8 core classes first (file + class + public methods + design rationale), then add file+class docs to 13 supporting classes, then one-liner file comments to ~140 widget/property subclass files.

**Tech Stack:** C++, Doxygen comment syntax (`/** */`), no build tooling.

---

## File Map

**Tier 1 — Full documentation (file + class + public methods + rationale):**
- `LVGLCore.h` — Global LVGL runtime wrapper
- `LVGLObject.h` — Widget instance wrapper
- `widgets/LVGLWidget.h` — Abstract widget type base
- `LVGLProperty.h` — Abstract property base + template classes
- `LVGLSimulator.h` — Canvas rendering and interactive editing
- `LVGLItem.h` — QGraphicsItem for widget display
- `MainWindow.h` — Main application window
- `LVGLProject.h` — Project save/load/export

**Tier 2 — File + class level docs:**
- `LVGLPropertyModel.h` (includes LVGLPropertyDelegate)
- `LVGLObjectModel.h`
- `LVGLWidgetModel.h`
- `LVGLStyle.h`
- `LVGLStyleModel.h`
- `LVGLFontData.h`
- `LVGLImageData.h`
- `LVGLDisplay.h`
- `LVGLImageManager.h`
- `LVGLFontManager.h`
- `LVGLObjectManager.h`
- `LVGLWidgetRegistry.h`
- `LVGLWorker.h`
- `LVGLDialog.h`
- `LVGLFontDialog.h`
- `LVGLNewDialog.h`
- `LVGLTabWidget.h`
- `LVGLWidgetListView.h`
- `ProjectManager.h`
- `RecentFilesManager.h`
- `AssetManager.h`
- `CanvasActions.h`
- `DragDropHandler.h`
- `SelectionManager.h`

**Tier 3 — One-liner file comments:**
- `widgets/*.h` — 28 widget subclass headers (excluding LVGLWidget.h and LVGLWidgets.h)
- `properties/*.h` — 112 property subclass headers
- `widgets/LVGLWidgets.h` — aggregate include header

---

### Task 1: Document LVGLCore.h

**Files:**
- Modify: `LVGLCore.h`

- [ ] **Step 1: Add file-level and class-level documentation**

Add at the top of the file, after the header guard:

```cpp
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
```

Add before the class declaration:

```cpp
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
```

- [ ] **Step 2: Add method-level documentation for initialization and rendering**

Document these methods in LVGLCore.h:

```cpp
  /**
   * @brief Initializes the LVGL display, loads default assets, and registers all widget types.
   * @param width Display width in pixels.
   * @param height Display height in pixels.
   *
   * Must be called once before any other operation. Sets up the framebuffer,
   * loads the default image, initializes built-in fonts, and registers all
   * ~28 widget types with the widget registry.
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
   *
   * The framebuffer is rendered by LVGLDisplay and reflects the current
   * state of all LVGL objects. Used by LVGLSimulator for canvas painting.
   */
  QPixmap framebuffer() const;

  /**
   * @brief Captures a rectangular region of the framebuffer.
   * @param region The area to capture, in display coordinates.
   * @return QPixmap of the captured region.
   */
  QPixmap grab(const QRect &region) const;
```

- [ ] **Step 3: Add method-level documentation for object management**

```cpp
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
  LVGLDisplay *display() const;
  /** @brief Access the image asset manager. */
  LVGLImageManager *imageManager() const;
  /** @brief Access the font asset manager. */
  LVGLFontManager *fontManager() const;
  /** @brief Access the widget instance tracker. */
  LVGLObjectManager *objectManager() const;
  /** @brief Access the widget type registry. */
  LVGLWidgetRegistry *widgetRegistry() const;
```

- [ ] **Step 4: Add method-level documentation for image, symbol, and polling methods**

```cpp
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
   *
   * Called periodically (via timer) to let LVGL process animations,
   * input events, and pending redraws. After polling, the framebuffer
   * reflects the updated display state.
   */
  void poll();

  /**
   * @brief Clears the internal object tracking list.
   *
   * Only clears the LVGLObjectManager's list — does not call lv_obj_del()
   * on the tracked objects. Use removeAllObjects() for full cleanup.
   */
  void objsclear();

  /**
   * @brief Injects a mouse event into the LVGL input driver.
   * @param x X coordinate in display pixels.
   * @param y Y coordinate in display pixels.
   * @param pressed true if the mouse button is held down.
   */
  void sendMouseEvent(int x, int y, bool pressed);
```

- [ ] **Step 5: Add method-level documentation for geometry helpers and object accessors**

```cpp
  /**
   * @brief Computes the absolute screen position of an LVGL object.
   * @param lv_obj The LVGL object pointer.
   * @return Position relative to the display origin.
   *
   * Walks up the parent chain to accumulate offsets. Needed because LVGL
   * stores positions relative to the parent, but Qt scene coordinates are absolute.
   */
  QPoint get_absolute_position(const lv_obj_t *lv_obj) const;

  /** @brief Returns the width and height of an LVGL object. */
  QSize get_object_size(const lv_obj_t *lv_obj) const;

  /** @brief Returns the absolute bounding rectangle of an LVGL object. */
  QRect get_object_rect(const lv_obj_t *lv_obj) const;

  /**
   * @brief Registers a widget instance with the object manager.
   * @param object The LVGLObject to track. The manager takes ownership and
   *               will delete the object when removeObject() is called.
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

  /**
   * @brief Finds a widget instance by name.
   * @return The matching object, or nullptr.
   */
  LVGLObject *object(QString name) const;

  /**
   * @brief Finds the LVGLObject wrapping a given lv_obj_t.
   * @return The matching object, or nullptr.
   */
  LVGLObject *object(lv_obj_t *obj) const;
```

- [ ] **Step 6: Add method-level documentation for color, font, style, and screen methods**

```cpp
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
  const lv_font_t *font(const QString &name, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
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
```

- [ ] **Step 7: Add documentation for the cast unions at end of file**

```cpp
/**
 * @brief Union helpers for storing typed pointers in Qt model item data.
 *
 * Qt's model/view framework passes data as QVariant/qintptr. These unions
 * provide a safe, readable way to cast between typed pointers and qintptr
 * without reinterpret_cast scattered through the model code.
 */
```

- [ ] **Step 8: Commit**

```bash
git add LVGLCore.h
git commit -m "docs: add Doxygen documentation to LVGLCore.h"
```

---

### Task 2: Document LVGLObject.h

**Files:**
- Modify: `LVGLObject.h`

- [ ] **Step 1: Add file-level and class-level documentation**

```cpp
/**
 * @file LVGLObject.h
 * @brief Wrapper around lv_obj_t that adds Qt integration, hierarchy, and serialization.
 *
 * Every widget placed on the canvas is represented by an LVGLObject. It bridges
 * the gap between LVGL's C-based object system and Qt's object model, providing
 * name management, parent-child tracking, property access, JSON serialization,
 * and C code generation.
 */
```

```cpp
/**
 * @class LVGLObject
 * @brief Represents a single widget instance on the builder canvas.
 *
 * Wraps an lv_obj_t (the LVGL C widget handle) and layers Qt functionality
 * on top: named identity, hierarchical parent-child relationships, style
 * management, and the ability to serialize to JSON or generate C source code.
 *
 * LVGLObject does not subclass lv_obj_t — LVGL is a C library, so this class
 * acts as a companion object that owns and manages the lv_obj_t lifetime.
 * The implicit conversion operators allow passing LVGLObject directly to
 * LVGL C functions that expect lv_obj_t*.
 *
 * Four constructors exist to handle different creation contexts:
 * - From widget palette (new object with LVGLObject parent)
 * - From widget palette (new object with raw lv_obj_t parent, for root objects)
 * - Internal creation with explicit LVGL parent (currently unused)
 * - Wrapping a pre-existing lv_obj_t (used by TabView for page sub-objects)
 */
```

- [ ] **Step 2: Add method-level documentation for constructors and identity**

```cpp
  /**
   * @brief Creates a new widget as a child of an existing LVGLObject.
   * @param widgetClass The widget type definition (e.g., LVGLButton).
   * @param name Display name; auto-generated if empty.
   * @param parent The parent LVGLObject in the widget tree.
   *
   * This is the primary constructor for user-created widgets via drag-and-drop.
   */
  LVGLObject(const LVGLWidget *widgetClass, QString name, LVGLObject *parent);

  /**
   * @brief Creates a new root-level widget with a raw LVGL parent.
   * @param widgetClass The widget type definition.
   * @param name Display name; auto-generated if empty.
   * @param parent The raw lv_obj_t parent (typically the LVGL screen object).
   *
   * Used for top-level widgets that have no LVGLObject parent.
   */
  LVGLObject(const LVGLWidget *widgetClass, QString name, lv_obj_t *parent);

  /**
   * @brief Creates a sub-object with an explicit LVGL parent object.
   * @param widgetClass The widget type definition.
   * @param parent The logical parent LVGLObject.
   * @param parentObj The raw lv_obj_t to use as LVGL parent (may differ from parent->obj()).
   *
   * Currently unused in the codebase. Intended for cases where the LVGL
   * parent container differs from the logical LVGLObject parent.
   */
  LVGLObject(const LVGLWidget *widgetClass, LVGLObject *parent, lv_obj_t *parentObj);

  /**
   * @brief Wraps a pre-existing lv_obj_t as an LVGLObject.
   * @param obj The pre-existing LVGL object to wrap (not created by this constructor).
   * @param widgetClass The widget type definition.
   * @param parent The parent LVGLObject, or nullptr for root.
   * @param movable Whether the object can be dragged on the canvas.
   * @param index Sub-object index (used by TabView pages); -1 if not applicable.
   *
   * Used by LVGLPropertyTabs to wrap TabView page objects that are created
   * by lv_tabview_add_tab() rather than the normal newObject() path.
   */
  LVGLObject(lv_obj_t *obj, const LVGLWidget *widgetClass, LVGLObject *parent, bool movable = true, int index = -1);

  /** @brief Returns the underlying LVGL object handle. */
  lv_obj_t *obj() const;

  /** @brief Returns the user-assigned display name. */
  QString name() const;
  /** @brief Sets the display name. */
  void setName(const QString &name);
  /**
   * @brief Returns the C-safe identifier for code generation.
   *
   * Lowercases the display name, replaces non-alphanumeric characters
   * with underscores, and prepends '_' if the result starts with a digit.
   */
  QString codeName() const;
```

- [ ] **Step 3: Add method-level documentation for state, type, and geometry**

```cpp
  /** @brief Returns true if the object is locked (cannot be selected or moved). */
  bool isLocked() const;
  /** @brief Sets the locked state. */
  void setLocked(bool locked);
  /** @brief Returns true if the object can be dragged on the canvas. */
  bool isMovable() const;

  /** @brief Returns the widget type definition for this instance. */
  const LVGLWidget *widgetClass() const;
  /** @brief Returns the widget type enum value. */
  LVGLWidget::Type widgetType() const;

  /** @brief Returns the absolute position on the display (accumulated from parents). */
  QPoint absolutePosition() const;
  /** @brief Returns the position relative to the parent widget. */
  QPoint position() const;
  /** @brief X coordinate relative to parent. */
  int x() const;
  /** @brief Y coordinate relative to parent. */
  int y() const;
  /** @brief Moves the widget to a new position relative to its parent. */
  void setPosition(const QPoint &pos);
  /** @brief Sets X coordinate relative to parent. */
  void setX(int x);
  /** @brief Sets Y coordinate relative to parent. */
  void setY(int y);

  /** @brief Returns the widget dimensions. */
  QSize size() const;
  /** @brief Returns position + size as a QRect. */
  QRect geometry() const;
  /** @brief Sets both position and size. */
  void setGeometry(QRect geometry);
  /** @brief Widget width in pixels. */
  int width() const;
  /** @brief Widget height in pixels. */
  int height() const;
  /** @brief Sets widget width. */
  void setWidth(int width);
  /** @brief Sets widget height. */
  void setHeight(int height);
```

- [ ] **Step 4: Add method-level documentation for serialization, styles, and hierarchy**

```cpp
  /**
   * @brief Serializes the widget and its children to a JSON object.
   *
   * Includes widget type, name, geometry, properties, styles, and
   * recursively serializes all child objects.
   */
  QJsonObject toJson();

  /** @brief Returns the custom styles as a JSON array. */
  QJsonArray jsonStyles() const;

  /**
   * @brief Generates C code lines for setting a custom style on this widget.
   * @param styleVar The C variable name for the style struct.
   * @param type The style type index.
   * @return List of C code lines (e.g., "style_btn_main.body.main_color = ...").
   */
  QStringList codeStyle(QString styleVar, int type) const;

  /** @brief Restores custom styles from a JSON array (during project load). */
  void parseStyles(const QJsonArray &styles);

  /** @brief Returns the parent LVGLObject, or nullptr for root objects. */
  LVGLObject *parent() const;

  /** @brief Implicit conversion to lv_obj_t* for use with LVGL C API. */
  operator lv_obj_t *() noexcept;
  /** @brief Implicit const conversion to lv_obj_t* for use with LVGL C API. */
  operator const lv_obj_t *() const noexcept;

  /** @brief Returns true if this widget has a custom (non-default) style for the given type. */
  bool hasCustomStyle(int type) const;
  /**
   * @brief Returns or creates a custom style for the given type index.
   * @param type The style type index (widget-specific, e.g., 0 = main style).
   * @return Pointer to the style struct, created on first access.
   */
  lv_style_t *style(int type);
  /** @brief Returns the C variable name for this widget's style. */
  QString styleCodeName(int type) const;

  /** @brief Returns the list of child LVGLObjects. */
  QList<LVGLObject *> childs() const;
  /** @brief Removes a child from this object's child list. */
  void removeChild(LVGLObject *child);

  /** @brief Returns true if this object is exported as a global variable in generated C code. */
  bool isAccessible() const;
  /** @brief Sets whether this object is exported as a global variable. */
  void setAccessible(bool accessible);

  /** @brief Returns true if another object already uses this name. */
  bool doesNameExists() const;
  /** @brief Static version: checks if any object (except `except`) uses the given name. */
  static bool doesNameExists(const QString &name, LVGLObject *except = nullptr);
  /** @brief Auto-generates a unique name based on the widget type. */
  void generateName();

  /**
   * @brief Deserializes a widget tree from JSON.
   * @param object The JSON object representing a single widget.
   * @param parent The parent LVGLObject, or nullptr for root widgets.
   * @return The reconstructed LVGLObject with all children.
   */
  static LVGLObject *parse(QJsonObject object, LVGLObject *parent);

  /** @brief Returns the sub-object index (used by TabView pages); -1 if not a sub-object. */
  int index() const;
  /** @brief Finds a child by its sub-object index. */
  LVGLObject *findChildByIndex(int index) const;
```

- [ ] **Step 5: Commit**

```bash
git add LVGLObject.h
git commit -m "docs: add Doxygen documentation to LVGLObject.h"
```

---

### Task 3: Document LVGLWidget.h

**Files:**
- Modify: `widgets/LVGLWidget.h`

- [ ] **Step 1: Add file-level and class-level documentation**

```cpp
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
```

```cpp
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
```

- [ ] **Step 2: Add documentation for the Type enum and pure virtual methods**

```cpp
  /**
   * @brief Enumerates all supported widget types.
   *
   * Used as a stable identifier for widget types across serialization,
   * code generation, and type-specific logic (e.g., special handling
   * for TabView pages).
   */
  enum Type { /* ... existing values ... */ };

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
```

- [ ] **Step 3: Add documentation for non-virtual methods and members**

```cpp
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
```

- [ ] **Step 4: Commit**

```bash
git add widgets/LVGLWidget.h
git commit -m "docs: add Doxygen documentation to LVGLWidget.h"
```

---

### Task 4: Document LVGLProperty.h

**Files:**
- Modify: `LVGLProperty.h`

- [ ] **Step 1: Add file-level documentation**

```cpp
/**
 * @file LVGLProperty.h
 * @brief Abstract property system for widget configuration in the builder.
 *
 * Defines the LVGLProperty base class and a family of typed subclasses that
 * handle getting/setting widget properties, providing editor widgets for the
 * property inspector, JSON serialization, and C code generation.
 *
 * The property system uses a composite pattern: an LVGLProperty can have
 * child properties (e.g., a "Geometry" property contains X, Y, Width, Height
 * sub-properties). This maps directly to the tree structure shown in the
 * property inspector panel.
 *
 * Template subclasses (LVGLPropertyType<T>, LVGLPropertyValT<T>) provide
 * type-safe get/set so that property implementations work with concrete C++
 * types (bool, int, QString, lv_coord_t) rather than QVariant. The
 * value()/setValue() bridge in the template handles QVariant conversion.
 */
```

- [ ] **Step 2: Add class-level documentation for LVGLProperty base**

```cpp
/**
 * @class LVGLProperty
 * @brief Abstract base class for all editable widget properties.
 *
 * Provides the interface for:
 * - Reading/writing property values on an LVGLObject
 * - Creating editor widgets for the property inspector
 * - Serializing to/from JSON for project save/load
 * - Generating C code for export
 *
 * Properties form a tree structure (parent/children) that mirrors the
 * display in the property inspector. Leaf properties do actual get/set;
 * branch properties group related leaves (e.g., "Geometry" groups X/Y/W/H).
 */
```

- [ ] **Step 3: Add method-level documentation for LVGLProperty**

```cpp
  /** @brief Returns the property display name shown in the inspector. */
  virtual QString name() const = 0;

  /** @brief Returns true if this property provides an editor widget. */
  virtual bool hasEditor() const;
  /**
   * @brief Creates the editor widget for the property inspector.
   * @param parent The parent QWidget for the editor.
   * @return The editor widget (combo box, spin box, line edit, etc.).
   */
  virtual QWidget *editor(QWidget *parent);
  /** @brief Updates the editor widget to reflect the current object's value. */
  virtual void updateEditor(LVGLObject *obj);
  /** @brief Reads the editor widget and applies the value to the object. */
  virtual void updateWidget(LVGLObject *obj);

  /** @brief Returns the default value for this property. */
  virtual QVariant defaultValue() const;
  /** @brief Reads the current property value from an object. */
  virtual QVariant value(LVGLObject *obj) const;
  /** @brief Writes a property value to an object. */
  virtual void setValue(LVGLObject *obj, QVariant value);

  /** @brief Serializes the property value to JSON. */
  virtual QJsonValue toJson(LVGLObject *obj) const;

  /** @brief Parent property in the tree, or nullptr for top-level. */
  const LVGLProperty *parent() const;
  /** @brief Number of child properties. */
  int count() const;
  /** @brief Child property at the given index. */
  const LVGLProperty *child(int index) const;
  /** @brief Row index within the parent's child list. */
  int row() const;
  /** @brief Index of a child property. */
  int indexOf(const LVGLProperty *item) const;

  /**
   * @brief Generates C code lines for setting this property.
   * @param obj The widget instance.
   * @return List of C function call strings (e.g., "lv_btn_set_toggle(btn1, true);").
   *
   * Base implementation returns an empty list. Subclasses return an empty
   * list when no function name was provided at construction time.
   */
  virtual QStringList function(LVGLObject *obj) const;
```

- [ ] **Step 4: Add class-level documentation for template and typed subclasses**

```cpp
/**
 * @class LVGLPropertyType
 * @brief Template base that provides type-safe get/set for properties with a known C++ type.
 * @tparam T The native value type (bool, int, lv_coord_t, QString, etc.).
 *
 * Subclasses implement get() and set() with the concrete type T instead of
 * QVariant. The value()/setValue() bridge handles QVariant conversion.
 */

/**
 * @class LVGLPropertyEnum
 * @brief Property backed by a list of string options, displayed as a combo box.
 *
 * Used for properties like alignment, layout mode, and other fixed-choice
 * settings. The enum text list maps 1:1 to integer values.
 */

/**
 * @class LVGLPropertyCoord
 * @brief Coordinate property with a spin box editor bounded by display dimensions.
 *
 * Can be oriented horizontally (bounded by display width) or vertically
 * (bounded by display height).
 */

/**
 * @class LVGLPropertyInt
 * @brief Integer property with configurable min/max range and optional suffix.
 */

/**
 * @class LVGLPropertyFont
 * @brief Font selection property displayed as a combo box of registered fonts.
 */

/**
 * @class LVGLPropertyValT
 * @brief Template property that wraps an LVGL getter/setter function pair.
 * @tparam T The value type (int16_t, uint16_t).
 *
 * Constructed with lambdas for the getter and setter, plus a function name
 * for code generation. This avoids creating a dedicated subclass for every
 * simple LVGL property — the lambda captures handle the specifics.
 */

/**
 * @class LVGLPropertyValInt16
 * @brief Convenience subclass of LVGLPropertyValT<int16_t> with an explicit constructor.
 */

/**
 * @class LVGLPropertyValUInt16
 * @brief Convenience subclass of LVGLPropertyValT<uint16_t> with an explicit constructor.
 */

/**
 * @class LVGLPropertyBool
 * @brief Boolean property displayed as a True/False combo box.
 *
 * Supports both direct subclass override (via virtual get/set) and
 * lambda-based construction for simple LVGL flag properties.
 */

/**
 * @class LVGLPropertyString
 * @brief String property displayed as a line edit.
 *
 * Like LVGLPropertyBool, supports both subclass override and lambda
 * construction. Handles conversion between QString and const char*
 * for the LVGL C API.
 */
```

- [ ] **Step 5: Commit**

```bash
git add LVGLProperty.h
git commit -m "docs: add Doxygen documentation to LVGLProperty.h"
```

---

### Task 5: Document LVGLSimulator.h

**Files:**
- Modify: `LVGLSimulator.h`

- [ ] **Step 1: Add file-level documentation**

```cpp
/**
 * @file LVGLSimulator.h
 * @brief Canvas view for rendering the LVGL display and handling interactive editing.
 *
 * Contains three classes:
 * - LVGLScene: QGraphicsScene that paints the LVGL framebuffer as the background.
 * - LVGLSimulator: QGraphicsView that manages widget selection, dragging, and
 *   drop-to-create from the palette.
 * - LVGLKeyPressEventFilter: Event filter for keyboard shortcuts (delete, arrow keys).
 */
```

- [ ] **Step 2: Add class-level and method-level documentation for LVGLScene**

```cpp
/**
 * @class LVGLScene
 * @brief QGraphicsScene subclass that renders the LVGL framebuffer as its background.
 *
 * Overrides drawBackground() to poll LVGL (advancing tick/tasks), paint the
 * current framebuffer, and draw selection/hover rectangles over active objects.
 */
```

Document public methods:

```cpp
  /** @brief Returns the currently selected LVGLObject, or nullptr. */
  LVGLObject *selected() const;
  /** @brief Returns the object currently under the mouse cursor, or nullptr. */
  LVGLObject *hoverObject() const;
  /** @brief Sets the hover-tracked object (for visual feedback). */
  void setHoverObject(LVGLObject *hoverObject);
  /** @brief Sets the selected object and triggers scene update. */
  void setSelected(LVGLObject *selected);
```

- [ ] **Step 3: Add class-level and method-level documentation for LVGLSimulator**

```cpp
/**
 * @class LVGLSimulator
 * @brief Main canvas view — renders the LVGL display and handles all user interaction.
 *
 * Coordinates three concerns:
 * - **Rendering:** Periodically polls LVGLCore and repaints the scene with the
 *   updated framebuffer.
 * - **Selection and dragging:** In design mode, handles mouse events to select,
 *   move, and resize widgets. Hit-testing uses SelectionManager (Qt-side geometry
 *   checks), not LVGL. Drag state is tracked manually (start position + delta)
 *   to coordinate between LVGLItem resize and whole-object moves.
 * - **Run mode:** When mouse is enabled, forwards mouse coordinates to LVGL
 *   via sendMouseEvent() so the UI can be interacted with as on a real device.
 * - **Widget creation:** Accepts drag-and-drop from the widget palette (LVGLWidgetModel)
 *   to create new widget instances on the canvas.
 *
 * Delegates specific responsibilities to helper classes: CanvasActions (object
 * creation, deletion, and movement), SelectionManager (hit-testing and selection
 * tracking), and DragDropHandler (palette drop processing).
 */
```

Document key public methods:

```cpp
  /** @brief Returns the currently selected LVGLObject, or nullptr. */
  LVGLObject *selectedObject() const;
  /** @brief Returns the LVGLItem graphics item. */
  LVGLItem *item() const;
  /** @brief Returns the object manipulation handler (create, delete, move). */
  CanvasActions *canvasActions() const;
  /** @brief Returns the selection and hit-testing manager. */
  SelectionManager *selectionManager() const;

  /**
   * @brief Moves a widget by a relative offset.
   * @param obj The widget to move.
   * @param dx Horizontal offset in pixels.
   * @param dy Vertical offset in pixels.
   */
  void moveObject(LVGLObject *obj, int dx, int dy);

  /** @brief Registers a widget with the object tracker and notifies the object model. */
  void addObject(LVGLObject *obj);
  /** @brief Deletes a widget and notifies the object model. Clears selection first. */
  void removeObject(LVGLObject *obj);

  /** @brief Connects the object tree model for insert/remove notifications. */
  void setObjectModel(LVGLObjectModel *objectModel);
  /** @brief Connects the property model for selection-triggered updates. */
  void setPropertyModel(LVGLPropertyModel *propertyModel);
  /**
   * @brief Sets the active LVGL screen and parent for new widget creation.
   * @param parent The lv_obj_t screen to activate via lv_scr_load().
   *
   * Called when switching between screens/tabs. Also stores the parent
   * so that newly drag-dropped widgets are created under it.
   */
  void setobjParent(lv_obj_t *parent);

  /** @brief Selects a widget on the canvas and notifies listeners. */
  void setSelectedObject(LVGLObject *obj);
  /** @brief Sets the canvas zoom level (percentage). */
  void setZoomLevel(int level);
  /** @brief Clears all widgets from the canvas. */
  void clear();
  /** @brief Enables or disables mouse input to the LVGL display (for run mode). */
  void setMouseEnable(bool enable);
  /** @brief Resizes the canvas to match a new display resolution. */
  void changeResolution(QSize size);
```

- [ ] **Step 4: Add class-level documentation for LVGLKeyPressEventFilter**

```cpp
/**
 * @class LVGLKeyPressEventFilter
 * @brief Application-level event filter for keyboard shortcuts on the canvas.
 *
 * Installed on the QApplication but only active when the LVGLSimulator has
 * focus. Handles Delete (remove widget), arrow keys (nudge selected widget
 * by 1px), and Ctrl+C/X/V (clipboard operations).
 */
```

- [ ] **Step 5: Commit**

```bash
git add LVGLSimulator.h
git commit -m "docs: add Doxygen documentation to LVGLSimulator.h"
```

---

### Task 6: Document LVGLItem.h

**Files:**
- Modify: `LVGLItem.h`

- [ ] **Step 1: Add file-level, class-level, and method-level documentation**

```cpp
/**
 * @file LVGLItem.h
 * @brief QGraphicsItem that displays an LVGLObject on the canvas with resize handles.
 *
 * LVGLItem is the visual representation of a selected widget in the
 * QGraphicsScene. It draws selection handles around the widget bounds and
 * translates mouse interactions into position/size changes on the underlying
 * LVGLObject.
 */
```

```cpp
/**
 * @class LVGLItem
 * @brief Graphics item providing selection visualization and interactive resizing.
 *
 * Inherits from both QObject (for signals/slots) and QGraphicsItem (for
 * scene rendering). Tracks which edges/corners the user is dragging via
 * the ResizeDirections struct to support 8-directional resizing.
 *
 * The item does not render the widget itself — the LVGL framebuffer handles
 * that. LVGLItem only draws the selection rectangle and resize handles on top.
 */
```

Document public methods:

```cpp
  /** @brief Returns the bounding rect, matching the associated LVGLObject's geometry. */
  QRectF boundingRect() const override;

  /**
   * @brief Paints the selection handles and border around the widget.
   *
   * Only draws when the item's object is selected. The widget content
   * itself comes from the LVGL framebuffer painted by LVGLScene.
   */
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

  /** @brief Returns the associated LVGLObject. */
  LVGLObject *object() const;
  /** @brief Sets the LVGLObject this item represents. */
  void setObject(LVGLObject *obj);
  /** @brief Returns true if the user is currently resizing via a handle. */
  bool isManipolating() const;

  /** @brief Emitted when the object's geometry changes due to user interaction. */
  // signal: geometryChanged()

  /** @brief Synchronizes the item's position and size with the LVGLObject. */
  // slot: updateGeometry()
```

Document the ResizeDirections struct:

```cpp
  /**
   * @brief Tracks which edges the user is dragging during a resize operation.
   *
   * Supports independent horizontal (Left/Right/None) and vertical
   * (Top/Bottom/None) directions, enabling 8-directional resize from
   * any edge or corner.
   */
  struct ResizeDirections { /* ... */ };
```

- [ ] **Step 2: Commit**

```bash
git add LVGLItem.h
git commit -m "docs: add Doxygen documentation to LVGLItem.h"
```

---

### Task 7: Document MainWindow.h

**Files:**
- Modify: `MainWindow.h`

- [ ] **Step 1: Add file-level and class-level documentation**

```cpp
/**
 * @file MainWindow.h
 * @brief Main application window coordinating all UI panels and project operations.
 */
```

```cpp
/**
 * @class MainWindow
 * @brief Top-level window that assembles the builder UI and wires components together.
 *
 * Owns and coordinates:
 * - Widget palette (left panel, driven by LVGLWidgetModel)
 * - Canvas simulator (center, LVGLSimulator)
 * - Property inspector (right panel, driven by LVGLPropertyModel)
 * - Object tree (right panel, driven by LVGLObjectModel)
 * - Style editor (right panel, driven by LVGLStyleModel)
 * - Image and font asset lists (right panel tabs)
 *
 * Also manages project-level operations (new, load, save, export) via
 * ProjectManager and tracks recent files via RecentFilesManager.
 */
```

- [ ] **Step 2: Add method-level documentation for key public and private methods**

```cpp
  /** @brief Returns the canvas simulator widget. */
  LVGLSimulator *simulator() const;

  /** @brief Updates the property inspector when a property value changes. */
  void updateProperty();
  /** @brief Handles object selection — updates inspector, style editor, and object tree. */
  void setCurrentObject(LVGLObject *obj);
  /** @brief Refreshes the style editor when style values change. */
  void styleChanged();
  /** @brief Creates a new empty project with a resolution dialog. */
  void openNewProject();

  /**
   * @brief Loads a project from a JSON file.
   * @param fileName Path to the .json project file.
   */
  void loadProject(const QString &fileName);

  /** @brief Enables or disables the builder UI panels (disabled during run mode). */
  void setEnableBuilder(bool enable);
```

- [ ] **Step 3: Commit**

```bash
git add MainWindow.h
git commit -m "docs: add Doxygen documentation to MainWindow.h"
```

---

### Task 8: Document LVGLProject.h

**Files:**
- Modify: `LVGLProject.h`

- [ ] **Step 1: Add file-level, class-level, and method-level documentation**

```cpp
/**
 * @file LVGLProject.h
 * @brief Project metadata and persistence — save/load JSON, export C code.
 */
```

```cpp
/**
 * @class LVGLProject
 * @brief Holds project metadata and implements file I/O.
 *
 * A project encapsulates a name and display resolution. Persistence is handled
 * via a JSON format (version 1) that includes the widget tree, custom styles,
 * image assets, and font assets. C code export generates a .h/.c pair with
 * lv_xxx_create() calls and style setup.
 *
 * The JSON file format:
 * @code
 * {
 *   "version": 1,
 *   "lvgl": { "name": "...", "resolution": {...}, "widgets": [...] },
 *   "images": [...],
 *   "fonts": [...]
 * }
 * @endcode
 */
```

```cpp
  /** @brief Default constructor — creates "App" at max LVGL resolution. */
  LVGLProject();
  /** @brief Constructs a project with the given name and resolution. */
  LVGLProject(const QString &name, QSize resolution);

  /** @brief Returns the project name. */
  QString name() const;
  /** @brief Returns the display resolution. */
  QSize resolution() const;

  /**
   * @brief Loads a project from a JSON file.
   * @param fileName Path to the .json file.
   * @return The loaded project, or nullptr on failure (invalid format, unsupported version).
   *
   * Restores images, fonts, screen color, and the full widget tree.
   * Shows an error dialog if an image file cannot be loaded.
   */
  static LVGLProject *load(const QString &fileName);

  /**
   * @brief Saves the current project state to a JSON file.
   * @param fileName Destination path.
   * @return true on success.
   *
   * Serializes all top-level objects (which recursively include children),
   * all images with file paths, and all custom fonts.
   */
  bool save(const QString &fileName);

  /**
   * @brief Exports the project as C source code.
   * @param path Output directory path.
   * @return true on success.
   *
   * Generates a .h file with extern declarations and a .c file with
   * a create function that reconstructs the UI. Also saves image and
   * font assets as separate .c files.
   */
  bool exportCode(const QString &path) const;

  /** @brief Returns the file path used in the last save/load operation. */
  QString fileName() const;
  /** @brief Sets the project name. */
  void setName(const QString &name);
```

- [ ] **Step 2: Commit**

```bash
git add LVGLProject.h
git commit -m "docs: add Doxygen documentation to LVGLProject.h"
```

---

### Task 9: Document Tier 2 headers (supporting classes)

**Files:**
- Modify: `LVGLPropertyModel.h`, `LVGLObjectModel.h`, `LVGLWidgetModel.h`, `LVGLStyle.h`, `LVGLStyleModel.h`, `LVGLFontData.h`, `LVGLImageData.h`, `LVGLDisplay.h`, `LVGLImageManager.h`, `LVGLFontManager.h`, `LVGLObjectManager.h`, `LVGLWidgetRegistry.h`, `LVGLWorker.h`, `LVGLDialog.h`, `LVGLFontDialog.h`, `LVGLNewDialog.h`, `LVGLTabWidget.h`, `LVGLWidgetListView.h`, `ProjectManager.h`, `RecentFilesManager.h`, `AssetManager.h`, `CanvasActions.h`, `DragDropHandler.h`, `SelectionManager.h`

- [ ] **Step 1: Add file-level and class-level docs to LVGLPropertyModel.h**

```cpp
/**
 * @file LVGLPropertyModel.h
 * @brief Qt item model and delegate for the property inspector panel.
 */

/**
 * @class LVGLPropertyModel
 * @brief QAbstractItemModel that exposes an LVGLObject's properties as a tree.
 *
 * Maps the LVGLProperty composite tree (parent/children) to Qt's model/view
 * framework. The property inspector (QTreeView) displays two columns: property
 * name and current value.
 */

/**
 * @class LVGLPropertyDelegate
 * @brief Custom delegate that creates property-specific editors in the inspector.
 *
 * Delegates editor creation to each LVGLProperty's editor() method, allowing
 * spin boxes for integers, combo boxes for enums, color pickers, etc.
 */
```

- [ ] **Step 2: Add file-level and class-level docs to LVGLObjectModel.h**

```cpp
/**
 * @file LVGLObjectModel.h
 * @brief Qt item model for the object tree panel showing the widget hierarchy.
 */

/**
 * @class LVGLObjectModel
 * @brief QAbstractItemModel that presents all LVGLObjects as a parent-child tree.
 *
 * Supports insert/remove notifications so the tree view stays synchronized
 * as widgets are added or deleted on the canvas.
 */
```

- [ ] **Step 3: Add file-level and class-level docs to LVGLWidgetModel.h**

```cpp
/**
 * @file LVGLWidgetModel.h
 * @brief Qt list model for the widget palette panel.
 */

/**
 * @class LVGLWidgetModel
 * @brief QAbstractListModel that lists available widget types for drag-and-drop creation.
 *
 * Provides mime data encoding so that dragging a widget type from the palette
 * and dropping it on the canvas triggers widget creation in LVGLSimulator.
 */
```

- [ ] **Step 4: Add file-level and class-level docs to LVGLStyle.h**

```cpp
/**
 * @file LVGLStyle.h
 * @brief LVGL style data model for the style editor panel.
 *
 * Defines the LVGLStyleItem tree structure that maps LVGL's flat style struct
 * (lv_style_t) into a hierarchical model (Body > Border > Width, Color, etc.)
 * suitable for display in a QTreeView.
 */

/**
 * @class LVGLStyleItem
 * @brief A node in the style property tree representing one editable style field.
 *
 * Each item knows its type (Color, Coord, Opacity, etc.) and its byte offset
 * into the lv_style_t struct, enabling direct read/write of style fields
 * without per-field getter/setter functions.
 */

/**
 * @class LVGLStyle
 * @brief Root of the style item tree with read/write access to a live lv_style_t.
 *
 * Wraps a pointer to an lv_style_t and provides get/set operations that
 * use the child LVGLStyleItem's offset to read/write the correct field.
 */
```

- [ ] **Step 5: Add file-level and class-level docs to remaining Tier 2 headers**

For each of these files, add a `@file` block and a `@class` block following the same pattern. Read each header first, then add appropriate docs:

`LVGLStyleModel.h`:
```cpp
/**
 * @file LVGLStyleModel.h
 * @brief Qt item model, delegate, and body-part dialog for the style editor panel.
 */
```

`LVGLFontData.h`:
```cpp
/**
 * @file LVGLFontData.h
 * @brief Font asset wrapper — loads TTF via FreeType, serializes to JSON and C code.
 */

/**
 * @class LVGLFontData
 * @brief Represents a single font asset (built-in or user-loaded).
 *
 * Wraps an lv_font_t and provides serialization for project save/load (JSON)
 * and C code export. Custom fonts are loaded from TTF files via FreeType;
 * built-in fonts reference LVGL's compiled-in font data.
 */
```

`LVGLImageData.h`:
```cpp
/**
 * @file LVGLImageData.h
 * @brief Image asset wrapper — loads images, converts to LVGL format, exports as C code.
 */

/**
 * @class LVGLImageData
 * @brief Represents a single image asset in the builder.
 *
 * Converts Qt images to LVGL's internal format (lv_img_dsc_t) and supports
 * multiple color depths. Can serialize to JSON (for project save) and to
 * C source code (for export).
 */
```

`LVGLDisplay.h`:
```cpp
/**
 * @file LVGLDisplay.h
 * @brief LVGL display driver wrapper managing the framebuffer and input driver.
 */

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
```

`LVGLImageManager.h`:
```cpp
/**
 * @file LVGLImageManager.h
 * @brief Manages the collection of image assets available to the builder.
 */

/**
 * @class LVGLImageManager
 * @brief Registry for image assets — add, remove, and look up images by name or descriptor.
 */
```

`LVGLFontManager.h`:
```cpp
/**
 * @file LVGLFontManager.h
 * @brief Manages built-in and custom fonts available to the builder.
 */

/**
 * @class LVGLFontManager
 * @brief Registry for font assets — initializes built-in fonts, loads custom TTF fonts.
 */
```

`LVGLObjectManager.h`:
```cpp
/**
 * @file LVGLObjectManager.h
 * @brief Tracks all LVGLObject instances in the current project.
 */

/**
 * @class LVGLObjectManager
 * @brief Registry for widget instances — add, remove, and query by name, type, or lv_obj_t.
 */
```

`LVGLWidgetRegistry.h`:
```cpp
/**
 * @file LVGLWidgetRegistry.h
 * @brief Registry of available widget types (LVGLWidget subclasses).
 */

/**
 * @class LVGLWidgetRegistry
 * @brief Stores and looks up widget type definitions registered at startup.
 */
```

`LVGLWorker.h`:
```cpp
/**
 * @file LVGLWorker.h
 * @brief Stub QThread subclass — declared but not implemented or used.
 */
```

`LVGLDialog.h`:
```cpp
/**
 * @file LVGLDialog.h
 * @brief Fixed-size dialog window that matches the LVGL display resolution.
 */
```

`LVGLFontDialog.h`:
```cpp
/**
 * @file LVGLFontDialog.h
 * @brief Dialog for loading custom fonts with size and BPP (bits per pixel) selection.
 */
```

`LVGLNewDialog.h`:
```cpp
/**
 * @file LVGLNewDialog.h
 * @brief New project dialog for setting project name and display resolution.
 */
```

`LVGLTabWidget.h`:
```cpp
/**
 * @file LVGLTabWidget.h
 * @brief Container for a single screen's LVGL parent object and widget instances.
 */
```

`LVGLWidgetListView.h`:
```cpp
/**
 * @file LVGLWidgetListView.h
 * @brief List view for the widget palette with drag-and-drop initiation support.
 */
```

`ProjectManager.h`:
```cpp
/**
 * @file ProjectManager.h
 * @brief Manages project lifecycle — new, open, save, save-as, export operations.
 */
```

`RecentFilesManager.h`:
```cpp
/**
 * @file RecentFilesManager.h
 * @brief Tracks and displays recently opened project files in the File menu.
 */
```

`AssetManager.h`:
```cpp
/**
 * @file AssetManager.h
 * @brief Coordinates image and font asset UI panels (add, remove, list display).
 */
```

`CanvasActions.h`:
```cpp
/**
 * @file CanvasActions.h
 * @brief Object manipulation operations — create, delete, move, and z-order for canvas widgets.
 */
```

`DragDropHandler.h`:
```cpp
/**
 * @file DragDropHandler.h
 * @brief Processes drag-and-drop events from the widget palette onto the canvas.
 */
```

`SelectionManager.h`:
```cpp
/**
 * @file SelectionManager.h
 * @brief Hit-testing and object selection logic for the canvas.
 */
```

- [ ] **Step 6: Commit**

```bash
git add LVGLPropertyModel.h LVGLObjectModel.h LVGLWidgetModel.h LVGLStyle.h LVGLStyleModel.h LVGLFontData.h LVGLImageData.h LVGLDisplay.h LVGLImageManager.h LVGLFontManager.h LVGLObjectManager.h LVGLWidgetRegistry.h LVGLWorker.h LVGLDialog.h LVGLFontDialog.h LVGLNewDialog.h LVGLTabWidget.h LVGLWidgetListView.h ProjectManager.h RecentFilesManager.h AssetManager.h CanvasActions.h DragDropHandler.h SelectionManager.h
git commit -m "docs: add Doxygen file and class documentation to Tier 2 headers"
```

---

### Task 10: Add one-liner file comments to widget subclass headers

**Files:**
- Modify: all `.h` files in `widgets/` except `LVGLWidget.h`

- [ ] **Step 1: Add @file comments to each widget header**

Read each widget header to understand what LVGL widget it wraps, then add a one-liner. The pattern for each file:

```cpp
/**
 * @file LVGLArc.h
 * @brief LVGLWidget subclass for lv_arc — circular arc indicator with configurable angles.
 */
```

Full list of one-liners to add (read each file first to verify accuracy):

| File | Comment |
|------|---------|
| `LVGLArc.h` | `lv_arc — circular arc indicator with configurable angles` |
| `LVGLBar.h` | `lv_bar — progress bar with min/max range` |
| `LVGLButton.h` | `lv_btn — push button with toggle support and layout properties` |
| `LVGLButtonMatrix.h` | `lv_btnm — grid of buttons defined by a string map` |
| `LVGLCalendar.h` | `lv_calendar — date picker with highlighted and today dates` |
| `LVGLCanvas.h` | `lv_canvas — bitmap buffer widget for off-screen rendering` |
| `LVGLChart.h` | `lv_chart — data visualization with line/bar/point series` |
| `LVGLCheckBox.h` | `lv_cb — checkbox with text label` |
| `LVGLColorPicker.h` | `lv_cpicker — color wheel/disc picker` |
| `LVGLContainer.h` | `lv_cont — layout container with auto-arrangement modes` |
| `LVGLDropDownList.h` | `lv_ddlist — drop-down selection list` |
| `LVGLGauge.h` | `lv_gauge — radial gauge with needle and scale` |
| `LVGLImage.h` | `lv_img — displays an image asset` |
| `LVGLImageButton.h` | `lv_imgbtn — button with separate images per state` |
| `LVGLImageSlider.h` | `Custom image-based slider widget` |
| `LVGLKeyboard.h` | `lv_kb — on-screen keyboard for text input` |
| `LVGLLabel.h` | `lv_label — text display with alignment and long-text modes` |
| `LVGLLED.h` | `lv_led — LED indicator with adjustable brightness` |
| `LVGLLine.h` | `lv_line — draws connected line segments from a point array` |
| `LVGLLineMeter.h` | `lv_lmeter — circular line meter (like a tachometer)` |
| `LVGLList.h` | `lv_list — scrollable list of labeled buttons` |
| `LVGLMessageBox.h` | `lv_mbox — message box with text and action buttons` |
| `LVGLPage.h` | `lv_page — scrollable container with scrollbar` |
| `LVGLPreloader.h` | `lv_preload — spinning loading indicator` |
| `LVGLSlider.h` | `lv_slider — draggable slider with min/max range and knob` |
| `LVGLSwitch.h` | `lv_sw — on/off toggle switch` |
| `LVGLTabview.h` | `lv_tabview — tabbed container with switchable pages` |
| `LVGLTextArea.h` | `lv_ta — text input area with single-line and multi-line modes` |
| `LVGLWidgets.h` | `Aggregate include header for all widget type definitions` |

- [ ] **Step 2: Commit**

```bash
cd widgets && git add *.h && cd ..
git commit -m "docs: add one-liner Doxygen file comments to all widget headers"
```

---

### Task 11: Add one-liner file comments to property subclass headers

**Files:**
- Modify: all `.h` files in `properties/`

- [ ] **Step 1: Read and categorize all property headers**

Read each property header file to understand what it does, then add a `@file` one-liner. The general pattern:

```cpp
/**
 * @file LVGLPropertyColor.h
 * @brief LVGLProperty subclass for color values — provides a color picker editor.
 */
```

For each file, read its class declaration to write an accurate one-liner. Group by the widget they serve where applicable (e.g., `LVGLPropertyChartType.h` -> "Chart type selection property — line, bar, or point").

- [ ] **Step 2: Add @file comments to all property headers**

Apply the one-liner to each of the 112 property header files. Read each file before writing to ensure accuracy.

- [ ] **Step 3: Commit**

```bash
cd properties && git add *.h && cd ..
git commit -m "docs: add one-liner Doxygen file comments to all property headers"
```

---

### Task 12: Add implementation notes to key .cpp files

**Files:**
- Modify: `LVGLObject.cpp`, `LVGLSimulator.cpp`, `LVGLProject.cpp`

- [ ] **Step 1: Add implementation notes to LVGLObject.cpp**

Add comments explaining non-obvious implementation details:

At the destructor:
```cpp
// Canvas objects allocate a separate pixel buffer that LVGL doesn't
// free automatically — we must delete it before lv_obj_del().
```

At the four constructors:
```cpp
// Constructor variants handle different creation contexts:
// 1. User drag-drop (LVGLObject parent) — most common path
// 2. Root object creation (raw lv_obj_t parent) — no LVGLObject parent
// 3. Explicit LVGL parent diverging from logical parent — currently unused
// 4. Wrapping pre-existing lv_obj_t — used for TabView page sub-objects
```

- [ ] **Step 2: Add implementation notes to LVGLProject.cpp**

At the IS_PAGE_OF_TABVIEW macro:
```cpp
/**
 * @brief Checks if an object is a page inside a TabView widget.
 *
 * TabView pages need special handling during code generation — they are
 * created via lv_tabview_add_tab() rather than the normal create pattern.
 * This macro identifies them by checking type, index, and parent type.
 */
```

At exportCode, before the widget loop:
```cpp
// Each widget is wrapped in an #if LV_USE_xxx guard so the generated
// code compiles even if some widget types are disabled in lv_conf.h.
```

- [ ] **Step 3: Add implementation notes to LVGLSimulator.cpp where mouse handling is non-obvious**

Read the mouse event handlers and add a brief comment explaining why manual mouse processing is used instead of Qt's drag framework (LVGL needs precise coordinates for hit-testing).

- [ ] **Step 4: Commit**

```bash
git add LVGLObject.cpp LVGLProject.cpp LVGLSimulator.cpp
git commit -m "docs: add implementation notes to key .cpp files"
```

---

### Task 13: Verify build still succeeds

**Files:** None (verification only)

- [ ] **Step 1: Build the project**

```bash
cmake -B build
cmake --build build
```

Expected: Clean build with no errors. Doxygen comments are regular C++ comments and cannot break compilation, but verify no typos introduced syntax errors.

- [ ] **Step 2: Run tests**

```bash
ctest --test-dir build
```

Expected: All tests pass (documentation changes should not affect behavior).
