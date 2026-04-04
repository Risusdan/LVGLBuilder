# Architecture Audit Report Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Produce a self-contained HTML report (`docs/architecture-audit-report.html`) analyzing LVGLBuilder's architectural problems, written in Traditional Chinese, targeting a C/C++ manager unfamiliar with Qt/LVGL.

**Architecture:** Single static HTML file with inline CSS. No JavaScript libraries. All charts and diagrams rendered with pure CSS (flexbox/grid). The file is built in one task since it's a single deliverable with no logic to test.

**Tech Stack:** HTML5, CSS3 (inline `<style>` block)

---

### Task 1: Create the complete HTML report

**Files:**
- Create: `docs/architecture-audit-report.html`

- [ ] **Step 1: Create the HTML file with all content**

Write `docs/architecture-audit-report.html` with the complete content below. This is the entire report in one file.

The report must contain these sections in order:

**Section 1 — Title:** "LVGLBuilder 架構審計報告"

**Section 2 — Executive Summary:** One paragraph covering:
- Project scale: 130 source files, ~14,000 lines (excluding vendored LVGL/FreeType)
- Zero test coverage
- 7 architectural problems identified (3 high, 3 medium, 1 low severity)
- Conclusion: refactoring is necessary before adding OSD Tool features, or technical debt will compound with every addition

**Section 3 — Project Overview:**
- Key metrics table: file count (130), total lines (~14,000), test count (0), widget types (~25)
- **CSS horizontal bar chart** showing top file sizes:
  - LVGLCore.cpp: 818 lines
  - LVGLObject.cpp: 766 lines
  - MainWindow.cpp: 508 lines
  - LVGLSimulator.cpp: 443 lines
  - LVGLProperty.cpp: 419 lines
  - LVGLFontData.cpp: 342 lines
  - LVGLProject.cpp: 302 lines
- Bar chart implementation: each bar is a `<div>` with `display: flex`, the bar width set as percentage relative to the largest file (818 lines = 100%). Color: steel blue. Label on the left, line count on the right.

**Section 4 — Problem 1: 上帝類別 (God Class)** — Severity: 高

Problem description (use C/C++ concepts):
- `LVGLCore` is a single class with 818 lines and 8 distinct responsibilities: display driver management, image asset management, font management, widget type registry, object instance tracking, color conversion, input handling, and coordinate helpers.
- In C/C++ terms: imagine a single `.c` file that manages the screen buffer, a font cache, an image cache, a widget factory, and an object registry all through one global struct. Any change to one subsystem risks breaking the others.

Code evidence — show the member variables from `LVGLCore.h` lines 120-138:
```cpp
// LVGLCore.h — 8 distinct data domains in one class
QHash<QString, LVGLImageData *> m_images;      // Image management
QHash<QString, const LVGLWidget *> m_widgets;   // Widget registry
QList<LVGLObject *> m_objects;                  // Object tracking
QList<LVGLFontData *> m_fonts;                  // Font management
lv_style_t m_screenStyle;                       // Style management
std::vector<lv_color_t> m_dispFrameBuf;         // Display buffer
lv_indev_data_t m_inputData;                    // Input handling
struct FT_LibraryRec_ *m_ft;                    // FreeType library
```

Impact:
- Cannot modify image management without risk of breaking font management or display rendering
- The `init()` function (LVGLCore.cpp:34-130) must initialize ALL subsystems at once — there is no way to start just one service
- New developers must understand 818 lines before they can safely change any part

Suggested solutions — with **before/after decomposition diagram**:

Solution A: Extract each responsibility into its own class (LVGLDisplay, LVGLImageManager, LVGLFontManager, LVGLWidgetRegistry, LVGLObjectManager). LVGLCore becomes a thin facade that delegates to them. Existing code using `lvgl.addImage(...)` continues to work.

Solution B: At minimum, extract the two largest domains (image management and font management) into separate classes, reducing LVGLCore by ~40%.

