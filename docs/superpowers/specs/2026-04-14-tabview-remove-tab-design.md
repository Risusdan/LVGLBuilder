# TabView Remove Tab — Design Spec

**Date:** 2026-04-14
**Goal:** Add `lv_tabview_remove_tab()` to the vendored LVGL 6.1 library and wire it into LVGLBuilder's property editor so users can delete specific tabs from TabView widgets via the existing delete button in the text-list dialog.

---

## Architecture

Two layers of work:

1. **LVGL C layer** — A new function `lv_tabview_remove_tab(tabview, id)` in the vendored LVGL that deletes a tab's page object, frees its button-map name string, shrinks the name-pointer array, adjusts `tab_cnt`/`tab_cur`, and re-syncs the button matrix and indicator.

2. **Builder C++/Qt layer** — The existing `LVGLPropertyTabs::set()` gains a removal code-path using a by-name matching algorithm that identifies which specific tabs were removed (rather than trimming from the end). This requires a `detachLvObj` mechanism on `LVGLObject` to prevent double-deletion of LVGL objects.

### End-to-end workflow

```
User opens TabView's "Tabs" property dialog (LVGLPropertyTextListDialog)
  → sees current tab names in a QListWidget (e.g. [A, B, C])
  → deletes "B" using the trash-can button, presses OK
  → dialog returns new list [A, C]

LVGLPropertyTabs::set() receives oldNames=[A,B,C], newNames=[A,C]
  → Phase 1: by-name matching detects "B" was removed
  → Phase 2: surviving tabs [A,C] match positions — no renames
  → Phase 3: no additions needed
  → Result: LVGL tabview now has [A, C], C's page content is preserved
```

---

## Component 1: `lv_tabview_remove_tab()`

**Location:** `lvgl/lvgl/src/lv_objx/lv_tabview.c` (must live here — needs access to static `tabview_realign()`). Declaration in `lv_tabview.h`.

**Signature:** `void lv_tabview_remove_tab(lv_obj_t * tabview, uint16_t id)`

**Behavior — annotated pseudocode:**

```c
void lv_tabview_remove_tab(lv_obj_t * tabview, uint16_t id)
{
    lv_tabview_ext_t * ext = lv_obj_get_ext_attr(tabview);

    /* --- Guard: refuse invalid or degenerate removals ---
     * A tabview with 0 or 1 tab cannot lose its last tab (LVGL has no
     * "empty tabview" state). Out-of-bounds ids are silently ignored. */
    if(ext->tab_cnt <= 1) return;
    if(id >= ext->tab_cnt) return;

    /* --- Step 1: Delete the LVGL page object ---
     * Tab pages live as children of ext->content (a container with
     * LV_LAYOUT_ROW_T). lv_obj_get_child_back() iterates oldest-first,
     * so counting forward `id` times reaches the correct page.
     * lv_obj_del() recursively frees the page and all widgets on it. */
    lv_obj_t * page = lv_obj_get_child_back(ext->content, NULL);
    for(uint16_t i = 0; i < id && page; i++)
        page = lv_obj_get_child_back(ext->content, page);
    if(page) lv_obj_del(page);

    /* --- Step 2: Free the dynamically-allocated name string ---
     * lv_tabview_add_tab() allocates each name via lv_mem_alloc().
     * We must free it before shifting the array. */
    lv_mem_free(ext->tab_name_ptr[id]);

    /* --- Step 3: Shrink tab_cnt, then compact the name-pointer array ---
     *
     * The array format depends on button position:
     *
     * TOP/BOTTOM: [name0, name1, ..., nameN-1, ""]
     *   → (tab_cnt + 1) pointers. The "" sentinel terminates the btnm map.
     *   → Shift entries left starting at `id`, re-terminate with "".
     *
     * LEFT/RIGHT: [name0, "\n", name1, "\n", ..., nameN-1, ""]
     *   → (tab_cnt * 2) pointers. "\n" literals (not allocated) separate rows.
     *   → First tab: remove [name0, "\n"]. Others: remove ["\n", nameN].
     *   → Special case: if only 1 tab remains, array is just [name, ""].
     */
    ext->tab_cnt--;
    // ... shift + realloc per format (see implementation) ...

    /* --- Step 4: Rebuild the button matrix ---
     * The old btnm map pointer may have been freed by lv_mem_realloc,
     * so we must invalidate it before setting the new map. */
    lv_btnm_ext_t * btnm_ext = lv_obj_get_ext_attr(ext->btns);
    btnm_ext->map_p = NULL;
    lv_btnm_set_map(ext->btns, ext->tab_name_ptr);

    /* --- Step 5: Adjust active tab index ---
     * Three cases:
     *   tab_cur > id  → decrement (tabs above shifted down)
     *   tab_cur == id → the active tab was removed; now points to
     *                    the next tab (same index) or clamps if at end
     *   tab_cur < id  → unchanged
     * In all cases, clamp to tab_cnt - 1. */
    if(ext->tab_cur >= ext->tab_cnt)
        ext->tab_cur = ext->tab_cnt - 1;
    else if(ext->tab_cur > id)
        ext->tab_cur--;

    /* --- Step 6: Re-layout ---
     * tabview_realign() resizes all page objects, repositions buttons
     * and indicator. lv_tabview_set_tab_act() scrolls content to the
     * (possibly new) active tab and updates the toggle highlight. */
    tabview_realign(tabview);
    lv_tabview_set_tab_act(tabview, ext->tab_cur, LV_ANIM_OFF);
}
```

---

