# Doxygen In-Source Documentation Design

## Goal

Add Doxygen-format comments to the LVGLBuilder codebase so that future maintainers and new contributors can understand what each component is, how to use it, and why it's designed that way — without reading every line of implementation.

## Audience

- Future maintainer (solo developer returning after weeks/months)
- New contributors onboarding to the codebase

## Approach: Top-Down

Document core classes fully first, then add lighter coverage to the rest. This delivers the highest-value documentation first — core classes are where onboarding confusion actually happens.

## Comment Format

Doxygen `/** */` block style. No Doxyfile or HTML generation — purely in-source convention.

### File-level (every documented file)

```cpp
/**
 * @file LVGLCore.h
 * @brief Central wrapper for LVGL display drivers, framebuffer, and type registry.
 *
 * LVGLCore initializes the LVGL runtime and delegates to specialized managers
 * for display, images, fonts, objects, and widget types. Most subsystems
 * access it via the `lvgl` global variable.
 */
```

### Class-level (core classes get full description + rationale, others get one-liner)

```cpp
/**
 * @class LVGLCore
 * @brief Central manager for the LVGL runtime and asset registries.
 *
 * Exposed as a global variable (`extern LVGLCore lvgl`). Only one instance
 * should exist at a time because LVGL 6.x uses internal global state —
 * multiple instances would conflict.
 */
```

### Method-level (Tier 1 public methods only)

```cpp
/**
 * @brief Advances the LVGL tick and task handler.
 *
 * Called periodically (via timer) to let LVGL process animations,
 * input events, and pending redraws. After polling, the framebuffer
 * reflects the updated display state.
 */
void poll();
```

### Subclass one-liner

```cpp
/**
 * @file LVGLButton.h
 * @brief LVGLWidget subclass for lv_btn — supports toggle state and layout properties.
 */
```

## Coverage Tiers

### Tier 1: Full documentation (file + class + public methods + design rationale)

| File | Reason |
|------|--------|
| `LVGLCore.h/.cpp` | Singleton, owns LVGL runtime, type registry — entry point for everything |
| `LVGLObject.h/.cpp` | Widget instance wrapper, JSON serialization, C code gen — most complex class |
| `LVGLWidget.h/.cpp` | Abstract base for all widget types — must understand to add new widgets |
| `LVGLProperty.h/.cpp` | Abstract base + template machinery — trickiest pattern in the codebase |
| `LVGLSimulator.h/.cpp` | Canvas rendering, drag/resize/selection — interactive editing logic |
| `LVGLItem.h/.cpp` | Graphics item with resize handles — ties LVGLObject to Qt scene |
| `MainWindow.h/.cpp` | Coordinates all UI panels — project operations entry point |
| `LVGLProject.h/.cpp` | Save/load/export — file format and code generation |

### Tier 2: File + class level only

| File |
|------|
| `LVGLPropertyModel.h/.cpp` |
| `LVGLPropertyDelegate.h/.cpp` |
| `LVGLObjectModel.h/.cpp` |
| `LVGLWidgetModel.h/.cpp` |
| `LVGLStyle.h/.cpp` |
| `LVGLFontData.h/.cpp` |
| `LVGLImageData.h/.cpp` |

### Tier 3: One-liner file comment only (~93 files)

- All `widgets/` subclasses
- All `properties/` subclasses

## Design Rationale Policy

**Document "why" when:**
- The class exists in a non-obvious form (e.g., global instance, template-heavy)
- A method uses an unexpected approach (e.g., manual drag state tracking instead of QDrag)
- Null checks or guards protect against non-obvious edge cases

**Skip rationale for:**
- Standard Qt patterns (signals/slots, model/view)
- Straightforward getters/setters
- Self-explanatory code

Guiding question: if a new contributor reads a method and thinks "why not just do X instead?", the comment should already answer that.

## Conventions

- `@brief` is mandatory on every documentation block
- `@param` / `@return` on Tier 1 public methods only
- Design rationale goes as free-text paragraph after `@brief`, not a special tag
- No `@author` or `@date` — git blame covers that
- No `@todo` — tracked issues go in `docs/tracked/` per project convention
- Comments live in headers unless rationale is purely about implementation detail (those go in `.cpp`)
- No private method documentation
- No documentation for vendored `lvgl/` or `freetype/`

## Execution Order

1. Tier 1 headers (`.h` files) — class and method docs
2. Tier 1 implementations (`.cpp` files) — implementation notes where logic is non-obvious
3. Tier 2 headers — file + class level
4. Tier 3 one-liners — batch through `widgets/` and `properties/`

## Out of Scope

- Doxyfile or HTML/PDF generation
- Vendored dependency documentation
- Private method documentation
- Code style guide (separate effort)