**Diagram:** A before/after comparison.
- Left side ("現狀"): One large red-bordered box labeled "LVGLCore (818 行)" containing 8 smaller items listed vertically: 顯示驅動, 圖片管理, 字型管理, Widget 註冊, 物件追蹤, 顏色轉換, 輸入處理, 座標工具
- Right side ("重構後"): 5 separate blue-bordered boxes (LVGLDisplay, LVGLImageManager, LVGLFontManager, LVGLWidgetRegistry, LVGLObjectManager) plus one thin green-bordered box labeled "LVGLCore (facade)" with arrows pointing to each service box.
- Implementation: CSS grid layout, 2 columns. Left column has one tall box. Right column has 6 smaller boxes. Use `border`, `border-radius`, `padding`. Arrow: use `→` character or CSS `::after` pseudo-element.

**Section 5 — Problem 2: 全域單例耦合 (Global Singleton Coupling)** — Severity: 高

Problem description:
- The entire application depends on a single global variable `extern LVGLCore lvgl;` (LVGLCore.h:142). This is equivalent to a massive `extern struct` in C — every file that includes the header can read and write the global state.
- 24 source files directly call `lvgl.someMethod()`, creating a spider-web of hidden dependencies.

Code evidence — show the global declaration and example usage:
```cpp
// LVGLCore.h:142 — global variable declaration
extern LVGLCore lvgl;

// LVGLProject.cpp:98 — project save depends on global state
for (LVGLObject *o : lvgl.allObjects()) {
    if (o->parent() == nullptr)
        widgetArr.append(o->toJson());
}

// LVGLSimulator.cpp:324 — UI rendering depends on global state
for (LVGLObject *o : lvgl.allObjects()) {
    if (geo.contains(pos)) ret << o;
}
```

Impact:
- Cannot test `LVGLProject::save()` without initializing the entire LVGL display subsystem, FreeType, and all widget types
- A bug in any of the 24 files can corrupt global state, causing failures in unrelated files
- Impossible to run two independent instances (e.g., for multi-document editing)

**Dependency spider-web diagram:**
- Center: a prominent box labeled "`lvgl` 全域變數"
- Surrounding: 24 boxes for each dependent file, arranged in a circle/grid
- Group them by category with different colors:
  - Core (red): LVGLCore.cpp, LVGLObject.cpp, LVGLSimulator.cpp, MainWindow.cpp, LVGLProject.cpp
  - Models (orange): LVGLObjectModel.cpp, LVGLWidgetModel.cpp, LVGLStyleModel.cpp, LVGLPropertyModel (indirect)
  - Properties (yellow): LVGLPropertyColor.cpp, LVGLPropertyImage.cpp, LVGLPropertyList.cpp, LVGLPropertyPoints.cpp, LVGLPropertySeries.cpp, LVGLPropertyAssignTextArea.cpp
  - Data (green): LVGLImageData.cpp, LVGLFontData.cpp, LVGLStyle.cpp, LVGLProperty.cpp, LVGLItem.cpp, LVGLDialog.cpp
  - Widgets (blue): LVGLImage.cpp, LVGLImageSlider.cpp, LVGLImageButton.cpp, LVGLTabview.cpp
- Implementation: CSS grid, center box larger with bold border. Surrounding boxes smaller. Lines can be implied by layout proximity (no actual SVG lines needed — the visual clustering around the center makes the point).

Suggested solutions:
- Solution A: Dependency injection — components receive the services they need as constructor parameters instead of accessing the global. Example: `ProjectManager(LVGLImageManager*, LVGLFontManager*, LVGLObjectManager*)`.
- Solution B: At minimum, pass LVGLCore as an explicit parameter to functions that use it, making the dependency visible in the function signature rather than hidden.

**Section 6 — Problem 3: 零測試覆蓋、無法測試 (Zero Tests, Untestable Architecture)** — Severity: 高

Problem description:
- The project has zero automated tests. Every change must be verified manually through the GUI.
- More importantly, the architecture makes it impossible to add unit tests: every core class depends on the global `lvgl` singleton, which requires LVGL display initialization, FreeType initialization, and Qt timer setup.

