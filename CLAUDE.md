# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

LVGLBuilder is a Qt-based GUI builder for LVGL (Light and Versatile Graphics Library). It lets users visually design embedded UIs via drag-and-drop, then export them as C code for embedded systems. It embeds LVGL 6.1 and FreeType as vendored dependencies.

## Build Commands

```bash
qmake LVGLBuilder.pro
make                # macOS/Linux
mingw32-make        # Windows (MinGW)
jom                 # Windows (MSVC)
```

Requires Qt 6 with Core, Gui, and Widgets modules. No longer compatible with Qt 5.

There is no test suite.

## Architecture

### Core Classes

- **LVGLCore** — Singleton (`lvgl` global). Wraps LVGL display/input drivers, manages the framebuffer (renders to QImage), and owns all registered images, fonts, and widget type definitions.
- **LVGLObject** — Represents a widget instance on the canvas. Wraps `lv_obj_t`, manages properties/styles/children, and handles JSON serialization and C code generation.
- **LVGLWidget** (abstract base) — Defines a widget *type* (button, label, slider, etc.). Each subclass implements `newObject()` to create the underlying `lv_obj_t` and declares the type's available properties. ~35 subclasses in `widgets/`.
- **LVGLProperty** (abstract base) — Defines a single editable property. Subclasses provide get/set logic, editor widgets, and JSON serialization. ~15 types in `properties/`.
- **LVGLSimulator** — QGraphicsView that renders the LVGL framebuffer, handles interactive editing (drag, resize, selection), and processes drag-drop widget creation from the palette.
- **LVGLItem** — QGraphicsItem that wraps an LVGLObject for canvas display with resize handles.
- **MainWindow** — Coordinates the UI: widget palette, property inspector, object tree, style editor, and project operations (save/load/export).
- **LVGLProject** — Project metadata (name, resolution). Handles JSON save/load and C code export by traversing the object tree.

### Data Flow

Widget creation: User drags from palette -> LVGLSimulator -> LVGLWidget.newObject() creates lv_obj_t -> LVGLObject wraps it -> LVGLItem added to scene -> LVGLObjectModel updated.

Property editing: User selects widget -> LVGLPropertyModel populates inspector -> user edits -> LVGLProperty.set() updates lv_obj_t -> LVGLCore re-renders framebuffer.

### Key Patterns

- **Model-View**: LVGLPropertyModel/LVGLPropertyDelegate for the property inspector, LVGLObjectModel for the object tree, LVGLWidgetModel for the widget palette.
- **Type Registry**: Widget types (LVGLWidget subclasses) are registered with LVGLCore at startup and looked up by type enum.
- **Qt Resource System**: Icons and images bundled via `resources/res.qrc`.

### Source Layout

- Root `.cpp/.h` files: Core application classes (MainWindow, LVGLCore, LVGLSimulator, LVGLObject, etc.)
- `widgets/`: LVGLWidget subclasses (one per widget type)
- `properties/`: LVGLProperty subclasses (one per property type)
- `lvgl/`: Vendored LVGL 6.1 library with `lv_conf.h` configuration
- `freetype/`: Vendored FreeType library

### Adding a New Widget

1. Create a new LVGLWidget subclass in `widgets/` implementing `newObject()`, property declarations, and style definitions.
2. Register it in LVGLCore's constructor where other widgets are registered.
3. Add source files to `LVGLBuilder.pro`.

### Project File Format

Projects are saved as JSON containing widget tree, properties, image/font assets, and display resolution. C code export generates `.c`/`.h` files with `lv_obj_create()` calls and property setters.
