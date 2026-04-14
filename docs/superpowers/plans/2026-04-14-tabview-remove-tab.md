# TabView Remove Tab Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add `lv_tabview_remove_tab()` to vendored LVGL 6.1 and wire it into LVGLBuilder's property editor so users can delete specific tabs via the existing dialog delete button, using by-name matching to preserve the correct tabs.

**Architecture:** Two layers — a C function in the vendored LVGL that handles low-level tab removal (page deletion, name array compaction, button matrix rebuild), and a C++/Qt layer that adds double-delete prevention to LVGLObject and rewrites the tab property setter with a three-phase by-name matching algorithm (remove → rename → add). TDD with QTest in a separate test project.

**Tech Stack:** C (LVGL 6.1 internals), C++/Qt 6 (builder property classes), QTest

---

## File Map

| Action | File | Responsibility |
|--------|------|----------------|
| Create | `tests/tests.pro` | Subdirs project for test suite |
| Create | `tests/tst_tabview_remove/tst_tabview_remove.pro` | Test project for tabview removal |
| Create | `tests/tst_tabview_remove/tst_tabview_remove.cpp` | QTest test cases (all 3 rounds) |
| Modify | `lvgl/lvgl/src/lv_objx/lv_tabview.h:101-112` | Declare `lv_tabview_remove_tab()` |
| Modify | `lvgl/lvgl/src/lv_objx/lv_tabview.c:335` | Implement `lv_tabview_remove_tab()` |
| Modify | `LVGLObject.h:79-81` | Add `detachLvObj()`, `detachLvObjRecursive()` |
| Modify | `LVGLObject.cpp:42-45` | Guard destructor, implement `detachLvObjRecursive()` |
| Modify | `widgets/LVGLTabview.cpp:49-76` | Rewrite `LVGLPropertyTabs::set()` with three-phase algorithm |

---

### Task 1: Create the test project scaffold

**Files:**
- Create: `tests/tests.pro`
- Create: `tests/tst_tabview_remove/tst_tabview_remove.pro`
- Create: `tests/tst_tabview_remove/tst_tabview_remove.cpp`

- [ ] **Step 1: Create `tests/tests.pro`**

```pro
TEMPLATE = subdirs
SUBDIRS += tst_tabview_remove
```

- [ ] **Step 2: Create `tests/tst_tabview_remove/tst_tabview_remove.pro`**

This test project compiles the app sources (minus main.cpp and MainWindow.cpp) alongside the test file. It includes LVGL and FreeType via their `.pri` files.

```pro
QT += core gui widgets testlib
CONFIG += c++11 testcase
CONFIG += sdk_no_version_check

TARGET = tst_tabview_remove

include(../../lvgl/lvgl.pri)
include(../../freetype/freetype.pri)

INCLUDEPATH += ../../

RESOURCES += ../../resources/res.qrc

SOURCES += \
    tst_tabview_remove.cpp \
    ../../LVGLCore.cpp \
    ../../LVGLFontData.cpp \
    ../../LVGLObject.cpp \
    ../../LVGLProject.cpp \
    ../../LVGLProperty.cpp \
    ../../LVGLStyle.cpp \
    ../../LVGLImageData.cpp \
    ../../properties/LVGLPropertyAssignTextArea.cpp \
    ../../properties/LVGLPropertyColor.cpp \
    ../../properties/LVGLPropertyDate.cpp \
    ../../properties/LVGLPropertyDateList.cpp \
    ../../properties/LVGLPropertyFlags.cpp \
    ../../properties/LVGLPropertyGeometry.cpp \
    ../../properties/LVGLPropertyImage.cpp \
    ../../properties/LVGLPropertyList.cpp \
    ../../properties/LVGLPropertyPoints.cpp \
    ../../properties/LVGLPropertyRange.cpp \
    ../../properties/LVGLPropertyScale.cpp \
    ../../properties/LVGLPropertySeries.cpp \
    ../../properties/LVGLPropertyTextList.cpp \
    ../../properties/LVGLPropertyVal2.cpp \
    ../../widgets/LVGLArc.cpp \
    ../../widgets/LVGLBar.cpp \
    ../../widgets/LVGLButton.cpp \
    ../../widgets/LVGLButtonMatrix.cpp \
    ../../widgets/LVGLCalendar.cpp \
    ../../widgets/LVGLCanvas.cpp \
    ../../widgets/LVGLChart.cpp \
    ../../widgets/LVGLCheckBox.cpp \
    ../../widgets/LVGLColorPicker.cpp \
    ../../widgets/LVGLContainer.cpp \
    ../../widgets/LVGLDropDownList.cpp \
    ../../widgets/LVGLGauge.cpp \
    ../../widgets/LVGLImage.cpp \
    ../../widgets/LVGLImageButton.cpp \
    ../../widgets/LVGLImageSlider.cpp \
    ../../widgets/LVGLKeyboard.cpp \
    ../../widgets/LVGLLabel.cpp \
    ../../widgets/LVGLLED.cpp \
    ../../widgets/LVGLLine.cpp \
    ../../widgets/LVGLLineMeter.cpp \
    ../../widgets/LVGLList.cpp \
    ../../widgets/LVGLMessageBox.cpp \
    ../../widgets/LVGLPage.cpp \
    ../../widgets/LVGLPreloader.cpp \
    ../../widgets/LVGLSlider.cpp \
    ../../widgets/LVGLSwitch.cpp \
    ../../widgets/LVGLTabview.cpp \
    ../../widgets/LVGLTextArea.cpp \
    ../../widgets/LVGLWidget.cpp

HEADERS += \
    ../../LVGLCore.h \
    ../../LVGLFontData.h \
    ../../LVGLObject.h \
    ../../LVGLProject.h \
    ../../LVGLProperty.h \
    ../../LVGLStyle.h \
    ../../LVGLImageData.h \
    ../../properties/LVGLPropertyTextList.h \
    ../../widgets/LVGLWidgets.h \
    ../../widgets/LVGLWidget.h
```