Code evidence — show the init function dependency chain:
```cpp
// LVGLCore.cpp:34-63 — to test ANY core functionality,
// ALL of this must execute first:
void LVGLCore::init(int width, int height) {
    lv_init();                          // 1. Initialize LVGL library
    m_dispFrameBuf.resize(n);           // 2. Allocate framebuffers
    lv_disp_drv_register(&m_dispDrv);   // 3. Register display driver
    lv_indev_drv_register(&indev_drv);  // 4. Register input driver
    m_default = lvgl.addImage(...);     // 5. Load default image
    m_fonts << new LVGLFontData(...);   // 6. Load all fonts
    addWidget(new LVGLArc);            // 7. Register ~25 widget types
    addWidget(new LVGLBar);
    // ... 25 more registrations
}
```

Impact:
- A C++ project with 14,000 lines and zero tests means every bug fix risks introducing regressions
- Manual testing takes minutes per change; automated tests would take seconds
- Refactoring is extremely risky without a safety net of tests
- No way to verify correctness of JSON serialization, property logic, or code generation automatically

Suggested solutions:
- Solution A: First decompose LVGLCore (Problem 1), then write unit tests for each extracted service in isolation. For example, test `LVGLImageManager::addImage()` without initializing the display.
- Solution B: As an interim step, add integration tests that initialize the full system, testing JSON round-trip serialization and property get/set correctness. Less ideal but possible without refactoring.

**Section 7 — Problem 4: UI 與業務邏輯混雜 (UI Mixed with Business Logic)** — Severity: 中

Problem description:
- `LVGLSimulator::mousePressEvent()` (443 lines total file) contains widget selection logic, right-click menu construction, object deletion, object reordering, and color dialog management — all in one event handler.
- In C terms: imagine a single `onMouseClick()` callback that handles hit-testing, menu display, object deletion, and configuration changes. Any UI change (e.g., changing the menu) risks breaking the object manipulation logic.

Code evidence from `LVGLSimulator.cpp:139-194`:
```cpp
void LVGLSimulator::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        auto obj = selectObject(...);        // Hit testing
        QMenu menu(obj->name(), this);       // UI: build menu
        QAction *remove = menu.addAction("Remove");
        sel = menu.exec(...);                // UI: show menu
        if (sel == remove) {
            lvgl.removeObject(obj);          // Business logic: delete
        } else if (sel == mfore) {
            lv_obj_move_foreground(obj->obj()); // Business logic: reorder
        }
    }
}
```

Similarly, `MainWindow::loadProject()` (MainWindow.cpp:263-280) mixes file I/O, error dialogs, window title updates, resolution changes, and UI state management in one method.

Impact:
- Cannot test object selection or deletion logic without a running Qt GUI
- Changing the right-click menu requires understanding object manipulation code
- Adding new operations (e.g., copy/paste via menu) means modifying the same already-complex function

Suggested solutions:
- Solution A: Extract stateless operation classes: `CanvasActions` (create/delete/move/reorder objects), `SelectionManager` (track selected/hovered object). Event handlers become thin dispatchers.
- Solution B: At minimum, extract the business logic from each event handler into separate private methods, so each method has one responsibility.

**Section 8 — Problem 5: 手動記憶體管理風險 (Manual Memory Management Risk)** — Severity: 中

Problem description:
- The codebase uses raw `new`/`delete` throughout, with multi-level pointer chains and no RAII (Resource Acquisition Is Initialization) or smart pointers.
- In `LVGLFontData::parse()`, 5 separate heap allocations occur in sequence. If any allocation or FreeType call fails partway through, previously allocated buffers leak.

Code evidence from `LVGLFontData.cpp:56-155`:
```cpp
// 5 sequential raw allocations with no cleanup on failure:
lv_font_fmt_txt_glyph_dsc_t *glyph_dsc =
    new lv_font_fmt_txt_glyph_dsc_t[glyphs + 1];  // Alloc 1
uint8_t *gylph_bitmap_buf =
    new uint8_t[gylph_bitmap.size()];               // Alloc 2
lv_font_fmt_txt_cmap_t *cmap =
    new lv_font_fmt_txt_cmap_t[1];                   // Alloc 3
lv_font_fmt_txt_dsc_t *font_dsc =
    new lv_font_fmt_txt_dsc_t;                       // Alloc 4
lv_font_t *font = new lv_font_t;                    // Alloc 5

// Destructor must manually match all allocations:
~LVGLFontData() {
    delete [] fdsc->glyph_bitmap;   // Must use delete[]
    delete [] fdsc->glyph_dsc;      // Must use delete[]
    delete [] fdsc->cmaps;          // Must use delete[]
    delete fdsc;                    // Must use delete (not delete[])
    delete m_font;                  // Must use delete (not delete[])
}
```

