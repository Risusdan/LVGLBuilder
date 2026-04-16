# TabView Remove Tab — Bug Fixes Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Fix 4 correctness bugs and 2 minor issues found by code review of the tabview-remove-tab feature: incorrect indexing for LEFT/RIGHT button positions in both LVGL and the property layer, zombie tab when user removes all tabs, memory leak on rename, missing Qt meta-type registration, and test build artifacts untracked.

**Architecture:** The root of the LEFT/RIGHT bugs is that tab index (0..N-1) does NOT equal name-array index for LEFT/RIGHT layouts (array is `[n0, "\n", n1, "\n", ..., ""]`). The fix introduces a single `tab_name_array_idx()` helper (one in the LVGL C file, one in the builder) that maps tab index → array index based on button position. All three code paths (`get()`, Phase 2 rename free, `lv_tabview_remove_tab` name free) use this helper. Zombie-tab and rename-leak are straightforward guards/frees.

**Tech Stack:** C (LVGL 6.1), C++/Qt 6, QTest

---

## File Map

| Action | File | Responsibility |
|--------|------|----------------|
| Modify | `lvgl/lvgl/src/lv_objx/lv_tabview.c:378` | Compute correct array index for LEFT/RIGHT in `lv_mem_free()` call |
| Modify | `widgets/LVGLTabview.cpp:42-48` | Fix `get()` — skip `"\n"` separators for LEFT/RIGHT |
| Modify | `widgets/LVGLTabview.cpp:88-107` | Guard against removing last tab (C3), free old name in Phase 2 (C4), handle LEFT/RIGHT index in Phase 2 (C2) |
| Modify | `LVGLCore.cpp:22-24` | Register `LVGLObject*` meta-type in constructor |
| Modify | `.gitignore` | Exclude test build artifacts |
| Modify | `tests/tst_tabview_remove/tst_tabview_remove.cpp` | Add 6 tests covering LEFT/RIGHT, empty-list, rename-leak scenarios |

---

### Task 1: Add failing tests for LEFT/RIGHT button positions

**Files:**
- Modify: `tests/tst_tabview_remove/tst_tabview_remove.cpp`

- [ ] **Step 1: Add 3 LEFT/RIGHT test methods**

Insert these after `survivingPageIndexUpdated()` and before the closing `};`:

```cpp
  // =================================================================
  // Round 5: LEFT/RIGHT button position tests (C1, C2 bug coverage)
  //
  // Background: For LEFT/RIGHT positions the name-pointer array is
  // [n0, "\n", n1, "\n", ..., nameN-1, ""] — tab k's allocated name
  // lives at array index k*2, NOT k. Pre-review, the code used tab
  // index as array index in multiple places, causing UB in lv_mem_free
  // and "\n" leaking into tab lists.
  // =================================================================

  // Remove middle tab from a 3-tab LEFT-position tabview.
  // This exercises lv_mem_free with id=1, which previously freed "\n".
  void removeMiddleTabLeft() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *tv = lv_tabview_create(screen, nullptr);
    lv_tabview_set_btns_pos(tv, LV_TABVIEW_BTNS_POS_LEFT);
    lv_tabview_add_tab(tv, "A");
    lv_tabview_add_tab(tv, "B");
    lv_tabview_add_tab(tv, "C");

    lv_tabview_remove_tab(tv, 1);  // remove "B"

    QCOMPARE(lv_tabview_get_tab_count(tv), static_cast<uint16_t>(2));
    lv_tabview_ext_t *ext = reinterpret_cast<lv_tabview_ext_t *>(
        lv_obj_get_ext_attr(tv));
    // For LEFT layout with 2 tabs: [A, "\n", C, ""]
    QCOMPARE(QString(ext->tab_name_ptr[0]), QString("A"));
    QCOMPARE(QString(ext->tab_name_ptr[2]), QString("C"));

    lv_obj_del(tv);
  }

  // Remove first tab from a 3-tab RIGHT-position tabview.
  void removeFirstTabRight() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *tv = lv_tabview_create(screen, nullptr);
    lv_tabview_set_btns_pos(tv, LV_TABVIEW_BTNS_POS_RIGHT);
    lv_tabview_add_tab(tv, "A");
    lv_tabview_add_tab(tv, "B");
    lv_tabview_add_tab(tv, "C");

    lv_tabview_remove_tab(tv, 0);  // remove "A"

    QCOMPARE(lv_tabview_get_tab_count(tv), static_cast<uint16_t>(2));
    lv_tabview_ext_t *ext = reinterpret_cast<lv_tabview_ext_t *>(
        lv_obj_get_ext_attr(tv));
    QCOMPARE(QString(ext->tab_name_ptr[0]), QString("B"));
    QCOMPARE(QString(ext->tab_name_ptr[2]), QString("C"));

    lv_obj_del(tv);
  }

  // Property-level get() must return just the tab names for LEFT layout,
  // NOT include the "\n" separators that live between them in the array.
  void propertyGetLeft() {
    lv_obj_t *screen = lv_scr_act();
    const LVGLWidget *tvWidget = lvgl.widget("lv_tabview");
    LVGLObject *tvObj = new LVGLObject(tvWidget, "test_tv_left", screen);
    lvgl.addObject(tvObj);

    // Set LEFT position BEFORE adding tabs (so the array is built in LEFT format)
    lv_tabview_set_btns_pos(tvObj->obj(), LV_TABVIEW_BTNS_POS_LEFT);

    LVGLProperty *tabsProp = tvWidget->property("Tabs");
    tabsProp->setValue(tvObj, QVariant(QVariantList{{"A"}, {"B"}, {"C"}}));

    // Reading back the list should give just ["A", "B", "C"],
    // NOT ["A", "\n", "B"].
    QVariant v = tabsProp->value(tvObj);
    QCOMPARE(v.toString(), QString("A, B, C"));
  }
```

