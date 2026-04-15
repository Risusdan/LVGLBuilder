# Fix TabView Remove Tab Crash — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Fix the crash/hang that occurs when removing a tab via the UI. The root cause is that `LVGLPropertyTabs::set()` calls `lvgl.removeObject()`/`addObject()` directly, bypassing the `LVGLObjectModel` notifications that the object tree QTreeView requires.

**Architecture:** Move model notifications from `LVGLSimulator` into `LVGLCore` via Qt signals. When `LVGLCore::addObject()` or `removeObject()` is called — regardless of who calls it — the connected `LVGLObjectModel` is automatically notified with `beginInsertRows/endInsertRows` or `beginRemoveRows/endRemoveRows`. Also fix stale `m_index` on surviving tab page wrappers after removal.

**Tech Stack:** C++/Qt 6 (signals/slots, QAbstractItemModel), QTest

---

## File Map

| Action | File | Responsibility |
|--------|------|----------------|
| Modify | `LVGLCore.h:61-62` | Add 4 signals for object lifecycle |
| Modify | `LVGLCore.cpp:495-507` | Emit signals in `addObject()` and `removeObject()` |
| Modify | `LVGLSimulator.cpp:334-356` | Remove manual model notifications (now handled by signals) |
| Modify | `MainWindow.cpp:61-69` | Connect LVGLCore signals to LVGLObjectModel |
| Modify | `LVGLObject.h:79` | Add `setIndex()` method |
| Modify | `widgets/LVGLTabview.cpp:88-107` | Update surviving pages' m_index after removal |
| Modify | `tests/tst_tabview_remove/tst_tabview_remove.cpp` | Add tests for signal-based notifications and index fix |

---

### Task 1: Add failing test for object model notification

**Files:**
- Modify: `tests/tst_tabview_remove/tst_tabview_remove.cpp`

This test verifies that `lvgl.removeObject()` emits signals that the model can connect to. It also verifies that surviving pages have corrected `m_index` values.

- [ ] **Step 1: Add test methods**

Insert these after `propertyNoChange()` and before the closing `};`:

```cpp
  // =================================================================
  // Round 4: Crash-fix tests
  // Verify that LVGLCore emits signals on addObject/removeObject,
  // and that surviving pages get corrected m_index values.
  // =================================================================

  // Verify LVGLCore emits aboutToRemoveObject/objectRemoved signals
  // when removeObject() is called. This is the fix for the crash:
  // without these signals, the QTreeView's LVGLObjectModel isn't
  // notified of removals → stale model indices → use-after-free.
  void coreEmitsRemoveSignals() {
    lv_obj_t *screen = lv_scr_act();
    const LVGLWidget *labelWidget = lvgl.widget("lv_label");
    QVERIFY(labelWidget != nullptr);
    LVGLObject *obj = new LVGLObject(labelWidget, "signal_test", screen);
    lvgl.addObject(obj);

    QSignalSpy aboutToRemove(&lvgl, SIGNAL(aboutToRemoveObject(LVGLObject*)));
    QSignalSpy removed(&lvgl, SIGNAL(objectRemoved()));

    lvgl.removeObject(obj);

    QCOMPARE(aboutToRemove.count(), 1);
    QCOMPARE(removed.count(), 1);
  }

  // Verify LVGLCore emits aboutToAddObject/objectAdded signals
  void coreEmitsAddSignals() {
    lv_obj_t *screen = lv_scr_act();
    const LVGLWidget *labelWidget = lvgl.widget("lv_label");
    QVERIFY(labelWidget != nullptr);

    QSignalSpy aboutToAdd(&lvgl, SIGNAL(aboutToAddObject(LVGLObject*)));
    QSignalSpy added(&lvgl, SIGNAL(objectAdded()));

    LVGLObject *obj = new LVGLObject(labelWidget, "signal_test", screen);
    lvgl.addObject(obj);

    QCOMPARE(aboutToAdd.count(), 1);
    QCOMPARE(added.count(), 1);
  }

  // Verify that after removing tab B from [A, B, C],
  // page C's m_index is updated from 2 to 1.
  void survivingPageIndexUpdated() {
    LVGLObject *tvObj = createTabviewWithTabs({"A", "B", "C"});
    QCOMPARE(tvObj->childs().size(), 3);

    // Before removal: indices are 0, 1, 2
    QCOMPARE(tvObj->childs().at(0)->index(), 0);
    QCOMPARE(tvObj->childs().at(1)->index(), 1);
    QCOMPARE(tvObj->childs().at(2)->index(), 2);

    LVGLProperty *tabsProp = lvgl.widget("lv_tabview")->property("Tabs");
    tabsProp->setValue(tvObj, QVariant(QVariantList{{"A"}, {"C"}}));

    // After removal of B: surviving pages should be re-indexed 0, 1
    QCOMPARE(tvObj->childs().size(), 2);
    QCOMPARE(tvObj->childs().at(0)->index(), 0);
    QCOMPARE(tvObj->childs().at(1)->index(), 1);
  }
```

- [ ] **Step 2: Build and verify the tests fail**

