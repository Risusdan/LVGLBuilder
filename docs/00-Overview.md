# LVGLBuilder Developer Guide

This guide is for new team members getting started with the LVGLBuilder
codebase. For the original project introduction, see the root
[README.md](../README.md).

## What Is LVGLBuilder?

A Qt-based GUI builder for [LVGL](https://lvgl.io/) (Light and Versatile
Graphics Library). Users visually design embedded UIs via drag-and-drop,
then export them as C code targeting embedded systems. It embeds LVGL 6.1
and FreeType as vendored dependencies.

## What Changed Since the Original Codebase

The original codebase (up to commit `a283301f`) was built with Qt 5 and
qmake. Since then, the following changes have been made:

| Area | Before | After |
|------|--------|-------|
| Build system | qmake (`.pro` file) | CMake 3.16+ |
| Qt version | Qt 5 (5.11+; tested with 5.12.8, 5.14.2, 5.15.1, 5.15.2) | Qt 6 (tested with 6.9.2; Core, Gui, Widgets, Test modules) |

**Architecture refactoring:**

- `LVGLCore` decomposed into 5 focused managers: LVGLDisplay,
  LVGLImageManager, LVGLFontManager, LVGLObjectManager, LVGLWidgetRegistry
- `MainWindow` decomposed into ProjectManager, RecentFilesManager,
  AssetManager
- `LVGLSimulator` decomposed into SelectionManager, CanvasActions,
  DragDropHandler
- Widget-specific property classes moved from `widgets/` to `properties/`

**Bug fixes (~25):** serialization data corruption, enum casts in widget
styles, null safety, resize handle edge cases, canvas rendering distortion.

**Testing:** QTest infrastructure added with 11 test suites covering core
classes, serialization, code export, widgets, and managers.

**Documentation:** Doxygen-style comments added to all 31 header files.

**Feature:** JSON schema versioning for project files.

## Quick Start

```bash
cmake -B build
cmake --build build
./build/LVGLBuilder
```

## Documentation Index

- [01-How-to-Build.md](01-How-to-Build.md) -- Prerequisites, build steps, IDE setup, troubleshooting
- [02-Architecture.md](02-Architecture.md) -- Core classes, data flow, key patterns
- [03-How-to-Write-Tests.md](03-How-to-Write-Tests.md) -- QTest tutorial, adding tests, running tests
- [04-How-to-Add-a-Widget.md](04-How-to-Add-a-Widget.md) -- Step-by-step widget creation tutorial
- [05-Source-Layout.md](05-Source-Layout.md) -- Directory structure and file guide