- [ ] **Step 3: Create the initial test file with a minimal build-verification test**

```cpp
// tests/tst_tabview_remove/tst_tabview_remove.cpp
#include <QtTest>

#include "LVGLCore.h"
#include "LVGLObject.h"

class TestTabviewRemove : public QObject {
  Q_OBJECT

 private slots:
  void initTestCase() {
    // Load Qt resources (icons, images) needed by LVGLCore
    Q_INIT_RESOURCE(res);
    // Initialize LVGL display driver with a 320x480 framebuffer
    lvgl.init(320, 480);
  }

  void cleanup() {
    // Remove all builder-tracked objects between tests so each test
    // starts with a clean slate (no stale LVGLObject wrappers).
    lvgl.removeAllObjects();
  }

  // === Minimal build-verification test ===
  // Ensures the test scaffold compiles and links correctly.
  void canCreateTabview() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *tv = lv_tabview_create(screen, nullptr);
    lv_tabview_add_tab(tv, "A");
    QCOMPARE(lv_tabview_get_tab_count(tv), static_cast<uint16_t>(1));
    lv_obj_del(tv);
  }
};

QTEST_MAIN(TestTabviewRemove)
#include "tst_tabview_remove.moc"
```

- [ ] **Step 4: Build and verify the scaffold compiles**

```bash
cd tests/tst_tabview_remove && qmake && make
```

Expected: Compiles and links successfully.

- [ ] **Step 5: Run the test**

```bash
./tst_tabview_remove
```

Expected: `canCreateTabview` PASSES.

- [ ] **Step 6: Commit**

```bash
git add tests/tests.pro tests/tst_tabview_remove/tst_tabview_remove.pro tests/tst_tabview_remove/tst_tabview_remove.cpp
git commit -m "test: add QTest scaffold for tabview remove feature"
```

---

### Task 2: Write failing LVGL-level removal tests

**Files:**
- Modify: `tests/tst_tabview_remove/tst_tabview_remove.cpp`

Add 5 test methods that call `lv_tabview_remove_tab()` — which does not exist yet. The build should fail with a linker error.

- [ ] **Step 1: Add the 5 LVGL-level test methods**

Insert these test slots after `canCreateTabview()` and before the closing `};`:

```cpp
  // =================================================================
  // Round 1: LVGL-level removal tests
  // These test lv_tabview_remove_tab() directly on the LVGL C layer.
  // =================================================================

  // Remove a middle tab from a 3-tab tabview.
  // Verifies: tab_cnt decrements, name array compacts correctly,
  //           tab_cur stays at 0 (unaffected by removal of index 1).
  void removeSingleTab() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *tv = lv_tabview_create(screen, nullptr);
    lv_tabview_add_tab(tv, "A");
    lv_tabview_add_tab(tv, "B");
    lv_tabview_add_tab(tv, "C");

    QCOMPARE(lv_tabview_get_tab_count(tv), static_cast<uint16_t>(3));

    lv_tabview_remove_tab(tv, 1);  // remove "B"

    QCOMPARE(lv_tabview_get_tab_count(tv), static_cast<uint16_t>(2));

    // Remaining tabs should be "A" and "C"
    lv_tabview_ext_t *ext = reinterpret_cast<lv_tabview_ext_t *>(
        lv_obj_get_ext_attr(tv));
    QCOMPARE(QString(ext->tab_name_ptr[0]), QString("A"));
    QCOMPARE(QString(ext->tab_name_ptr[1]), QString("C"));

    // Active tab should still be 0
    QCOMPARE(lv_tabview_get_tab_act(tv), static_cast<uint16_t>(0));

    lv_obj_del(tv);
  }

  // Remove the first tab (index 0) from a 2-tab tabview.
  // Verifies: the surviving tab shifts to index 0, tab_cur clamps to 0.
  void removeFirstTab() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *tv = lv_tabview_create(screen, nullptr);
    lv_tabview_add_tab(tv, "X");
    lv_tabview_add_tab(tv, "Y");

    lv_tabview_remove_tab(tv, 0);  // remove "X"

    QCOMPARE(lv_tabview_get_tab_count(tv), static_cast<uint16_t>(1));
    lv_tabview_ext_t *ext = reinterpret_cast<lv_tabview_ext_t *>(
        lv_obj_get_ext_attr(tv));
    QCOMPARE(QString(ext->tab_name_ptr[0]), QString("Y"));
    QCOMPARE(lv_tabview_get_tab_act(tv), static_cast<uint16_t>(0));

    lv_obj_del(tv);
  }

  // Remove the last tab while it is the active tab.
  // Verifies: tab_cur clamps to tab_cnt - 1 (doesn't go out of bounds).
  void removeLastTabActive() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *tv = lv_tabview_create(screen, nullptr);
    lv_tabview_add_tab(tv, "P");
    lv_tabview_add_tab(tv, "Q");
    lv_tabview_add_tab(tv, "R");

    // Set active to last tab, then remove it
    lv_tabview_set_tab_act(tv, 2, LV_ANIM_OFF);
    lv_tabview_remove_tab(tv, 2);

    QCOMPARE(lv_tabview_get_tab_count(tv), static_cast<uint16_t>(2));
    // Active tab should clamp to 1 (new last tab)
    QCOMPARE(lv_tabview_get_tab_act(tv), static_cast<uint16_t>(1));

    lv_obj_del(tv);
  }

  // Attempt to remove a tab from a tabview with only 1 tab.
  // Verifies: the call is silently ignored (LVGL has no empty tabview state).
  void removeIgnoredWhenOnlyOneTab() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *tv = lv_tabview_create(screen, nullptr);
    lv_tabview_add_tab(tv, "Solo");

    lv_tabview_remove_tab(tv, 0);  // should be a no-op

    QCOMPARE(lv_tabview_get_tab_count(tv), static_cast<uint16_t>(1));
    lv_tabview_ext_t *ext = reinterpret_cast<lv_tabview_ext_t *>(
        lv_obj_get_ext_attr(tv));
    QCOMPARE(QString(ext->tab_name_ptr[0]), QString("Solo"));

    lv_obj_del(tv);
  }

  // Attempt to remove a tab at an out-of-bounds index.
  // Verifies: the call is silently ignored.
  void removeOutOfBoundsIgnored() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *tv = lv_tabview_create(screen, nullptr);
    lv_tabview_add_tab(tv, "A");
    lv_tabview_add_tab(tv, "B");

    lv_tabview_remove_tab(tv, 99);  // out of bounds — no-op

    QCOMPARE(lv_tabview_get_tab_count(tv), static_cast<uint16_t>(2));

    lv_obj_del(tv);
  }
```