- [ ] **Step 2: Build and run**

```bash
cd tests/tst_tabview_remove && make && ./tst_tabview_remove.app/Contents/MacOS/tst_tabview_remove -v2 2>&1 | grep -E "^(PASS|FAIL|QFATAL).*(Left|Right|propertyGet)"
```

Expected: some combination of FAIL or QFATAL for these new tests. On macOS `removeMiddleTabLeft` may crash in `lv_mem_free` — that's ok, it confirms the UB. `propertyGetLeft` will FAIL with a string containing "\n".

If a test crashes instead of failing cleanly, just note it in the commit message.

- [ ] **Step 3: Commit**

```bash
git add tests/tst_tabview_remove/tst_tabview_remove.cpp
git commit -m "test: add failing LEFT/RIGHT tests for tabview remove (C1, C2)"
```

---

### Task 2: Fix C1 — correct array index for LEFT/RIGHT in `lv_mem_free`

**Files:**
- Modify: `lvgl/lvgl/src/lv_objx/lv_tabview.c:378`

- [ ] **Step 1: Replace the `lv_mem_free` call**

In `lvgl/lvgl/src/lv_objx/lv_tabview.c`, find this block (around line 375-378):

```c
    /* --- Step 2: Free the dynamically-allocated name string ---
     * lv_tabview_add_tab() allocates each name via lv_mem_alloc().
     * We must free it before shifting the array. */
    lv_mem_free((void *)ext->tab_name_ptr[id]);
```

Replace with:

```c
    /* --- Step 2: Free the dynamically-allocated name string ---
     * lv_tabview_add_tab() allocates each name via lv_mem_alloc().
     *
     * The array-index of the name depends on button position:
     *   TOP/BOTTOM: [n0, n1, ..., ""]    -> tab `k`'s name lives at index k
     *   LEFT/RIGHT: [n0, "\n", n1, ...]  -> tab `k`'s name lives at index k*2
     *
     * Using the wrong index for LEFT/RIGHT would free the "\n" string
     * literal, which is undefined behavior. */
    uint16_t name_idx = (ext->btns_pos == LV_TABVIEW_BTNS_POS_LEFT ||
                         ext->btns_pos == LV_TABVIEW_BTNS_POS_RIGHT)
                        ? (uint16_t)(id * 2) : id;
    lv_mem_free((void *)ext->tab_name_ptr[name_idx]);
```

