# Architecture

## Core Classes

| Class | Role |
|-------|------|
| **LVGLCore** | Singleton (`lvgl` global). Initializes the LVGL library, delegates to 5 specialized managers. Only one instance — LVGL 6.x uses internal global state. |
| **LVGLObject** | Wraps one `lv_obj_t` (LVGL C widget handle). Manages properties, children, JSON serialization, and C code generation. |
| **LVGLWidget** | Abstract base defining a widget *type* (button, label, slider, etc.). Each subclass implements `newObject()` and declares available properties. ~28 subclasses in `widgets/`. |
| **LVGLProperty** | Abstract base for editable properties. Subclasses provide get/set logic, editor widgets, and JSON serialization. ~15 types in `properties/`. |
| **LVGLSimulator** | QGraphicsView that renders the LVGL framebuffer and handles interactive editing (drag, resize, selection). |
| **LVGLItem** | QGraphicsItem wrapping an LVGLObject for canvas display with resize handles. |
| **MainWindow** | Coordinates the UI: widget palette, property inspector, object tree, style editor, and project operations. |
| **LVGLProject** | Project metadata (name, resolution). JSON save/load and C code export. |

## LVGLCore Managers

LVGLCore delegates to focused managers rather than implementing everything
directly. Each manager owns one concern:

| Manager | Responsibility |
|---------|---------------|
| **LVGLDisplay** | Framebuffer rendering, display/input driver setup |
| **LVGLImageManager** | Image asset collection (add, remove, lookup) |
| **LVGLFontManager** | Built-in and custom font management |
| **LVGLObjectManager** | Tracks all LVGLObject instances in the project |
| **LVGLWidgetRegistry** | Registry of available widget types |

## Data Flow

### Widget Creation

```
User drags from palette
    -> LVGLSimulator receives drop event
    -> LVGLWidget.newObject() creates lv_obj_t
    -> LVGLObject wraps the lv_obj_t
    -> LVGLItem added to QGraphicsScene
    -> LVGLObjectModel updated (object tree reflects new widget)
```

### Property Editing

```
User selects widget on canvas
    -> LVGLPropertyModel populates the property inspector
    -> User edits a value
    -> LVGLProperty.set() updates the lv_obj_t
    -> LVGLCore re-renders the framebuffer
    -> LVGLSimulator repaints the canvas
```

## Key Patterns

**Model-View:** The UI uses Qt's model/view framework throughout:
- `LVGLPropertyModel` + `LVGLPropertyDelegate` for the property inspector
- `LVGLObjectModel` for the object tree
- `LVGLWidgetModel` for the widget palette

**Type Registry:** Widget types (LVGLWidget subclasses) are registered with
LVGLCore at startup via `registerWidget()`. They are looked up by class
name (e.g., `"lv_btn"`) through the LVGLWidgetRegistry.

**Qt Resource System:** Icons and images are bundled via `resources/res.qrc`
and loaded with `Q_INIT_RESOURCE(res)`.

## Project File Format

Projects are saved as JSON containing:
- Widget tree (parent-child hierarchy with properties)
- Image and font asset references
- Display resolution
- Schema version (for forward compatibility)

C code export traverses the object tree and generates `.c`/`.h` files with
`lv_obj_create()` calls and property setters.