## Component 2: LVGLObject — preventing double-delete

**Problem:** `lv_tabview_remove_tab()` calls `lv_obj_del(page)` internally, deleting the LVGL page and all its LVGL children. But the builder wraps each page (and any widgets placed on it) in `LVGLObject`, whose destructor also calls `lv_obj_del(m_obj)`. Both firing causes a crash.

**Solution — additions to `LVGLObject`:**

- `void detachLvObj()` — sets `m_obj = nullptr`.
- `void detachLvObjRecursive()` — calls `detachLvObj()` on self and all descendant `LVGLObject`s recursively. Needed because a tab page can have child widgets, and `lv_obj_del(page)` recursively deletes all LVGL children.
- **Destructor guard** — `if (!m_obj) return;` at the top of `~LVGLObject()`.

**Removal sequence — annotated with WHY each step's order matters:**

```cpp
// === Removing a tab page from the builder ===
//
// There are TWO parallel object trees that must stay in sync:
//
//   LVGL tree:    lv_obj_t (tabview) → lv_obj_t (page) → lv_obj_t (child widgets)
//   Builder tree: LVGLObject (tabview) → LVGLObject (page) → LVGLObject (child widgets)
//
// lv_tabview_remove_tab() deletes the LVGL page + all its LVGL children.
// lvgl.removeObject() deletes the LVGLObject page + all its LVGLObject children.
// If both run independently, each LVGLObject destructor calls lv_obj_del()
// on an already-deleted lv_obj_t → crash (double-free).
//
// Solution: detach first, so destructors become no-ops.

// Step 1: Detach ALL wrappers in the subtree.
// After this, every LVGLObject under `page` has m_obj == nullptr,
// so their destructors won't call lv_obj_del().
page->detachLvObjRecursive();

// Step 2: Clean up the builder-side tree.
// lvgl.removeObject() recursively:
//   - removes each child LVGLObject from m_objects list
//   - removes page from parent's child list
//   - deletes all LVGLObject wrappers (destructors are safe — m_obj is null)
// After this, the builder tree is clean. The LVGL tree is still intact.
lvgl.removeObject(page);

// Step 3: Clean up the LVGL-side tree.
// lv_tabview_remove_tab() calls lv_obj_del(page) which frees the LVGL
// page object and all its LVGL children, then rebuilds the button matrix
// and re-layouts the tabview.
lv_tabview_remove_tab(tv, id);
```

---

## Component 3: `LVGLPropertyTabs::set()` — by-name matching algorithm

**Current state** (`widgets/LVGLTabview.cpp:49-76`): Handles rename (positional) and add (append). No removal path — when the dialog returns a shorter list, removed tabs are silently ignored.

**Constraint:** LVGL's `lv_tabview_add_tab()` only appends — no insert-at-position. So we can remove any tab by index but can only add at the end.

**New three-phase algorithm — annotated pseudocode:**

```cpp
void LVGLPropertyTabs::set(LVGLObject *obj, QStringList newNames) {
    lv_tabview_ext_t *ext = lv_obj_get_ext_attr(obj->obj());

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

    // Build name→count map from newNames
    QMap<QString, int> countMap;
    for (const QString &name : newNames)
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
        LVGLObject *page = obj->findChildByIndex(i);
        if (page) {
            page->detachLvObjRecursive();  // prevent double-delete
            lvgl.removeObject(page);       // clean up builder wrappers
        }
        lv_tabview_remove_tab(obj->obj(), static_cast<uint16_t>(i));
    }

    // ================================================================
    // PHASE 2: Positional rename — handle in-place text edits
    // ================================================================
    //
    // After removals, the surviving LVGL tabs are at indices 0..tab_cnt-1.
    // Compare each with newNames[i]. If they differ, the user edited the
    // text in the dialog → rename in place.
    //
    // Example: old=[A, B, C] → after removing B → LVGL has [A, C]
    //          new=[A, C] → A=A ok, C=C ok → no renames needed.
    //
    // Example: old=[A, B] → no removals → LVGL has [A, B]
    //          new=[A, X] → A=A ok, B≠X → rename B to X.

    ext = lv_obj_get_ext_attr(obj->obj());  // re-read (tab_cnt changed)
    for (uint16_t i = 0; i < qMin(ext->tab_cnt, (uint16_t)newNames.size()); ++i) {
        QByteArray name = newNames.at(i).toLatin1();
        if (strcmp(ext->tab_name_ptr[i], name.data()) == 0)
            continue;  // name unchanged, skip

        // Allocate new name string and replace in the array
        char *name_dm = (char *)lv_mem_alloc(name.size() + 1);
        memcpy(name_dm, name.constData(), name.size() + 1);
        ext->tab_name_ptr[i] = name_dm;
        lv_btnm_set_map(ext->btns, ext->tab_name_ptr);
    }

    // ================================================================
    // PHASE 3: Append new tabs — handle additions
    // ================================================================
    //
    // Any newNames entries beyond the current tab_cnt are brand-new tabs.
    // LVGL only supports appending (no insert-at-position).
    //
    // Example: old=[A, C] (after removal), new=[A, C, D]
    //          → tab_cnt=2, newNames.size()=3 → add "D" at the end.

    for (int i = ext->tab_cnt; i < newNames.size(); ++i) {
        lv_obj_t *page_obj = lv_tabview_add_tab(obj->obj(), qPrintable(newNames.at(i)));
        LVGLObject *page = new LVGLObject(page_obj, lvgl.widget("lv_page"), obj, false, i);
        lvgl.addObject(page);
    }
}
```

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