Impact:
- If `FT_Load_Char()` fails after `glyph_dsc` is allocated but before `font` is allocated, the glyph buffer leaks
- Mixing `delete` and `delete[]` incorrectly is undefined behavior — a common source of subtle bugs
- Every new developer must trace the full allocation chain to understand ownership

Suggested solutions:
- Solution A: Replace raw allocations with `std::unique_ptr` and custom deleters. Group related allocations into RAII wrapper structs so cleanup is automatic.
- Solution B: At minimum, add early-return cleanup using `goto cleanup` pattern (common in C) or restructure to ensure all allocations are freed on every exit path.

**Section 9 — Problem 6: Widget 定義的重複程式碼 (Duplicated Widget Boilerplate)** — Severity: 中

Problem description:
- Each of the 20+ widget files in `widgets/` defines inline property classes with nearly identical structure. The pattern is: subclass `LVGLPropertyEnum`, define `name()`, `function()`, `get()`, `set()` with only the LVGL API names differing.
- In C terms: imagine 20 source files each containing a `get_state()`/`set_state()` function pair where only the function name being called changes, but the surrounding boilerplate is copy-pasted identically.

Code evidence — compare `LVGLButton.cpp:7-30` with `LVGLLabel.cpp:7-28`:
```cpp
// widgets/LVGLButton.cpp — property class pattern
class LVGLPropertyButtonState : public LVGLPropertyEnum {
    QString name() const { return "State"; }
    QStringList function(LVGLObject *obj) const {
        return QStringList() << QString("lv_btn_set_state(%1, %2);")
            .arg(obj->codeName()).arg(m_values.at(get(obj)));
    }
    int get(LVGLObject *obj) const { return lv_btn_get_state(obj->obj()); }
    void set(LVGLObject *obj, int index) { lv_btn_set_state(obj->obj(), index & 0xff); }
};

// widgets/LVGLLabel.cpp — same pattern, different API names
class LVGLPropertyLabelAlign : public LVGLPropertyEnum {
    QString name() const { return "Align"; }
    QStringList function(LVGLObject *obj) const {
        return QStringList() << QString("lv_label_set_align(%1, %2);")
            .arg(obj->codeName()).arg(m_values.at(get(obj)));
    }
    int get(LVGLObject *obj) const { return lv_label_get_align(obj->obj()); }
    void set(LVGLObject *obj, int index) { lv_label_set_align(obj->obj(), index & 0xff); }
};
```

Impact:
- Adding a new widget type requires copying ~80 lines of boilerplate from an existing widget
- A bug in the property pattern (e.g., incorrect `function()` format) must be fixed in 20+ files
- Property classes defined inside widget `.cpp` files cannot be reused by other widgets

Suggested solutions:
- Solution A: Create a generic property template or macro that takes the LVGL getter/setter function pointers as parameters, eliminating the need for a separate class per property. Example: `LVGLEnumProperty<lv_btn_get_state, lv_btn_set_state>("State", values)`.
- Solution B: Move all property class definitions from `widgets/*.cpp` into separate files under `properties/`, making them discoverable and reusable.

**Section 10 — Problem 7: 循環依賴與職責不清 (Circular Dependencies)** — Severity: 低

Problem description:
- `LVGLObject` includes `LVGLCore.h` and directly calls LVGL C functions (like `lv_obj_del()`), while `LVGLCore` stores and manages `LVGLObject*` instances. This creates a bidirectional dependency.
- In C terms: module A includes module B's header, and module B includes module A's header. Neither can be compiled or tested independently.

Code evidence:
```cpp
// LVGLObject.cpp:3 — depends on LVGLCore
#include "LVGLCore.h"

// LVGLObject.cpp:43-50 — directly calls LVGL C library
LVGLObject::~LVGLObject() {
    lv_obj_del(m_obj);  // Bypasses LVGLCore, calls LVGL directly
}

// LVGLCore.h:125 — stores LVGLObject pointers
QList<LVGLObject *> m_objects;

// LVGLCore.cpp manages object lifetime
void LVGLCore::removeObject(LVGLObject *object) {
    // But LVGLObject's destructor also manages LVGL lifetime
}
```

