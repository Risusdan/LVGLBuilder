# TabView Remove Tab — Design Spec

**Date:** 2026-04-14
**Goal:** Add `lv_tabview_remove_tab()` to the vendored LVGL 6.1 library and wire it into LVGLBuilder's property editor so users can delete specific tabs from TabView widgets via the existing delete button in the text-list dialog.

---

## Architecture

Two layers of work:

1. **LVGL C layer** — A new function `lv_tabview_remove_tab(tabview, id)` in the vendored LVGL that deletes a tab's page object, frees its button-map name string, shrinks the name-pointer array, adjusts `tab_cnt`/`tab_cur`, and re-syncs the button matrix and indicator.

2. **Builder C++/Qt layer** — The existing `LVGLPropertyTabs::set()` gains a removal code-path using a by-name matching algorithm that identifies which specific tabs were removed (rather than trimming from the end). This requires a `detachLvObj` mechanism on `LVGLObject` to prevent double-deletion of LVGL objects.

---

## Component 1: `lv_tabview_remove_tab()`

**Location:** `lvgl/lvgl/src/lv_objx/lv_tabview.c` (must live here — needs access to static `tabview_realign()`). Declaration in `lv_tabview.h`.

**Signature:** `void lv_tabview_remove_tab(lv_obj_t * tabview, uint16_t id)`

**Behavior:**

1. **Guard:** Return immediately if `id >= tab_cnt` or `tab_cnt <= 1` (refuse to remove the last tab).
2. **Delete page:** Find the `id`-th child of `ext->content` via `lv_obj_get_child_back()` traversal, call `lv_obj_del()`.
3. **Free name string:** `lv_mem_free(ext->tab_name_ptr[id])`.
4. **Decrement** `ext->tab_cnt`.
5. **Shift name-pointer array** and realloc smaller. Two formats:
   - **TOP/BOTTOM:** `[name0, name1, ..., ""]` — shift entries left, terminate with `""`, realloc to `(tab_cnt + 1) * sizeof(char*)`.
   - **LEFT/RIGHT:** `[name0, "\n", name1, "\n", ..., ""]` — remove the name and its adjacent `"\n"` separator, realloc to `tab_cnt * 2 * sizeof(char*)`.
6. **Rebuild button matrix:** Invalidate `btnm_ext->map_p = NULL`, then `lv_btnm_set_map(ext->btns, ext->tab_name_ptr)`.
7. **Adjust `ext->tab_cur`:** Clamp to `tab_cnt - 1` if beyond; decrement if after the removed index.
8. **Re-layout:** `tabview_realign(tabview)` + `lv_tabview_set_tab_act(tabview, ext->tab_cur, LV_ANIM_OFF)`.

---

## Component 2: LVGLObject — preventing double-delete

**Problem:** `lv_tabview_remove_tab()` calls `lv_obj_del(page)` internally, deleting the LVGL page and all its LVGL children. But the builder wraps each page (and any widgets placed on it) in `LVGLObject`, whose destructor also calls `lv_obj_del(m_obj)`. Both firing causes a crash.

**Solution — additions to `LVGLObject`:**

- `void detachLvObj()` — sets `m_obj = nullptr`.
- `void detachLvObjRecursive()` — calls `detachLvObj()` on self and all descendant `LVGLObject`s recursively. Needed because a tab page can have child widgets, and `lv_obj_del(page)` recursively deletes all LVGL children.
- **Destructor guard** — `if (!m_obj) return;` at the top of `~LVGLObject()`.

**Removal sequence in the property editor:**

```
page->detachLvObjRecursive();   // prevent all wrappers from calling lv_obj_del
lvgl.removeObject(page);         // clean up wrapper tree (children, parent refs, m_objects, delete)
lv_tabview_remove_tab(tv, id);   // LVGL handles the actual lv_obj deletion
```

`lvgl.removeObject()` already handles recursive child cleanup, removal from parent's child list, removal from `m_objects`, and `delete`. With detached `m_obj`s, all destructors are safe no-ops.

---

## Component 3: `LVGLPropertyTabs::set()` — by-name matching algorithm

**Current state** (`widgets/LVGLTabview.cpp:49-76`): Handles rename (positional) and add (append). No removal path — when the dialog returns a shorter list, removed tabs are silently ignored.

**Constraint:** LVGL's `lv_tabview_add_tab()` only appends — no insert-at-position. So we can remove any tab by index but can only add at the end.

**New three-phase algorithm:**

