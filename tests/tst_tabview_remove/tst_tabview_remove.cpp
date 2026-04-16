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

  // =================================================================
  // Round 1: LVGL-level removal tests
  // These test lv_tabview_remove_tab() directly on the LVGL C layer.
  // =================================================================

  // Remove a middle tab from a 3-tab tabview.
  void removeSingleTab() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *tv = lv_tabview_create(screen, nullptr);
    lv_tabview_add_tab(tv, "A");
    lv_tabview_add_tab(tv, "B");
    lv_tabview_add_tab(tv, "C");

    QCOMPARE(lv_tabview_get_tab_count(tv), static_cast<uint16_t>(3));

    lv_tabview_remove_tab(tv, 1);  // remove "B"

    QCOMPARE(lv_tabview_get_tab_count(tv), static_cast<uint16_t>(2));

    lv_tabview_ext_t *ext = reinterpret_cast<lv_tabview_ext_t *>(
        lv_obj_get_ext_attr(tv));
    QCOMPARE(QString(ext->tab_name_ptr[0]), QString("A"));
    QCOMPARE(QString(ext->tab_name_ptr[1]), QString("C"));

    QCOMPARE(lv_tabview_get_tab_act(tv), static_cast<uint16_t>(0));

    lv_obj_del(tv);
  }

  // Remove the first tab (index 0).
  void removeFirstTab() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *tv = lv_tabview_create(screen, nullptr);
    lv_tabview_add_tab(tv, "X");
    lv_tabview_add_tab(tv, "Y");

    lv_tabview_remove_tab(tv, 0);

    QCOMPARE(lv_tabview_get_tab_count(tv), static_cast<uint16_t>(1));
    lv_tabview_ext_t *ext = reinterpret_cast<lv_tabview_ext_t *>(
        lv_obj_get_ext_attr(tv));
    QCOMPARE(QString(ext->tab_name_ptr[0]), QString("Y"));
    QCOMPARE(lv_tabview_get_tab_act(tv), static_cast<uint16_t>(0));

    lv_obj_del(tv);
  }

  // Remove the last tab while it is the active tab.
  void removeLastTabActive() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *tv = lv_tabview_create(screen, nullptr);
    lv_tabview_add_tab(tv, "P");
    lv_tabview_add_tab(tv, "Q");
    lv_tabview_add_tab(tv, "R");

    lv_tabview_set_tab_act(tv, 2, LV_ANIM_OFF);
    lv_tabview_remove_tab(tv, 2);

    QCOMPARE(lv_tabview_get_tab_count(tv), static_cast<uint16_t>(2));
    QCOMPARE(lv_tabview_get_tab_act(tv), static_cast<uint16_t>(1));

    lv_obj_del(tv);
  }

  // Attempt to remove when only 1 tab — should be no-op.
  void removeIgnoredWhenOnlyOneTab() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *tv = lv_tabview_create(screen, nullptr);
    lv_tabview_add_tab(tv, "Solo");

    lv_tabview_remove_tab(tv, 0);

    QCOMPARE(lv_tabview_get_tab_count(tv), static_cast<uint16_t>(1));
    lv_tabview_ext_t *ext = reinterpret_cast<lv_tabview_ext_t *>(
        lv_obj_get_ext_attr(tv));
    QCOMPARE(QString(ext->tab_name_ptr[0]), QString("Solo"));

    lv_obj_del(tv);
  }

  // Attempt to remove at out-of-bounds index — should be no-op.
  void removeOutOfBoundsIgnored() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *tv = lv_tabview_create(screen, nullptr);
    lv_tabview_add_tab(tv, "A");
    lv_tabview_add_tab(tv, "B");

    lv_tabview_remove_tab(tv, 99);

    QCOMPARE(lv_tabview_get_tab_count(tv), static_cast<uint16_t>(2));

    lv_obj_del(tv);
  }

  // =================================================================
  // Round 2: LVGLObject detach tests
  // These test the double-delete prevention mechanism.
  //
  // Background: lv_tabview_remove_tab() calls lv_obj_del(page), which
  // deletes the LVGL page. But ~LVGLObject() also calls lv_obj_del(m_obj).
  // If both run, the app crashes (double-free). detachLvObj() sets
  // m_obj = nullptr so the destructor becomes a safe no-op.
  // =================================================================

  void detachPreventsDoubleDelete() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *tv = lv_tabview_create(screen, nullptr);
    lv_obj_t *page_obj = lv_tabview_add_tab(tv, "Tab1");

    const LVGLWidget *pageWidget = lvgl.widget("lv_page");
    QVERIFY(pageWidget != nullptr);
    LVGLObject *pageWrapper = new LVGLObject(page_obj, pageWidget, nullptr, false, 0);

    pageWrapper->detachLvObj();
    QVERIFY(pageWrapper->obj() == nullptr);

    // This delete should NOT crash — destructor skips lv_obj_del
    delete pageWrapper;

    lv_obj_del(tv);
  }

  void detachRecursive() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *tv = lv_tabview_create(screen, nullptr);
    lv_obj_t *page_obj = lv_tabview_add_tab(tv, "Tab1");

    const LVGLWidget *pageWidget = lvgl.widget("lv_page");
    const LVGLWidget *labelWidget = lvgl.widget("lv_label");
    QVERIFY(pageWidget != nullptr);
    QVERIFY(labelWidget != nullptr);

    LVGLObject *pageWrapper = new LVGLObject(page_obj, pageWidget, nullptr, false, 0);
    lv_obj_t *lbl = lv_label_create(page_obj, nullptr);
    LVGLObject *lblWrapper = new LVGLObject(lbl, labelWidget, pageWrapper, true, -1);

    QVERIFY(pageWrapper->obj() != nullptr);
    QVERIFY(lblWrapper->obj() != nullptr);

    pageWrapper->detachLvObjRecursive();

    QVERIFY(pageWrapper->obj() == nullptr);
    QVERIFY(lblWrapper->obj() == nullptr);

    delete lblWrapper;
    delete pageWrapper;

    lv_obj_del(tv);
  }

  // =================================================================
  // Round 3: Builder property integration tests
  // These test LVGLPropertyTabs::set() — the by-name matching algorithm
  // that detects which tabs were removed, renames survivors, and appends
  // new tabs.
  // =================================================================

  // Helper: create a tabview with N tabs via the builder property system.
  LVGLObject *createTabviewWithTabs(QStringList tabNames) {
    const LVGLWidget *tvWidget = lvgl.widget("lv_tabview");
    Q_ASSERT(tvWidget != nullptr);
    lv_obj_t *screen = lv_scr_act();
    LVGLObject *tvObj = new LVGLObject(tvWidget, "test_tv", screen);
    lvgl.addObject(tvObj);

    LVGLProperty *tabsProp = tvWidget->property("Tabs");
    Q_ASSERT(tabsProp != nullptr);
    QVariantList varList;
    for (const QString &name : tabNames)
      varList.append(name);
    tabsProp->setValue(tvObj, QVariant(varList));

    return tvObj;
  }

  // Core test: old=[A, B, C], new=[A, C]
  void propertyRemovesByName() {
    LVGLObject *tvObj = createTabviewWithTabs({"A", "B", "C"});
    lv_tabview_ext_t *ext = reinterpret_cast<lv_tabview_ext_t *>(
        lv_obj_get_ext_attr(tvObj->obj()));

    QCOMPARE(ext->tab_cnt, static_cast<uint16_t>(3));
    QCOMPARE(tvObj->childs().size(), 3);

    LVGLProperty *tabsProp = lvgl.widget("lv_tabview")->property("Tabs");
    tabsProp->setValue(tvObj, QVariant(QVariantList{{"A"}, {"C"}}));

    QCOMPARE(ext->tab_cnt, static_cast<uint16_t>(2));
    QCOMPARE(tvObj->childs().size(), 2);
    QCOMPARE(QString(ext->tab_name_ptr[0]), QString("A"));
    QCOMPARE(QString(ext->tab_name_ptr[1]), QString("C"));
  }

  // Remove + add: old=[A, B, C], new=[A, C, D]
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

  // Pure rename: old=[A, B], new=[A, X]
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

  // No-op: old=[A, B], new=[A, B]
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

  // =================================================================
  // Round 4: Crash-fix tests
  // Verify that LVGLCore emits signals on addObject/removeObject,
  // and that surviving pages get corrected m_index values.
  // =================================================================

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

  void survivingPageIndexUpdated() {
    LVGLObject *tvObj = createTabviewWithTabs({"A", "B", "C"});
    QCOMPARE(tvObj->childs().size(), 3);

    QCOMPARE(tvObj->childs().at(0)->index(), 0);
    QCOMPARE(tvObj->childs().at(1)->index(), 1);
    QCOMPARE(tvObj->childs().at(2)->index(), 2);

    LVGLProperty *tabsProp = lvgl.widget("lv_tabview")->property("Tabs");
    tabsProp->setValue(tvObj, QVariant(QVariantList{{"A"}, {"C"}}));

    QCOMPARE(tvObj->childs().size(), 2);
    QCOMPARE(tvObj->childs().at(0)->index(), 0);
    QCOMPARE(tvObj->childs().at(1)->index(), 1);
  }

  // =================================================================
  // Round 5: LEFT/RIGHT button position tests (C1, C2 bug coverage)
  // =================================================================

  void removeMiddleTabLeft() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *tv = lv_tabview_create(screen, nullptr);
    lv_tabview_set_btns_pos(tv, LV_TABVIEW_BTNS_POS_LEFT);
    lv_tabview_add_tab(tv, "A");
    lv_tabview_add_tab(tv, "B");
    lv_tabview_add_tab(tv, "C");

    lv_tabview_remove_tab(tv, 1);

    QCOMPARE(lv_tabview_get_tab_count(tv), static_cast<uint16_t>(2));
    lv_tabview_ext_t *ext = reinterpret_cast<lv_tabview_ext_t *>(
        lv_obj_get_ext_attr(tv));
    QCOMPARE(QString(ext->tab_name_ptr[0]), QString("A"));
    QCOMPARE(QString(ext->tab_name_ptr[2]), QString("C"));

    lv_obj_del(tv);
  }

  void removeFirstTabRight() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *tv = lv_tabview_create(screen, nullptr);
    lv_tabview_set_btns_pos(tv, LV_TABVIEW_BTNS_POS_RIGHT);
    lv_tabview_add_tab(tv, "A");
    lv_tabview_add_tab(tv, "B");
    lv_tabview_add_tab(tv, "C");

    lv_tabview_remove_tab(tv, 0);

    QCOMPARE(lv_tabview_get_tab_count(tv), static_cast<uint16_t>(2));
    lv_tabview_ext_t *ext = reinterpret_cast<lv_tabview_ext_t *>(
        lv_obj_get_ext_attr(tv));
    QCOMPARE(QString(ext->tab_name_ptr[0]), QString("B"));
    QCOMPARE(QString(ext->tab_name_ptr[2]), QString("C"));

    lv_obj_del(tv);
  }

  void propertyGetLeft() {
    lv_obj_t *screen = lv_scr_act();
    const LVGLWidget *tvWidget = lvgl.widget("lv_tabview");
    LVGLObject *tvObj = new LVGLObject(tvWidget, "test_tv_left", screen);
    lvgl.addObject(tvObj);

    lv_tabview_set_btns_pos(tvObj->obj(), LV_TABVIEW_BTNS_POS_LEFT);

    LVGLProperty *tabsProp = tvWidget->property("Tabs");
    tabsProp->setValue(tvObj, QVariant(QVariantList{{"A"}, {"B"}, {"C"}}));

    QVariant v = tabsProp->value(tvObj);
    QCOMPARE(v.toString(), QString("A, B, C"));
  }

  void propertyRenameLeft() {
    lv_obj_t *screen = lv_scr_act();
    const LVGLWidget *tvWidget = lvgl.widget("lv_tabview");
    LVGLObject *tvObj = new LVGLObject(tvWidget, "test_rename_left", screen);
    lvgl.addObject(tvObj);
    lv_tabview_set_btns_pos(tvObj->obj(), LV_TABVIEW_BTNS_POS_LEFT);

    LVGLProperty *tabsProp = tvWidget->property("Tabs");
    tabsProp->setValue(tvObj, QVariant(QVariantList{{"A"}, {"B"}, {"C"}}));

    tabsProp->setValue(tvObj, QVariant(QVariantList{{"A"}, {"X"}, {"C"}}));

    QVariant v = tabsProp->value(tvObj);
    QCOMPARE(v.toString(), QString("A, X, C"));
  }
};

QTEST_MAIN(TestTabviewRemove)
#include "tst_tabview_remove.moc"