```bash
cd tests/tst_tabview_remove && make && ./tst_tabview_remove.app/Contents/MacOS/tst_tabview_remove
```

Expected: `coreEmitsRemoveSignals` FAILS (signal count is 0 — signals don't exist yet). `coreEmitsAddSignals` FAILS (same). `survivingPageIndexUpdated` FAILS (index stays 2 instead of 1).

- [ ] **Step 3: Commit**

```bash
git add tests/tst_tabview_remove/tst_tabview_remove.cpp
git commit -m "test: add failing tests for object model signals and index fix"
```

---

### Task 2: Add lifecycle signals to LVGLCore

**Files:**
- Modify: `LVGLCore.h:104` (before `private slots:`)
- Modify: `LVGLCore.cpp:495-507`

- [ ] **Step 1: Add signals to LVGLCore.h**

In `LVGLCore.h`, insert after line 103 (`static const char *DEFAULT_MONTHS[12];`) and before line 105 (`private slots:`):

```cpp
 signals:
  // Emitted by addObject() / removeObject() so that any connected model
  // (e.g. LVGLObjectModel) can call beginInsertRows / beginRemoveRows.
  // This ensures the object tree QTreeView stays in sync regardless of
  // who adds/removes objects (simulator, property editor, JSON loader).
  void aboutToAddObject(LVGLObject *object);
  void objectAdded();
  void aboutToRemoveObject(LVGLObject *object);
  void objectRemoved();

```

- [ ] **Step 2: Emit signals in LVGLCore::addObject()**

In `LVGLCore.cpp`, replace line 495:

```cpp
void LVGLCore::addObject(LVGLObject *object) { m_objects << object; }
```

with:

```cpp
void LVGLCore::addObject(LVGLObject *object) {
  emit aboutToAddObject(object);
  m_objects << object;
  emit objectAdded();
}
```

- [ ] **Step 3: Emit signals in LVGLCore::removeObject()**

In `LVGLCore.cpp`, replace lines 497-507:

```cpp
void LVGLCore::removeObject(LVGLObject *object) {
  auto childs = object->childs();
  while (!childs.isEmpty()) {
    removeObject(childs.at(0));
    childs = object->childs();
  }
  if (object->parent()) object->parent()->removeChild(object);
  m_objects.removeOne(object);
  delete object;
  object = nullptr;
}
```

with:

```cpp
void LVGLCore::removeObject(LVGLObject *object) {
  // Recursively remove children first (depth-first).
  // Each recursive call emits its own signals, so the model
  // gets individual begin/end notifications per child.
  auto childs = object->childs();
  while (!childs.isEmpty()) {
    removeObject(childs.at(0));
    childs = object->childs();
  }
  // Signal BEFORE removal — model reads the object's parent/row
  // while the tree is still intact.
  emit aboutToRemoveObject(object);
  if (object->parent()) object->parent()->removeChild(object);
  m_objects.removeOne(object);
  delete object;
  // Signal AFTER removal — model commits the row removal.
  emit objectRemoved();
}
```

- [ ] **Step 4: Build and run the signal tests**

```bash
cd tests/tst_tabview_remove && make && ./tst_tabview_remove.app/Contents/MacOS/tst_tabview_remove -v2 2>&1 | grep -E "PASS|FAIL|coreEmits"
```

Expected: `coreEmitsRemoveSignals` PASSES, `coreEmitsAddSignals` PASSES. `survivingPageIndexUpdated` still FAILS (index fix not done yet).

- [ ] **Step 5: Commit**

```bash
git add LVGLCore.h LVGLCore.cpp
git commit -m "feat: add lifecycle signals to LVGLCore::addObject/removeObject"
```

---

### Task 3: Connect signals in MainWindow and remove manual notifications from LVGLSimulator

**Files:**
- Modify: `MainWindow.cpp:61-69`
- Modify: `LVGLSimulator.cpp:334-356`

- [ ] **Step 1: Connect signals in MainWindow.cpp**

In `MainWindow.cpp`, after line 69 (`m_simulator->setObjectModel(m_objectModel);`), add:

```cpp
  // LVGLCore signals → LVGLObjectModel notifications.
  // This ensures the object tree QTreeView stays in sync whenever
  // objects are added/removed, regardless of who does it (simulator,
  // property editor, JSON loader, etc.).
  connect(&lvgl, &LVGLCore::aboutToAddObject,
          m_objectModel, &LVGLObjectModel::beginInsertObject);
  connect(&lvgl, &LVGLCore::objectAdded,
          m_objectModel, &LVGLObjectModel::endInsertObject);
  connect(&lvgl, &LVGLCore::aboutToRemoveObject,
          m_objectModel, &LVGLObjectModel::beginRemoveObject);
  connect(&lvgl, &LVGLCore::objectRemoved,
          m_objectModel, &LVGLObjectModel::endRemoveObject);
```

- [ ] **Step 2: Remove manual model notifications from LVGLSimulator::addObject()**

In `LVGLSimulator.cpp`, replace lines 334-349:

```cpp
void LVGLSimulator::addObject(LVGLObject *obj) {
  connect(obj, &LVGLObject::positionChanged, m_item, &LVGLItem::updateGeometry);

  // add to object viewer
  if (m_objectModel) m_objectModel->beginInsertObject(obj);

  // add object to interal list
  lvgl.addObject(obj);

  if (m_objectModel) m_objectModel->endInsertObject();

  setSelectedObject(obj);
  setFocus();

  emit objectAdded(obj);
}
```

with:

```cpp
void LVGLSimulator::addObject(LVGLObject *obj) {
  connect(obj, &LVGLObject::positionChanged, m_item, &LVGLItem::updateGeometry);

  // Model notification is handled by LVGLCore::addObject() signals,
  // connected to LVGLObjectModel in MainWindow.
  lvgl.addObject(obj);

  setSelectedObject(obj);
  setFocus();

  emit objectAdded(obj);
}
```

- [ ] **Step 3: Remove manual model notifications from LVGLSimulator::removeObject()**

In `LVGLSimulator.cpp`, replace lines 351-356:

```cpp
void LVGLSimulator::removeObject(LVGLObject *obj) {
  setSelectedObject(nullptr);
  if (m_objectModel) m_objectModel->beginRemoveObject(obj);
  lvgl.removeObject(obj);
  if (m_objectModel) m_objectModel->endRemoveObject();
}
```

with:

```cpp
void LVGLSimulator::removeObject(LVGLObject *obj) {
  setSelectedObject(nullptr);
  // Model notification is handled by LVGLCore::removeObject() signals,
  // connected to LVGLObjectModel in MainWindow.
  lvgl.removeObject(obj);
}
```

- [ ] **Step 4: Build and run all tests**

```bash
cd tests/tst_tabview_remove && make && ./tst_tabview_remove.app/Contents/MacOS/tst_tabview_remove
```

Expected: All 14 existing tests + 2 signal tests PASS. `survivingPageIndexUpdated` still FAILS.

- [ ] **Step 5: Commit**

```bash
git add MainWindow.cpp LVGLSimulator.cpp
git commit -m "fix: connect LVGLCore lifecycle signals to object model

Moves model notifications from LVGLSimulator into LVGLCore signals,
so the object tree QTreeView stays in sync when objects are
added/removed from any call site (property editor, simulator, etc.).
This fixes the crash when removing tabs via the UI."
```

---

### Task 4: Fix stale m_index on surviving tab pages

**Files:**
- Modify: `LVGLObject.h:79`
- Modify: `widgets/LVGLTabview.cpp:88-107`

After removing a tab, the surviving page `LVGLObject`s keep their original `m_index` values. For example, removing index 1 from `[A(0), B(1), C(2)]` leaves C with `m_index=2` even though it's now at LVGL position 1. This breaks `findChildByIndex()` for subsequent operations.

- [ ] **Step 1: Add `setIndex()` to LVGLObject.h**

In `LVGLObject.h`, after `int index() const;` (line 79), add:

```cpp
	void setIndex(int index);
```

And in `LVGLObject.cpp`, after the `index()` getter (after line 751 `return m_index;`), add:

```cpp
void LVGLObject::setIndex(int index)
{
	m_index = index;
}
```

- [ ] **Step 2: Re-index surviving pages after Phase 1 removals**

In `widgets/LVGLTabview.cpp`, after the Phase 1 removal loop (after line 107 `}`), add:

```cpp
		// After removals, re-index surviving page wrappers.
		// Example: removing index 1 from [A(0), B(1), C(2)] leaves
		// C with m_index=2, but it's now at LVGL position 1.
		// Without this fix, findChildByIndex() fails for subsequent
		// operations (widget drop, save/load, further tab edits).
		{
			int idx = 0;
			for (LVGLObject *child : obj->childs()) {
				child->setIndex(idx++);
			}
		}
```

- [ ] **Step 3: Build and run all tests**

```bash
cd tests/tst_tabview_remove && make && ./tst_tabview_remove.app/Contents/MacOS/tst_tabview_remove
```

Expected: ALL 17 tests PASS, including `survivingPageIndexUpdated`.

- [ ] **Step 4: Commit**

```bash
git add LVGLObject.h LVGLObject.cpp widgets/LVGLTabview.cpp
git commit -m "fix: re-index surviving tab page wrappers after removal"
```

---

### Task 5: Build the main app and run full regression check

**Files:** None modified — verification only.

- [ ] **Step 1: Run all tests**

```bash
cd tests/tst_tabview_remove && make clean && make && ./tst_tabview_remove.app/Contents/MacOS/tst_tabview_remove
```

Expected: All 17 tests PASS.

- [ ] **Step 2: Optional manual smoke test**

```
1. Launch ./LVGLBuilder (or build on Windows and test there)
2. Drag a TabView onto the canvas
3. Select TabView, open "Tabs" property, add "A", "B", "C" → OK
4. Verify object tree shows the tabview with 3 page children
5. Re-open "Tabs", select "B", click trash, press OK
6. Verify: no crash, "B" tab disappears, object tree shows 2 children
7. Re-open "Tabs", verify list shows ["A", "C"]
8. Drop a label widget onto the "C" tab page — should work (m_index is correct)
```

---
