# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

LVGLBuilder is a Qt-based GUI builder for LVGL (Light and Versatile Graphics Library). It lets users visually design embedded UIs via drag-and-drop, then export them as C code for embedded systems. It embeds LVGL 6.1 and FreeType as vendored dependencies.

## Build Commands

```bash
cmake -B build
cmake --build build
ctest --test-dir build    # run tests
```

Requires Qt 6 with Core, Gui, Widgets, and Test modules. No longer compatible with Qt 5.

Tests use QTest, run with `ctest --test-dir build`.

## Architecture

See [docs/02-Architecture.md](docs/02-Architecture.md) for core classes, data flow, and key patterns.

See [docs/05-Source-Layout.md](docs/05-Source-Layout.md) for directory structure and file descriptions.

See [docs/04-How-to-Add-a-Widget.md](docs/04-How-to-Add-a-Widget.md) for a step-by-step widget creation tutorial.