- [ ] **Step 2: Build and verify the tests fail**

```bash
cd tests/tst_tabview_remove && make 2>&1 | tail -5
```

Expected: Linker error — `undefined reference to 'lv_tabview_remove_tab'`.

- [ ] **Step 3: Commit**

```bash
git add tests/tst_tabview_remove/tst_tabview_remove.cpp
git commit -m "test: add failing LVGL-level tests for lv_tabview_remove_tab"
```

---

### Task 3: Implement `lv_tabview_remove_tab()` in vendored LVGL

**Files:**
- Modify: `lvgl/lvgl/src/lv_objx/lv_tabview.h:111`
- Modify: `lvgl/lvgl/src/lv_objx/lv_tabview.c:335`

- [ ] **Step 1: Add the declaration to lv_tabview.h**

In `lvgl/lvgl/src/lv_objx/lv_tabview.h`, after `lv_tabview_add_tab` (line 111), add:

```c
/**
 * Remove a tab by index.
 * The tab's page object is deleted, its name string is freed, the button
 * matrix is rebuilt, and the active-tab index is adjusted.
 * Ignored if tab_cnt <= 1 (cannot remove the last tab) or id >= tab_cnt.
 * @param tabview pointer to Tab view object
 * @param id index of the tab to remove (0-based)
 */
void lv_tabview_remove_tab(lv_obj_t * tabview, uint16_t id);
```

- [ ] **Step 2: Implement the function in lv_tabview.c**

In `lvgl/lvgl/src/lv_objx/lv_tabview.c`, after the closing brace of `lv_tabview_add_tab` (line 334), before the `/*===================== Setter functions` comment (line 336), add:

```c
/**
 * Remove a tab by index.
 *
 * Deletes the tab's page object, frees its name string, compacts the
 * name-pointer array, rebuilds the button matrix, adjusts tab_cur,
 * and re-layouts the tabview.
 *
 * Ignored if tab_cnt <= 1 (LVGL has no "empty tabview" state) or
 * id >= tab_cnt (out of bounds).
 *
 * @param tabview pointer to Tab view object
 * @param id      index of the tab to remove (0-based)
 */
void lv_tabview_remove_tab(lv_obj_t * tabview, uint16_t id)
{
    LV_ASSERT_OBJ(tabview, LV_OBJX_NAME);

    lv_tabview_ext_t * ext = lv_obj_get_ext_attr(tabview);

    /* --- Guard: refuse invalid or degenerate removals ---
     * A tabview with 0 or 1 tab cannot lose its last tab (LVGL has no
     * "empty tabview" state). Out-of-bounds ids are silently ignored. */
    if(ext->tab_cnt <= 1) return;
    if(id >= ext->tab_cnt) return;

    /* --- Step 1: Delete the LVGL page object ---
     * Tab pages live as children of ext->content (a container with
     * LV_LAYOUT_ROW_T). lv_obj_get_child_back() iterates oldest-first
     * (creation order), so counting forward `id` times reaches the
     * correct page. lv_obj_del() recursively frees the page and all
     * widgets on it. */
    lv_obj_t * page = lv_obj_get_child_back(ext->content, NULL);
    for(uint16_t i = 0; i < id && page != NULL; i++) {
        page = lv_obj_get_child_back(ext->content, page);
    }
    if(page != NULL) {
        lv_obj_del(page);
    }

    /* --- Step 2: Free the dynamically-allocated name string ---
     * lv_tabview_add_tab() allocates each name via lv_mem_alloc().
     * We must free it before shifting the array. */
    lv_mem_free((void *)ext->tab_name_ptr[id]);

    /* --- Step 3: Shrink tab_cnt, then compact the name-pointer array ---
     *
     * The array format depends on button position:
     *
     * TOP/BOTTOM: [name0, name1, ..., nameN-1, ""]
     *   → (tab_cnt + 1) pointers. The "" sentinel terminates the btnm map.
     *   → Shift entries left starting at `id`, re-terminate with "".
     *
     * LEFT/RIGHT: [name0, "\n", name1, "\n", ..., nameN-1, ""]
     *   → (old_tab_cnt * 2) pointers. "\n" literals (not allocated)
     *     separate button rows.
     *   → First tab (id==0): remove [name0, "\n"] (indices 0-1).
     *     Other tabs: remove ["\n", nameN] (the separator before the name).
     *   → Special case: if only 1 tab remains, array is just [name, ""].
     */
    ext->tab_cnt--;

    switch(ext->btns_pos) {
        case LV_TABVIEW_BTNS_POS_TOP:
        case LV_TABVIEW_BTNS_POS_BOTTOM:
            /* Shift entries left to fill the gap at `id` */
            for(uint16_t i = id; i < ext->tab_cnt; i++) {
                ext->tab_name_ptr[i] = ext->tab_name_ptr[i + 1];
            }
            ext->tab_name_ptr[ext->tab_cnt] = "";
            ext->tab_name_ptr = lv_mem_realloc((void *)ext->tab_name_ptr,
                                               sizeof(char *) * (ext->tab_cnt + 1));
            break;
        case LV_TABVIEW_BTNS_POS_LEFT:
        case LV_TABVIEW_BTNS_POS_RIGHT:
            if(ext->tab_cnt == 1) {
                /* Only one tab left — result is just [survivor_name, ""].
                 * If we removed id==0, the survivor was at old index 1
                 * (old array position 2). If we removed id==1, the survivor
                 * is already at position 0. */
                if(id == 0) {
                    ext->tab_name_ptr[0] = ext->tab_name_ptr[2];
                }
                ext->tab_name_ptr[1] = "";
            } else {
                /* Remove the name and its adjacent "\n" separator.
                 * First tab (id==0): remove positions [0,1] → [name0, "\n"].
                 * Other tabs (id>0): remove positions [id*2-1, id*2] → ["\n", nameN].
                 * Then shift the remaining entries left by 2. */
                uint16_t arr_id;
                if(id == 0) {
                    arr_id = 0;        /* remove [name0, "\n"] */
                } else {
                    arr_id = id * 2 - 1; /* remove ["\n", nameN] */
                }
                /* old total entries = (ext->tab_cnt + 1) * 2  (before decrement) */
                uint16_t old_total = (ext->tab_cnt + 1) * 2;
                for(uint16_t i = arr_id; i + 2 < old_total; i++) {
                    ext->tab_name_ptr[i] = ext->tab_name_ptr[i + 2];
                }
            }
            ext->tab_name_ptr = lv_mem_realloc((void *)ext->tab_name_ptr,
                                               sizeof(char *) * (ext->tab_cnt * 2));
            break;
    }

    /* --- Step 4: Rebuild the button matrix ---
     * The old btnm map pointer may have been freed by lv_mem_realloc,
     * so we must invalidate it before setting the new map. */
    lv_btnm_ext_t * btnm_ext = lv_obj_get_ext_attr(ext->btns);
    btnm_ext->map_p = NULL;
    lv_btnm_set_map(ext->btns, ext->tab_name_ptr);

    /* --- Step 5: Adjust active tab index ---
     * Three cases:
     *   tab_cur > id  → decrement (tabs above shifted down)
     *   tab_cur == id and at end → clamp to tab_cnt - 1
     *   tab_cur < id  → unchanged
     */
    if(ext->tab_cur >= ext->tab_cnt) {
        ext->tab_cur = ext->tab_cnt - 1;
    } else if(ext->tab_cur > id) {
        ext->tab_cur--;
    }

    lv_btnm_set_btn_ctrl(ext->btns, ext->tab_cur, LV_BTNM_CTRL_NO_REPEAT);

    /* --- Step 6: Re-layout ---
     * tabview_realign() resizes all page objects, repositions buttons
     * and indicator. lv_tabview_set_tab_act() scrolls content to the
     * (possibly new) active tab and updates the toggle highlight. */
    tabview_realign(tabview);
    lv_tabview_set_tab_act(tabview, ext->tab_cur, LV_ANIM_OFF);
}
```

- [ ] **Step 3: Build and run the LVGL-level tests**

```bash
cd tests/tst_tabview_remove && make && ./tst_tabview_remove
```

Expected: All 6 tests PASS (`canCreateTabview`, `removeSingleTab`, `removeFirstTab`, `removeLastTabActive`, `removeIgnoredWhenOnlyOneTab`, `removeOutOfBoundsIgnored`).

- [ ] **Step 4: Commit**