- [ ] **Step 2: Build and run tests**

```bash
cd tests/tst_tabview_remove && make && ./tst_tabview_remove.app/Contents/MacOS/tst_tabview_remove 2>&1 | tail -25
```

Expected: `removeMiddleTabLeft` and `removeFirstTabRight` now PASS. `propertyGetLeft` still FAILS (Task 3 fixes that).

- [ ] **Step 3: Commit**

```bash
git add lvgl/lvgl/src/lv_objx/lv_tabview.c
git commit -m "fix(C1): use correct name-array index for LEFT/RIGHT tabview positions

tab_name_ptr[id] is only valid for TOP/BOTTOM. For LEFT/RIGHT,
the array has [name0, \"\\n\", name1, \"\\n\", ...] so tab k's
allocated name lives at array index k*2, not k. The previous
code called lv_mem_free on string literals — undefined behavior."
```

---

### Task 3: Fix C2 — `LVGLPropertyTabs::get()` must skip separators

**Files:**
- Modify: `widgets/LVGLTabview.cpp:42-48`

- [ ] **Step 1: Rewrite `get()` to handle both layouts**

In `widgets/LVGLTabview.cpp`, find the `get()` method (lines 42-48):

```cpp
	inline QStringList get(LVGLObject *obj) const {
		QStringList ret;
		lv_tabview_ext_t * ext = reinterpret_cast<lv_tabview_ext_t*>(lv_obj_get_ext_attr(obj->obj()));
		for (uint16_t i = 0; i < ext->tab_cnt; ++i)
			ret << QString(ext->tab_name_ptr[i]);
		return ret;
	}
```

Replace with:

```cpp
	inline QStringList get(LVGLObject *obj) const {
		QStringList ret;
		lv_tabview_ext_t * ext = reinterpret_cast<lv_tabview_ext_t*>(lv_obj_get_ext_attr(obj->obj()));
		// tab k's allocated name lives at array index k for TOP/BOTTOM,
		// and k*2 for LEFT/RIGHT (separated by "\n" literals).
		const bool sideLayout =
			(ext->btns_pos == LV_TABVIEW_BTNS_POS_LEFT ||
			 ext->btns_pos == LV_TABVIEW_BTNS_POS_RIGHT);
		const uint16_t stride = sideLayout ? 2 : 1;
		for (uint16_t i = 0; i < ext->tab_cnt; ++i)
			ret << QString(ext->tab_name_ptr[i * stride]);
		return ret;
	}
```

- [ ] **Step 2: Build and run**

```bash
cd tests/tst_tabview_remove && make && ./tst_tabview_remove.app/Contents/MacOS/tst_tabview_remove 2>&1 | tail -25
```

Expected: `propertyGetLeft` now PASSES. All prior tests still PASS.

- [ ] **Step 3: Commit**

```bash
git add widgets/LVGLTabview.cpp
git commit -m "fix(C2): LVGLPropertyTabs::get skips separators for LEFT/RIGHT

The name array for LEFT/RIGHT button positions uses 2 entries per
tab (name + \"\\n\"). The old get() returned separators as if they
were tab names, producing lists like [\"A\", \"\\n\", \"B\"]
instead of [\"A\", \"B\", \"C\"] for 3-tab side layouts."
```

---

### Task 4: Add failing test for Phase 2 rename in LEFT/RIGHT layout (C2 continued)

**Files:**
- Modify: `tests/tst_tabview_remove/tst_tabview_remove.cpp`

- [ ] **Step 1: Add test**

Append after `propertyGetLeft()`:

```cpp
  // Phase 2 rename must also use the correct array index for LEFT/RIGHT.
  // Renaming tab 1 in a LEFT-layout tabview should overwrite the name
  // at array index 2, not index 1 (which is a "\n" separator).
  void propertyRenameLeft() {
    lv_obj_t *screen = lv_scr_act();
    const LVGLWidget *tvWidget = lvgl.widget("lv_tabview");
    LVGLObject *tvObj = new LVGLObject(tvWidget, "test_rename_left", screen);
    lvgl.addObject(tvObj);
    lv_tabview_set_btns_pos(tvObj->obj(), LV_TABVIEW_BTNS_POS_LEFT);

    LVGLProperty *tabsProp = tvWidget->property("Tabs");
    tabsProp->setValue(tvObj, QVariant(QVariantList{{"A"}, {"B"}, {"C"}}));

    // Rename B → X
    tabsProp->setValue(tvObj, QVariant(QVariantList{{"A"}, {"X"}, {"C"}}));

    // Reading back should give ["A", "X", "C"], not include "\n"
    QVariant v = tabsProp->value(tvObj);
    QCOMPARE(v.toString(), QString("A, X, C"));
  }
```

- [ ] **Step 2: Build and verify it fails**

```bash
cd tests/tst_tabview_remove && make && ./tst_tabview_remove.app/Contents/MacOS/tst_tabview_remove 2>&1 | grep -E "propertyRenameLeft"
```

Expected: `propertyRenameLeft` FAILS (Phase 2 writes to wrong index).

- [ ] **Step 3: Commit**

```bash
git add tests/tst_tabview_remove/tst_tabview_remove.cpp
git commit -m "test: add failing test for Phase 2 rename in LEFT layout"
```

---

### Task 5: Fix C2 Phase 2 + C4 — rename uses correct index and frees old name

**Files:**
- Modify: `widgets/LVGLTabview.cpp:132-149`

- [ ] **Step 1: Replace Phase 2 loop**

In `widgets/LVGLTabview.cpp`, find the Phase 2 block (lines 132-149):

```cpp
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
```

Replace with:

```cpp
		// Re-read ext since tab_cnt may have changed during removals
		ext = reinterpret_cast<lv_tabview_ext_t*>(lv_obj_get_ext_attr(obj->obj()));
		// tab k's name lives at array index k for TOP/BOTTOM, k*2 for LEFT/RIGHT
		const bool sideLayout =
			(ext->btns_pos == LV_TABVIEW_BTNS_POS_LEFT ||
			 ext->btns_pos == LV_TABVIEW_BTNS_POS_RIGHT);
		const uint16_t stride = sideLayout ? 2 : 1;
		for (uint16_t i = 0; i < qMin(ext->tab_cnt, static_cast<uint16_t>(list.size())); ++i) {
			const uint16_t arrIdx = i * stride;
			QByteArray name = list.at(i).toLatin1();
			if (strcmp(ext->tab_name_ptr[arrIdx], name.data()) == 0)
				continue;  // name unchanged, skip

			// Allocate new LVGL name string
			char * name_dm = reinterpret_cast<char*>(lv_mem_alloc(static_cast<uint32_t>(name.size() + 1)));
			if (name_dm == nullptr)
				continue;
			memcpy(name_dm, name.constData(), static_cast<size_t>(name.size() + 1));

			// FIX (C4): free the old name BEFORE overwriting its slot.
			// Old name was allocated by lv_tabview_add_tab() or a prior
			// rename — both use lv_mem_alloc. Without this free, every
			// rename leaks bytes in LVGL's heap.
			lv_mem_free((void *)ext->tab_name_ptr[arrIdx]);
			ext->tab_name_ptr[arrIdx] = name_dm;

			lv_btnm_set_map(ext->btns, ext->tab_name_ptr);
			lv_btnm_set_btn_ctrl(ext->btns, ext->tab_cur, LV_BTNM_CTRL_NO_REPEAT);
		}
```

- [ ] **Step 2: Build and run all tests**

```bash
cd tests/tst_tabview_remove && make && ./tst_tabview_remove.app/Contents/MacOS/tst_tabview_remove 2>&1 | tail -25
```

Expected: `propertyRenameLeft` now PASSES. All prior tests still PASS.

- [ ] **Step 3: Commit**

```bash
git add widgets/LVGLTabview.cpp
git commit -m "fix(C2, C4): Phase 2 rename uses correct index + frees old name

- Uses stride-based array indexing so LEFT/RIGHT tabviews write the
  new name at index k*2 instead of k (which is a \"\\n\" separator).
- Frees the old name via lv_mem_free before overwriting the slot;
  the old code leaked every renamed name string."
```