Impact:
- Unclear who owns the `lv_obj_t` lifecycle — does LVGLCore delete it, or does LVGLObject?
- Cannot compile LVGLObject in isolation for testing
- Refactoring either class requires understanding both

Suggested solutions:
- Solution A: Establish a clear ownership rule: LVGLCore (or its extracted ObjectManager) owns and deletes objects. LVGLObject never calls LVGL lifecycle functions directly.
- Solution B: Break the header cycle using forward declarations — LVGLObject only needs a forward declaration of LVGLCore, not the full header.

**Section 11 — Refactoring Benefits Summary**

A summary table mapping each problem to its expected improvement:

| Problem | Current State | After Refactoring |
|---|---|---|
| God Class | 1 class, 818 lines, 8 responsibilities | 5-6 focused classes, each <200 lines |
| Global Singleton | 24 files coupled to global variable | Dependencies explicit in constructor parameters |
| Zero Tests | 0 tests, untestable architecture | Each service testable independently |
| Mixed Concerns | Business logic buried in UI event handlers | Stateless operations testable without GUI |
| Memory Management | Raw new/delete chains, leak risk | RAII/smart pointers, automatic cleanup |
| Widget Boilerplate | Copy-paste across 20+ files | Shared templates, one place to change |
| Circular Dependencies | Bidirectional coupling | Clear ownership, unidirectional dependencies |

Additional benefits:
- **New developer onboarding:** Understanding one 200-line service vs. one 818-line god class
- **Feature development speed:** Changes localized to single module instead of rippling across 24 files
- **Bug isolation:** A bug in font management cannot corrupt image management
- **OSD Tool integration:** Clean interfaces make it possible to port features from colleague forks

**Section 12 — Risks of Not Refactoring**

- Every new OSD Tool feature added to the current architecture increases coupling further
- The god class grows toward 1,000+ lines, making each change progressively riskier
- Without tests, manual verification time grows linearly with feature count
- Memory management issues compound as more widget types and font formats are added
- Team scaling becomes impractical — new developers cannot contribute safely without understanding the entire 14,000-line codebase

**CSS Design Requirements:**

```css
/* Color scheme */
--color-high: #e74c3c;      /* Red for high severity */
--color-medium: #f39c12;    /* Yellow/amber for medium */
--color-low: #27ae60;       /* Green for low */
--color-bar: #4682b4;       /* Steel blue for bar chart */
--color-bg: #ffffff;        /* White background */
--color-code-bg: #f5f5f5;   /* Light gray for code blocks */

/* Typography */
body: sans-serif (system font stack), 16px base
code/pre: monospace, 14px
h1: 28px, h2: 22px, h3: 18px

/* Layout */
max-width: 900px, centered
print-friendly (no fixed positioning, clean page breaks)

/* Severity badges */
Inline colored labels with rounded corners, white text on colored background

/* Code blocks */
Light gray background, 1px border, rounded corners, padding 16px
File path + line number header in smaller italic text above the block

/* Bar chart */
Horizontal bars using flexbox, label left, count right, bar fills proportionally

/* Diagrams */
CSS grid/flexbox, colored borders, padding, inline labels
```

- [ ] **Step 2: Verify the report renders correctly**

Run: `open docs/architecture-audit-report.html`

Verify:
- All 7 problem sections render with correct severity badges
- Bar chart shows proportional bars
- Before/after decomposition diagram (Problem 1) is clear
- Dependency spider-web diagram (Problem 2) shows the 24 dependent files
- Code blocks are readable with syntax-like formatting
- Print preview looks clean (Cmd+P in browser)

- [ ] **Step 3: Commit**

```bash
git add docs/architecture-audit-report.html
git commit -m "docs: add architecture audit report for management review

Chinese-language HTML report analyzing 7 architectural problems
in LVGLBuilder with code evidence, impact analysis, and
suggested solutions to justify the refactoring effort."
```