### Phase 1 — Name-based removal (highest index first)

Build a `name → count` map from the new list. Walk the old tab names: if a name has remaining count > 0, it survives (decrement count); otherwise mark for removal.

Process removals from highest index to lowest so indices stay valid:

```
for each removal index i (descending):
    page = obj->findChildByIndex(i)
    page->detachLvObjRecursive()
    lvgl.removeObject(page)
    lv_tabview_remove_tab(tv, i)
```

### Phase 2 — Positional rename

After removals, compare each surviving tab's name with `newNames[i]`. Rename any that differ (handles in-place text edits in the dialog).

### Phase 3 — Append new tabs

For indices beyond `ext->tab_cnt`, call `lv_tabview_add_tab()` and create `LVGLObject` wrappers.

### Scenario coverage

| Old | New | Phase 1 (remove) | Phase 2 (rename) | Phase 3 (add) | Result |
|-----|-----|-------------------|-------------------|---------------|--------|
| `[A,B,C]` | `[A,C]` | Remove B (idx 1) | A=A, C=C — none | — | `[A,C]` — C preserved |
| `[A,B,C]` | `[A,C,D]` | Remove B (idx 1) | A=A, C=C — none | Add D | `[A,C,D]` |
| `[A,B,C]` | `[A,X,C]` | Remove B (idx 1) | A=A, C≠X rename | Add C | `[A,X,C]` |
| `[A,B]` | `[A,B,C]` | None | None | Add C | `[A,B,C]` |

---

## File Map

| Action | File | What changes |
|--------|------|-------------|
| Modify | `lvgl/lvgl/src/lv_objx/lv_tabview.h` | Declare `lv_tabview_remove_tab()` |
| Modify | `lvgl/lvgl/src/lv_objx/lv_tabview.c` | Implement `lv_tabview_remove_tab()` |
| Modify | `LVGLObject.h` | Add `detachLvObj()`, `detachLvObjRecursive()` |
| Modify | `LVGLObject.cpp` | Guard destructor, implement `detachLvObjRecursive()` |
| Modify | `widgets/LVGLTabview.cpp` | Rewrite `LVGLPropertyTabs::set()` with three-phase algorithm |
| Create | `tests/tests.pro` | Subdirs project for test suite |
| Create | `tests/tst_tabview_remove/tst_tabview_remove.pro` | Test project (QT += testlib, CONFIG += testcase) |
| Create | `tests/tst_tabview_remove/tst_tabview_remove.cpp` | QTest covering all three components |

**Not modified:** `LVGLBuilder.pro` (existing build untouched), `properties/LVGLPropertyTextList.cpp` (dialog already has the delete button).

---

## TDD Plan

Separate test project at `tests/tests.pro` — opened as a second project in Qt Creator. Test Results pane auto-discovers QTest classes.

### Round 1: LVGL-level removal

Write failing tests first (compile error — function doesn't exist), then implement.

| Test | Setup | Action | Assertions |
|------|-------|--------|------------|
| `removeSingleTab` | `[A,B,C]` | remove idx 1 | count=2, names=[A,C], cur=0 |
| `removeFirstTab` | `[X,Y]` | remove idx 0 | count=1, names=[Y], cur=0 |
| `removeLastTabActive` | `[P,Q,R]` cur=2 | remove idx 2 | count=2, cur clamped to 1 |
| `removeIgnoredWhenOnlyOneTab` | `[Solo]` | remove idx 0 | count=1, no-op |
| `removeOutOfBoundsIgnored` | `[A,B]` | remove idx 99 | count=2, no-op |

### Round 2: LVGLObject detach

| Test | Assertions |
|------|------------|
| `detachPreventsDoubleDelete` | detach + delete wrapper doesn't crash |
| `detachRecursive` | page and all descendant wrappers have `obj() == nullptr` |

### Round 3: Builder property integration

| Test | Setup | Action | Assertions |
|------|-------|--------|------------|
| `propertyRemovesByName` | `[A,B,C]` | set `[A,C]` | tab_cnt=2, childs=2, names=[A,C] |
| `propertyRemoveAndAdd` | `[A,B,C]` | set `[A,C,D]` | tab_cnt=3, names=[A,C,D] |
| `propertyRenameOnly` | `[A,B]` | set `[A,X]` | tab_cnt=2, names=[A,X] |
| `propertyNoChange` | `[A,B]` | set `[A,B]` | tab_cnt=2, no-op |