```bash
git add lvgl/lvgl/src/lv_objx/lv_tabview.h lvgl/lvgl/src/lv_objx/lv_tabview.c
git commit -m "feat: add lv_tabview_remove_tab() to vendored LVGL 6.1"
```

---

### Task 4: Write failing LVGLObject detach tests

**Files:**
- Modify: `tests/tst_tabview_remove/tst_tabview_remove.cpp`

- [ ] **Step 1: Add the 2 detach test methods**

Insert these test slots after `removeOutOfBoundsIgnored()`:

```cpp
  // =================================================================
  // Round 2: LVGLObject detach tests
  // These test the double-delete prevention mechanism.
  //
  // Background: lv_tabview_remove_tab() calls lv_obj_del(page), which
  // deletes the LVGL page. But ~LVGLObject() also calls lv_obj_del(m_obj).
  // If both run, the app crashes (double-free). detachLvObj() sets
  // m_obj = nullptr so the destructor becomes a safe no-op.
  // =================================================================

  // Verify that detaching + deleting a wrapper doesn't crash.
  // Without the destructor guard, this would double-free.
  void detachPreventsDoubleDelete() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *tv = lv_tabview_create(screen, nullptr);
    lv_obj_t *page_obj = lv_tabview_add_tab(tv, "Tab1");

    // Create a builder wrapper for the page
    const LVGLWidget *pageWidget = lvgl.widget("lv_page");
    QVERIFY(pageWidget != nullptr);
    LVGLObject *pageWrapper = new LVGLObject(page_obj, pageWidget, nullptr, false, 0);

    // Detach: m_obj becomes null, so destructor won't call lv_obj_del
    pageWrapper->detachLvObj();
    QVERIFY(pageWrapper->obj() == nullptr);

    // This delete should NOT crash — destructor skips lv_obj_del
    delete pageWrapper;

    // The LVGL page still exists (owned by tabview) — clean up properly
    lv_obj_del(tv);
  }

  // Verify that detachLvObjRecursive() nulls out the page wrapper
  // AND all its descendant wrappers (simulating widgets placed on the page).
  void detachRecursive() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *tv = lv_tabview_create(screen, nullptr);
    lv_obj_t *page_obj = lv_tabview_add_tab(tv, "Tab1");

    const LVGLWidget *pageWidget = lvgl.widget("lv_page");
    const LVGLWidget *labelWidget = lvgl.widget("lv_label");
    QVERIFY(pageWidget != nullptr);
    QVERIFY(labelWidget != nullptr);

    // Create a page wrapper with a child label wrapper
    // (simulates a user placing a label widget on the tab page)
    LVGLObject *pageWrapper = new LVGLObject(page_obj, pageWidget, nullptr, false, 0);
    lv_obj_t *lbl = lv_label_create(page_obj, nullptr);
    LVGLObject *lblWrapper = new LVGLObject(lbl, labelWidget, pageWrapper, true, -1);

    QVERIFY(pageWrapper->obj() != nullptr);
    QVERIFY(lblWrapper->obj() != nullptr);

    // Recursive detach: both page and its child label should be nulled
    pageWrapper->detachLvObjRecursive();

    QVERIFY(pageWrapper->obj() == nullptr);
    QVERIFY(lblWrapper->obj() == nullptr);

    // Clean up wrappers (destructors are safe — m_obj is null)
    delete lblWrapper;
    delete pageWrapper;

    lv_obj_del(tv);
  }
```

- [ ] **Step 2: Build and verify the tests fail**

```bash
cd tests/tst_tabview_remove && make 2>&1 | tail -5
```

Expected: Compile error — `'class LVGLObject' has no member named 'detachLvObj'` and `'detachLvObjRecursive'`.

- [ ] **Step 3: Commit**

```bash
git add tests/tst_tabview_remove/tst_tabview_remove.cpp
git commit -m "test: add failing tests for LVGLObject detach mechanism"
```

---

### Task 5: Implement `detachLvObj()` and `detachLvObjRecursive()` on LVGLObject

**Files:**
- Modify: `LVGLObject.h:79-81`
- Modify: `LVGLObject.cpp:42-45`

- [ ] **Step 1: Add declarations in LVGLObject.h**

In `LVGLObject.h`, after `LVGLObject *findChildByIndex(int index) const;` (line 81), add:

```cpp
	/**
	 * @brief Detach the underlying lv_obj_t so the destructor won't delete it.
	 *
	 * Call this when an external owner (e.g. lv_tabview_remove_tab) will
	 * delete the lv_obj_t. Sets m_obj to nullptr so ~LVGLObject() is a no-op.
	 */
	void detachLvObj();

	/**
	 * @brief Recursively detach this object and all descendant LVGLObjects.
	 *
	 * Use when removing a container (e.g. a tab page) that has child widgets.
	 * lv_obj_del() on the container recursively frees all LVGL children,
	 * so every wrapper in the subtree must be detached to prevent double-free.
	 */
	void detachLvObjRecursive();
```

- [ ] **Step 2: Add destructor guard and implement methods in LVGLObject.cpp**

Replace the destructor at `LVGLObject.cpp:42-45`:

```cpp
LVGLObject::~LVGLObject()
{
	lv_obj_del(m_obj);
}
```

with:

```cpp
LVGLObject::~LVGLObject()
{
	// Guard: if m_obj was detached (set to nullptr), the LVGL object is
	// being deleted by an external owner (e.g. lv_tabview_remove_tab).
	// Calling lv_obj_del on a null/freed pointer would crash.
	if (!m_obj) return;
	lv_obj_del(m_obj);
}
```

After the destructor (before `lv_obj_t *LVGLObject::obj() const` at line 47), add:

```cpp
void LVGLObject::detachLvObj()
{
	m_obj = nullptr;
}

void LVGLObject::detachLvObjRecursive()
{
	// Detach all children first (depth-first), then detach self.
	// This mirrors lv_obj_del's recursive deletion of LVGL children.
	for (LVGLObject *child : m_childs) {
		child->detachLvObjRecursive();
	}
	m_obj = nullptr;
}
```

- [ ] **Step 3: Build and run all tests**

```bash
cd tests/tst_tabview_remove && make && ./tst_tabview_remove
```

Expected: All 8 tests PASS.

- [ ] **Step 4: Commit**

```bash
git add LVGLObject.h LVGLObject.cpp
git commit -m "feat: add LVGLObject::detachLvObj() to prevent double-delete"
```

---

### Task 6: Write failing builder property integration tests

**Files:**
- Modify: `tests/tst_tabview_remove/tst_tabview_remove.cpp`

These tests exercise `LVGLPropertyTabs::set()` through the public `LVGLProperty::setValue()` interface, testing the by-name matching algorithm at the builder level.

- [ ] **Step 1: Add the 4 property integration test methods**

Insert these test slots after `detachRecursive()`:

```cpp
  // =================================================================
  // Round 3: Builder property integration tests
  // These test LVGLPropertyTabs::set() — the by-name matching algorithm
  // that detects which tabs were removed, renames survivors, and appends
  // new tabs.
  //
  // The test flow mimics what happens when a user edits tabs in the
  // property dialog: setValue() receives the new list from the dialog,
  // and set() reconciles it with the current LVGL tab state.
  // =================================================================

  // Helper: create a tabview with N tabs via the builder property system.
  // Returns the LVGLObject wrapper for the tabview.
  LVGLObject *createTabviewWithTabs(QStringList tabNames) {
    const LVGLWidget *tvWidget = lvgl.widget("lv_tabview");
    Q_ASSERT(tvWidget != nullptr);
    lv_obj_t *screen = lv_scr_act();
    LVGLObject *tvObj = new LVGLObject(tvWidget, "test_tv", screen);
    lvgl.addObject(tvObj);

    // Use the "Tabs" property to add initial tabs
    LVGLProperty *tabsProp = tvWidget->property("Tabs");
    Q_ASSERT(tabsProp != nullptr);
    QVariantList varList;
    for (const QString &name : tabNames)
      varList.append(name);
    tabsProp->setValue(tvObj, QVariant(varList));

    return tvObj;
  }

  // Core test: old=[A, B, C], new=[A, C]
  // Phase 1 should detect "B" was removed (by-name matching), keeping
  // "C" in place rather than trimming from the end.
  void propertyRemovesByName() {
    LVGLObject *tvObj = createTabviewWithTabs({"A", "B", "C"});
    lv_tabview_ext_t *ext = reinterpret_cast<lv_tabview_ext_t *>(
        lv_obj_get_ext_attr(tvObj->obj()));

    // Verify initial state: 3 tabs, 3 child LVGLObjects
    QCOMPARE(ext->tab_cnt, static_cast<uint16_t>(3));
    QCOMPARE(tvObj->childs().size(), 3);

    // Simulate user removing "B" in the dialog → new list [A, C]
    LVGLProperty *tabsProp = lvgl.widget("lv_tabview")->property("Tabs");
    tabsProp->setValue(tvObj, QVariant(QVariantList{{"A"}, {"C"}}));

    // Verify: tab_cnt=2, names=[A,C], 2 child wrappers
    QCOMPARE(ext->tab_cnt, static_cast<uint16_t>(2));
    QCOMPARE(tvObj->childs().size(), 2);
    QCOMPARE(QString(ext->tab_name_ptr[0]), QString("A"));
    QCOMPARE(QString(ext->tab_name_ptr[1]), QString("C"));
  }

  // Remove + add: old=[A, B, C], new=[A, C, D]
  // Phase 1 removes "B", Phase 3 appends "D".
  void propertyRemoveAndAdd() {
    LVGLObject *tvObj = createTabviewWithTabs({"A", "B", "C"});
    lv_tabview_ext_t *ext = reinterpret_cast<lv_tabview_ext_t *>(
        lv_obj_get_ext_attr(tvObj->obj()));

    LVGLProperty *tabsProp = lvgl.widget("lv_tabview")->property("Tabs");
    tabsProp->setValue(tvObj, QVariant(QVariantList{{"A"}, {"C"}, {"D"}}));

    QCOMPARE(ext->tab_cnt, static_cast<uint16_t>(3));
    QCOMPARE(tvObj->childs().size(), 3);
    QCOMPARE(QString(ext->tab_name_ptr[0]), QString("A"));
    QCOMPARE(QString(ext->tab_name_ptr[1]), QString("C"));
    QCOMPARE(QString(ext->tab_name_ptr[2]), QString("D"));
  }

  // Pure rename: old=[A, B], new=[A, X] — no removal, Phase 2 renames.
  void propertyRenameOnly() {
    LVGLObject *tvObj = createTabviewWithTabs({"A", "B"});
    lv_tabview_ext_t *ext = reinterpret_cast<lv_tabview_ext_t *>(
        lv_obj_get_ext_attr(tvObj->obj()));

    LVGLProperty *tabsProp = lvgl.widget("lv_tabview")->property("Tabs");
    tabsProp->setValue(tvObj, QVariant(QVariantList{{"A"}, {"X"}}));

    QCOMPARE(ext->tab_cnt, static_cast<uint16_t>(2));
    QCOMPARE(tvObj->childs().size(), 2);
    QCOMPARE(QString(ext->tab_name_ptr[0]), QString("A"));
    QCOMPARE(QString(ext->tab_name_ptr[1]), QString("X"));
  }

  // No-op: old=[A, B], new=[A, B] — nothing changes.
  void propertyNoChange() {
    LVGLObject *tvObj = createTabviewWithTabs({"A", "B"});
    lv_tabview_ext_t *ext = reinterpret_cast<lv_tabview_ext_t *>(
        lv_obj_get_ext_attr(tvObj->obj()));

    LVGLProperty *tabsProp = lvgl.widget("lv_tabview")->property("Tabs");
    tabsProp->setValue(tvObj, QVariant(QVariantList{{"A"}, {"B"}}));

    QCOMPARE(ext->tab_cnt, static_cast<uint16_t>(2));
    QCOMPARE(tvObj->childs().size(), 2);
    QCOMPARE(QString(ext->tab_name_ptr[0]), QString("A"));
    QCOMPARE(QString(ext->tab_name_ptr[1]), QString("B"));
  }
```

