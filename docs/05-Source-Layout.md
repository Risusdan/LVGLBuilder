# Source Layout

## Directories

```
LVGLBuilder/
├── widgets/        Widget type definitions (LVGLWidget subclasses)
├── properties/     Property type definitions (LVGLProperty subclasses)
├── tests/          QTest-based unit tests
├── lvgl/           Vendored LVGL 6.1 (with lv_conf.h) — avoid modifying
├── freetype/       Vendored FreeType library — avoid modifying
├── resources/      Icons and images bundled via res.qrc
└── docs/           Developer documentation
```

### `widgets/`

One file pair per widget type. Naming convention: `LVGLWidgetName.h/.cpp`.

Each file is an LVGLWidget subclass that defines how a widget is created
(`newObject()`), what properties it has, and which styles it supports.
There are ~28 widget types.

### `properties/`

One file per property type. Naming convention:
`LVGLPropertyName.h` (many are header-only).

Each file is an LVGLProperty subclass that defines get/set logic, an
editor widget, JSON serialization, and C code generation for one property.

### `tests/`

One subdirectory per test module. Naming convention:
`tst_name/tst_name.cpp`.

See [03-How-to-Write-Tests.md](03-How-to-Write-Tests.md) for details.

### `lvgl/` and `freetype/`

Vendored third-party libraries. These are checked into the repo and built
as part of the CMake project. Avoid modifying them unless you need to patch
LVGL behavior for the builder.

`lvgl/lv_conf.h` is the LVGL configuration file — it controls which LVGL
features are enabled.

### `resources/`

Qt resources bundled via `res.qrc`. Contains widget preview icons and the
default image. Loaded at runtime with `Q_INIT_RESOURCE(res)`.

## Root-Level Files

The root directory contains the core application classes. Grouped by role:

### Core

| File          | Role                                                            |
| ------------- | --------------------------------------------------------------- |
| `LVGLCore`    | Central LVGL runtime wrapper, delegates to 5 managers           |
| `LVGLDisplay` | Framebuffer rendering, display/input driver setup               |
| `LVGLObject`  | Wrapper around `lv_obj_t` with Qt integration and serialization |
| `LVGLItem`    | QGraphicsItem for canvas display with resize handles            |

### Managers

| File                 | Role                                                 |
| -------------------- | ---------------------------------------------------- |
| `LVGLImageManager`   | Image asset collection (add, remove, lookup)         |
| `LVGLFontManager`    | Built-in and custom font management                  |
| `LVGLObjectManager`  | Tracks all LVGLObject instances in the project       |
| `LVGLWidgetRegistry` | Registry of available widget types                   |
| `ProjectManager`     | Project lifecycle — new, open, save, save-as, export |
| `RecentFilesManager` | Recently opened files in the File menu               |
| `AssetManager`       | Image and font asset UI panels                       |

### UI

| File                 | Role                                                      |
| -------------------- | --------------------------------------------------------- |
| `MainWindow`         | Main application window coordinating all panels           |
| `LVGLSimulator`      | Canvas view for LVGL rendering and interactive editing    |
| `LVGLTabWidget`      | Container for a screen's LVGL parent and widget instances |
| `LVGLWidgetListView` | Widget palette list with drag-and-drop support            |

### Models

| File                | Role                                                  |
| ------------------- | ----------------------------------------------------- |
| `LVGLPropertyModel` | Item model and delegate for the property inspector    |
| `LVGLObjectModel`   | Item model for the object tree (widget hierarchy)     |
| `LVGLStyleModel`    | Item model, delegate, and dialog for the style editor |
| `LVGLWidgetModel`   | List model for the widget palette                     |

### Project

| File             | Role                                            |
| ---------------- | ----------------------------------------------- |
| `LVGLProject`    | Project metadata, JSON save/load, C code export |
| `ProjectManager` | Project lifecycle operations                    |

### Data

| File            | Role                                                           |
| --------------- | -------------------------------------------------------------- |
| `LVGLImageData` | Image asset — loads images, converts to LVGL format, exports C |
| `LVGLFontData`  | Font asset — loads TTF via FreeType, serializes to JSON and C  |
| `LVGLStyle`     | LVGL style data model for the style editor                     |
| `LVGLProperty`  | Abstract property system for widget configuration              |

### Dialogs

| File             | Role                                                   |
| ---------------- | ------------------------------------------------------ |
| `LVGLDialog`     | Fixed-size dialog matching the LVGL display resolution |
| `LVGLNewDialog`  | New project dialog (name + display resolution)         |
| `LVGLFontDialog` | Custom font loading dialog (size + BPP selection)      |

### Other

| File               | Role                                                |
| ------------------ | --------------------------------------------------- |
| `CanvasActions`    | Object manipulation — create, delete, move, z-order |
| `SelectionManager` | Hit-testing and object selection on the canvas      |
| `DragDropHandler`  | Drag-and-drop from widget palette to canvas         |
| `LVGLWorker`       | Stub QThread subclass (unused)                      |
| `main.cpp`         | Application entry point                             |