---

### Task 6: Add failing test for empty-list zombie tab (C3)

**Files:**
- Modify: `tests/tst_tabview_remove/tst_tabview_remove.cpp`

- [ ] **Step 1: Add test**

Append after `propertyRenameLeft()`:

```cpp
  // If the user clears every tab in the dialog, the property setter
  // must not delete the last tab's wrapper while lv_tabview_remove_tab
  // silently no-ops. Previously that left a zombie: LVGL still owned
  // the tab, but no LVGLObject wrapper existed.
  void propertyEmptyListKeepsOneTab() {
    LVGLObject *tvObj = createTabviewWithTabs({"A", "B", "C"});
    lv_tabview_ext_t *ext = reinterpret_cast<lv_tabview_ext_t *>(
        lv_obj_get_ext_attr(tvObj->obj()));

    QCOMPARE(ext->tab_cnt, static_cast<uint16_t>(3));
    QCOMPARE(tvObj->childs().size(), 3);

    // User clears the whole list
    LVGLProperty *tabsProp = lvgl.widget("lv_tabview")->property("Tabs");
    tabsProp->setValue(tvObj, QVariant(QVariantList{}));

    // LVGL requires tab_cnt >= 1. The setter must leave at least one
    // tab with a corresponding LVGLObject wrapper.
    QVERIFY(ext->tab_cnt >= 1);
    QCOMPARE(tvObj->childs().size(), static_cast<int>(ext->tab_cnt));

    // No orphan wrappers
    for (LVGLObject *child : tvObj->childs()) {
      QVERIFY(child->obj() != nullptr);
    }
  }
```

- [ ] **Step 2: Build and verify the test fails or crashes**

```bash
cd tests/tst_tabview_remove && make && ./tst_tabview_remove.app/Contents/MacOS/tst_tabview_remove 2>&1 | tail -25
```

Expected: `propertyEmptyListKeepsOneTab` FAILS (child count 0, but tab_cnt still 1 → mismatch) or crashes.

- [ ] **Step 3: Commit**

```bash
git add tests/tst_tabview_remove/tst_tabview_remove.cpp
git commit -m "test: add failing test for empty-list zombie tab (C3)"
```

---

### Task 7: Fix C3 — guard Phase 1 against removing the last tab

**Files:**
- Modify: `widgets/LVGLTabview.cpp:88-107`

- [ ] **Step 1: Add guard inside the Phase 1 loop**

In `widgets/LVGLTabview.cpp`, find the Phase 1 removal loop (lines 88-107):

```cpp
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
```

Replace with:

```cpp
		// Process removals from HIGHEST index to LOWEST.
		// Why reverse order? When we remove index 2, indices 0 and 1 are
		// unaffected. If we removed index 0 first, all higher indices would
		// shift down and our stored indices would be wrong.
		for (int r = toRemove.size() - 1; r >= 0; --r) {
			int i = toRemove[r];

			// FIX (C3): LVGL requires tab_cnt >= 1 — refuse to remove the
			// last tab. Must check BEFORE deleting the builder wrapper,
			// otherwise lv_tabview_remove_tab silently no-ops but we've
			// already orphaned the LVGL page from its LVGLObject.
			lv_tabview_ext_t *curExt =
				reinterpret_cast<lv_tabview_ext_t*>(lv_obj_get_ext_attr(obj->obj()));
			if (curExt->tab_cnt <= 1)
				break;

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
```

- [ ] **Step 2: Build and run all tests**

```bash
cd tests/tst_tabview_remove && make && ./tst_tabview_remove.app/Contents/MacOS/tst_tabview_remove 2>&1 | tail -25
```

Expected: `propertyEmptyListKeepsOneTab` now PASSES. All prior tests still PASS.

- [ ] **Step 3: Commit**

```bash
git add widgets/LVGLTabview.cpp
git commit -m "fix(C3): refuse to remove the last tab in Phase 1 loop

lv_tabview_remove_tab silently no-ops when tab_cnt <= 1.
Without this check, deleting every tab from the dialog would
delete every builder wrapper but leave one LVGL page orphaned."
```