- [ ] **Step 2: Build and verify the removal test fails**

```bash
cd tests/tst_tabview_remove && make && ./tst_tabview_remove
```

Expected: `propertyRemovesByName` FAILS — tab count stays at 3 because `set()` doesn't handle removal yet. `propertyRenameOnly` and `propertyNoChange` should PASS (existing code handles these). `propertyRemoveAndAdd` should FAIL.

- [ ] **Step 3: Commit**

```bash
git add tests/tst_tabview_remove/tst_tabview_remove.cpp
git commit -m "test: add failing tests for builder property tab removal"
```

---

### Task 7: Rewrite `LVGLPropertyTabs::set()` with three-phase by-name matching

**Files:**
- Modify: `widgets/LVGLTabview.cpp:49-76`

- [ ] **Step 1: Replace the `set()` method**

In `widgets/LVGLTabview.cpp`, replace the `LVGLPropertyTabs::set()` method (lines 49–76):

```cpp
	inline void set(LVGLObject *obj, QStringList list) {
		lv_tabview_ext_t * ext = reinterpret_cast<lv_tabview_ext_t*>(lv_obj_get_ext_attr(obj->obj()));
		// rename
		for (uint16_t i = 0; i < qMin(ext->tab_cnt, static_cast<uint16_t>(list.size())); ++i) {
			QByteArray name = list.at(i).toLatin1();
			if (strcmp(ext->tab_name_ptr[i], name.data()) == 0)
				continue;

			char * name_dm = reinterpret_cast<char*>(lv_mem_alloc(name.size()));
			if (name_dm == nullptr)
				continue;

			memcpy(name_dm, name, name.size());
			name_dm[name.size()] = '\0';
			ext->tab_name_ptr[i] = name_dm;

			lv_btnm_set_map(ext->btns, ext->tab_name_ptr);
			lv_btnm_set_btn_ctrl(ext->btns, ext->tab_cur, LV_BTNM_CTRL_NO_REPEAT);
		}

		// add new
		for (uint16_t i = ext->tab_cnt; i < list.size(); ++i) {
			//lv_tabview_add_tab(obj->obj(), qPrintable(list.at(i)));
			lv_obj_t *page_obj = lv_tabview_add_tab(obj->obj(), qPrintable(list.at(i)));
			LVGLObject *page = new LVGLObject(page_obj, lvgl.widget("lv_page"), obj, false, i);
			lvgl.addObject(page);
		}
	}
```

with the new three-phase implementation:

```cpp
	inline void set(LVGLObject *obj, QStringList list) {
		lv_tabview_ext_t * ext = reinterpret_cast<lv_tabview_ext_t*>(lv_obj_get_ext_attr(obj->obj()));

		// ================================================================
		// PHASE 1: Name-based removal — detect which tabs were deleted
		// ================================================================
		//
		// Strategy: use a name→count map to match old tabs against new tabs.
		//
		// Example: old=[A, B, C], new=[A, C]
		//   countMap from new: {A:1, C:1}
		//   Walk old: A → found (A:0), B → not found → REMOVE, C → found (C:0)
		//   toRemove = [1]  (index of "B")
		//
		// Why a count map instead of a simple set?
		//   Handles duplicate tab names. If old=[A, A, B] and new=[A, B],
		//   the first A survives (count 1→0), the second A is removed (count=0).

		// Build name→count map from the new list
		QMap<QString, int> countMap;
		for (const QString &name : list)
			countMap[name]++;

		// Walk old tabs, consume counts to decide survive vs. remove
		QList<int> toRemove;
		for (uint16_t i = 0; i < ext->tab_cnt; ++i) {
			QString oldName(ext->tab_name_ptr[i]);
			if (countMap.value(oldName, 0) > 0) {
				countMap[oldName]--;   // this old tab survives
			} else {
				toRemove.append(i);   // this old tab was deleted by user
			}
		}

		// Process removals from HIGHEST index to LOWEST.
		// Why reverse order? When we remove index 2, indices 0 and 1 are
		// unaffected. If we removed index 0 first, all higher indices would
		// shift down and our stored indices would be wrong.
		for (int r = toRemove.size() - 1; r >= 0; --r) {
			int i = toRemove[r];

			// --- Clean up the builder-side wrapper tree ---
			// There are TWO parallel object trees:
			//   LVGL tree:    lv_obj_t (tabview) → lv_obj_t (page) → lv_obj_t (children)
			//   Builder tree: LVGLObject (tabview) → LVGLObject (page) → LVGLObject (children)
			//
			// lv_tabview_remove_tab() deletes the LVGL page + all LVGL children.
			// We must detach all wrappers first so their destructors don't
			// also call lv_obj_del() on already-freed objects (double-free crash).
			LVGLObject *page = obj->findChildByIndex(i);
			if (page) {
				page->detachLvObjRecursive();  // null out m_obj on page + descendants
				lvgl.removeObject(page);       // remove from builder tree + delete wrappers
			}

			// Now let LVGL do the actual object deletion + tabview bookkeeping
			lv_tabview_remove_tab(obj->obj(), static_cast<uint16_t>(i));
		}

		// ================================================================
		// PHASE 2: Positional rename — handle in-place text edits
		// ================================================================
		//
		// After removals, the surviving LVGL tabs are at indices 0..tab_cnt-1.
		// Compare each with list[i]. If they differ, the user edited the
		// text in the dialog → rename in place.
		//
		// Example: old=[A, B] → no removals → LVGL has [A, B]
		//          new=[A, X] → A=A ok, B≠X → rename B to X.

		// Re-read ext since tab_cnt may have changed during removals
		ext = reinterpret_cast<lv_tabview_ext_t*>(lv_obj_get_ext_attr(obj->obj()));
		for (uint16_t i = 0; i < qMin(ext->tab_cnt, static_cast<uint16_t>(list.size())); ++i) {
			QByteArray name = list.at(i).toLatin1();
			if (strcmp(ext->tab_name_ptr[i], name.data()) == 0)
				continue;  // name unchanged, skip

			// Allocate new LVGL name string and replace the old one
			char * name_dm = reinterpret_cast<char*>(lv_mem_alloc(static_cast<uint32_t>(name.size() + 1)));
			if (name_dm == nullptr)
				continue;

			memcpy(name_dm, name.constData(), static_cast<size_t>(name.size() + 1));
			ext->tab_name_ptr[i] = name_dm;

			lv_btnm_set_map(ext->btns, ext->tab_name_ptr);
			lv_btnm_set_btn_ctrl(ext->btns, ext->tab_cur, LV_BTNM_CTRL_NO_REPEAT);
		}

		// ================================================================
		// PHASE 3: Append new tabs — handle additions
		// ================================================================
		//
		// Any list entries beyond the current tab_cnt are brand-new tabs.
		// LVGL only supports appending (no insert-at-position).
		//
		// Example: old=[A, C] (after removal), new=[A, C, D]
		//          → tab_cnt=2, list.size()=3 → add "D" at the end.

		for (int i = ext->tab_cnt; i < list.size(); ++i) {
			lv_obj_t *page_obj = lv_tabview_add_tab(obj->obj(), qPrintable(list.at(i)));
			LVGLObject *page = new LVGLObject(page_obj, lvgl.widget("lv_page"), obj, false, i);
			lvgl.addObject(page);
		}
	}
```

- [ ] **Step 2: Build and run all tests**

```bash
cd tests/tst_tabview_remove && make && ./tst_tabview_remove
```

Expected: All 12 tests PASS.

- [ ] **Step 3: Commit**

```bash
git add widgets/LVGLTabview.cpp
git commit -m "feat: wire tab removal into builder property editor with by-name matching"
```

---

### Task 8: Verify existing app still builds (regression check)

**Files:** None modified — verification only.

- [ ] **Step 1: Build the main application**

```bash
cd /Users/chienlinsu/Documents/00-Github/LVGLBuilder && qmake && make
```

Expected: Compiles and links without errors. The changes to `LVGLObject.h`, `LVGLObject.cpp`, `lv_tabview.h`, `lv_tabview.c`, and `LVGLTabview.cpp` are all backward-compatible.

- [ ] **Step 2: Run all tests one final time**

```bash
cd tests/tst_tabview_remove && ./tst_tabview_remove
```

Expected: All 12 tests PASS.

- [ ] **Step 3: Optional manual smoke test**

```
1. Launch ./LVGLBuilder
2. Drag a TabView onto the canvas
3. Select the TabView, find "Tabs" property, click "..." editor button
4. Add tabs "A", "B", "C" → press OK → three tabs appear
5. Re-open editor, select "B", click trash icon → press OK → "B" disappears, "A" and "C" remain
6. Re-open editor, verify list shows ["A", "C"]
```

---
