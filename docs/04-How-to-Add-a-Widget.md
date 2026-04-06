# How to Add a Widget

This tutorial walks through the structure of a widget using **LVGLLED**
(the LED indicator) as an example. It is the simplest widget in the
codebase — 29 lines of header and 70 lines of implementation. Open
`widgets/LVGLLED.h` and `widgets/LVGLLED.cpp` alongside this guide.

## Overview

Adding a widget requires four things:

1. A **LVGLWidget subclass** in `widgets/` (the widget type definition)
2. A **property class** in `properties/` (if the widget has custom properties)
3. **Registration** in `LVGLCore`'s constructor
4. **Build system entries** in `CMakeLists.txt` and `tests/CMakeLists.txt`

## Step 1: Create the Header

Create `widgets/LVGLMyWidget.h`:

```cpp
#ifndef LVGLMYWIDGET_H
#define LVGLMYWIDGET_H

#include "LVGLWidget.h"

class LVGLMyWidget : public LVGLWidget
{
public:
    LVGLMyWidget();

    virtual QString name() const;
    virtual QString className() const;
    virtual Type type() const;
    virtual QIcon icon() const;
    virtual lv_obj_t *newObject(lv_obj_t *parent) const;
    virtual QSize minimumSize() const;
    virtual QStringList styles() const;
    virtual lv_style_t *style(lv_obj_t *obj, int type) const;
    virtual void setStyle(lv_obj_t *obj, int type, lv_style_t *style) const;
    virtual lv_style_t *defaultStyle(int type) const;
};

#endif // LVGLMYWIDGET_H
```

Every widget overrides the same set of virtual methods. See `LVGLLED.h`
for the real version — it's identical in structure.

## Step 2: Implement the Source

Create `widgets/LVGLMyWidget.cpp`. Here's what each method does, using
LVGLLED as the reference:

### Constructor — declare properties and styles

```cpp
LVGLLED::LVGLLED()
{
    m_properties << new LVGLPropertyLEDBrightness;  // widget-specific property
    m_editableStyles << LVGL::Body;                  // LV_LED_STYLE_MAIN
}
```

- `m_properties`: list of `LVGLProperty*` — each becomes an editable row
  in the property inspector.
- `m_editableStyles`: which style parts the user can edit. Values come from
  the `LVGL::StylePart` enum and map to the widget's LVGL style types.

### `name()` and `className()`

```cpp
QString LVGLLED::name() const { return "LED"; }           // display name in palette
QString LVGLLED::className() const { return "lv_led"; }   // LVGL C class name
```

`className()` is used to look up the widget type in the registry. It must
match the LVGL naming convention (`lv_xxx`).

### `type()`

```cpp
LVGLWidget::Type LVGLLED::type() const { return LED; }
```

Returns an enum value from `LVGLWidget::Type`. When adding a new widget,
add a new enum entry to `LVGLWidget::Type` in `widgets/LVGLWidget.h`.

### `newObject()` — creates the LVGL widget

```cpp
lv_obj_t *LVGLLED::newObject(lv_obj_t *parent) const
{
    lv_obj_t *obj = lv_led_create(parent, nullptr);
    return obj;
}
```

This is the key method. It calls the LVGL C creation function for your
widget type. The `parent` parameter is the LVGL parent object. The second
argument (`nullptr`) means "no copy source."

### `minimumSize()`

```cpp
QSize LVGLLED::minimumSize() const
{
    return QSize(LV_LED_WIDTH_DEF, LV_LED_HEIGHT_DEF);
}
```

Returns the minimum drag size. Use LVGL's default macros if available, or
pick a reasonable size.

### Style methods

```cpp
QStringList LVGLLED::styles() const
{
    return QStringList() << "LV_LED_STYLE_MAIN";
}

lv_style_t *LVGLLED::style(lv_obj_t *obj, int type) const
{
    return const_cast<lv_style_t*>(lv_led_get_style(obj, type & 0xff));
}

void LVGLLED::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const
{
    lv_led_set_style(obj, static_cast<lv_led_style_t>(type), style);
}

lv_style_t *LVGLLED::defaultStyle(int type) const
{
    if (type == LV_LED_STYLE_MAIN)
        return &lv_style_pretty_color;
    return nullptr;
}
```

- `styles()`: returns the names of style parts (shown in the style editor).
- `style()`/`setStyle()`: get/set style using LVGL's C API. The `type`
  parameter is the style index. Cast it to the widget's style enum.
- `defaultStyle()`: returns the LVGL default style for comparison.

Widgets with multiple style parts (like TabView with MAIN, BTN_BG,
BTN_REL, etc.) return multiple entries and handle each case.

## Step 3: Create a Property Class (if needed)

If your widget has custom properties beyond styles, create a property class
in `properties/`. LVGLLED uses a header-only property:

```cpp
// properties/LVGLPropertyLEDBrightness.h
class LVGLPropertyLEDBrightness : public LVGLPropertyInt
{
public:
    LVGLPropertyLEDBrightness() : LVGLPropertyInt(0, 255) {}

    QString name() const { return "Brightness"; }

    QStringList function(LVGLObject *obj) const {
        return QStringList() << QString("lv_led_set_bright(%1, %2);")
                                    .arg(obj->codeName()).arg(get(obj));
    }

protected:
    int get(LVGLObject *obj) const { return lv_led_get_bright(obj->obj()); }
    void set(LVGLObject *obj, int value) {
        lv_led_set_bright(obj->obj(), static_cast<uint8_t>(value));
    }
};
```

Key points:
- Subclass the appropriate `LVGLProperty` base (`LVGLPropertyInt`,
  `LVGLPropertyBool`, `LVGLPropertyString`, etc.)
- `function()` returns the C code line for export
- `get()`/`set()` call the LVGL C API

## Step 4: Register the Widget

In `LVGLCore.cpp`, add to the constructor's registration block:

```cpp
#include "widgets/LVGLMyWidget.h"  // add include at top

// In LVGLCore::LVGLCore(), alongside other registerWidget calls:
registerWidget(new LVGLMyWidget);
```

Also add `LVGLMyWidget` to the `Type` enum in `widgets/LVGLWidget.h`.

## Step 5: Update the Build System

Add your source files to **both** CMakeLists:

In the root `CMakeLists.txt`, add to the widget sources list:

```cmake
widgets/LVGLMyWidget.cpp
```

In `tests/CMakeLists.txt`, add to the `lvglbuilder_lib` sources list:

```cmake
${CMAKE_SOURCE_DIR}/widgets/LVGLMyWidget.cpp
```

## Step 6: Build and Verify

```bash
cmake --build build
./build/LVGLBuilder
```

Your widget should now appear in the palette. Verify:
- Drag it onto the canvas
- Edit its properties in the inspector
- Save the project as JSON, reload it, confirm the widget is preserved
- Export as C code and check the generated output