---

### Task 8: Fix I2 — register `LVGLObject*` meta-type

**Files:**
- Modify: `LVGLCore.cpp:22-24`

- [ ] **Step 1: Register the meta-type**

In `LVGLCore.cpp`, find the constructor (around line 22):

```cpp
LVGLCore::LVGLCore(QObject *parent) : QObject(parent), m_defaultFont(nullptr) {
  FT_Init_FreeType(&m_ft);
}
```

Replace with:

```cpp
LVGLCore::LVGLCore(QObject *parent) : QObject(parent), m_defaultFont(nullptr) {
  // Register LVGLObject* so QSignalSpy and queued-connection use cases
  // can marshal the pointer across threads. Harmless in single-threaded
  // direct-connection code but silences QSignalSpy warnings.
  qRegisterMetaType<LVGLObject*>("LVGLObject*");
  FT_Init_FreeType(&m_ft);
}
```

- [ ] **Step 2: Build and verify warnings are gone**

```bash
cd tests/tst_tabview_remove && make && ./tst_tabview_remove.app/Contents/MacOS/tst_tabview_remove 2>&1 | grep -E "QWARN|unable to handle"
```

Expected: No output (the `QSignalSpy: Unable to handle parameter 'object' of type 'LVGLObject*'` warnings should be gone).

- [ ] **Step 3: Commit**

```bash
git add LVGLCore.cpp
git commit -m "fix(I2): register LVGLObject* meta-type in LVGLCore constructor

Silences QSignalSpy warnings in tests and enables queued-connection
use of the new lifecycle signals."
```

---

### Task 9: Fix M4 — .gitignore for test build artifacts

**Files:**
- Modify: `.gitignore`

- [ ] **Step 1: Add test-specific entries**

In `.gitignore`, replace the file contents with:

```

*.user
.qmake.stash
Makefile
build/
*.app/

# Test build artifacts
tests/**/*.o
tests/**/*.moc
tests/**/moc_*.cpp
tests/**/moc_*.h
tests/**/qrc_*.cpp
tests/**/target_wrapper.sh
tests/**/ui_*.h
tests/**/tst_*[!.]
```

- [ ] **Step 2: Verify untracked artifacts are now ignored**

```bash
git status --short tests/ | grep -v "^\?\? tests/tst_tabview_remove/tst_tabview_remove.cpp" | grep -v "^\?\? tests/tst_tabview_remove/tst_tabview_remove.pro" | grep -v "^\?\? tests/tests.pro" | head -10
```

Expected: no output (all artifacts ignored, only source files show as tracked or previously-tracked).

- [ ] **Step 3: Commit**

```bash
git add .gitignore
git commit -m "chore(M4): ignore test build artifacts"
```

---

### Task 10: Final regression check

**Files:** None — verification only.

- [ ] **Step 1: Clean rebuild and run everything**

```bash
cd tests/tst_tabview_remove && make clean && make && ./tst_tabview_remove.app/Contents/MacOS/tst_tabview_remove 2>&1 | tail -30
```

Expected: All tests PASS. Specifically:
- 14 tests from the original feature (canCreateTabview, removeSingleTab, ...)
- 3 tests from Round 4 (coreEmitsRemoveSignals, coreEmitsAddSignals, survivingPageIndexUpdated)
- 3 new LEFT/RIGHT tests (removeMiddleTabLeft, removeFirstTabRight, propertyGetLeft)
- 1 new rename-left test (propertyRenameLeft)
- 1 new empty-list test (propertyEmptyListKeepsOneTab)
- Total: 21 tests + init/cleanup = 23 PASS

- [ ] **Step 2: Optional manual smoke test on Windows**

```
1. Pull the branch on Windows, rebuild with qmake + mingw/MSVC
2. Drag a TabView onto the canvas
3. Test the default TOP position: add A/B/C, remove B → should work
4. Change "Button position" property to "Left"
5. Test LEFT position: add D/E/F, remove E → should not crash
6. Test empty-list: try to delete every tab → should leave one tab
```

---
