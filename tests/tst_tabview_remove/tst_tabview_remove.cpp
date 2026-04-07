// tests/tst_tabview_remove/tst_tabview_remove.cpp
#include <QtTest>

#include "LVGLCore.h"
#include "LVGLObject.h"
#include "LVGLProperty.h"

class TestTabviewRemove : public QObject {
  Q_OBJECT

 private slots:
  void initTestCase() {
    Q_INIT_RESOURCE(res);
    lvgl.init(320, 480);
  }

  void cleanup() { lvgl.removeAllObjects(); }

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

    // Active tab should be clamped to 0
    QCOMPARE(lv_tabview_get_tab_act(tv), static_cast<uint16_t>(0));

    lv_obj_del(tv);
  }

  void removeFirstTab() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *tv = lv_tabview_create(screen, nullptr);
    lv_tabview_add_tab(tv, "X");
    lv_tabview_add_tab(tv, "Y");

    lv_tabview_remove_tab(tv, 0);  // remove first

    QCOMPARE(lv_tabview_get_tab_count(tv), static_cast<uint16_t>(1));
    lv_tabview_ext_t *ext = reinterpret_cast<lv_tabview_ext_t *>(
        lv_obj_get_ext_attr(tv));
    QCOMPARE(QString(ext->tab_name_ptr[0]), QString("Y"));
    QCOMPARE(lv_tabview_get_tab_act(tv), static_cast<uint16_t>(0));

    lv_obj_del(tv);
  }

  void removeLastTab() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *tv = lv_tabview_create(screen, nullptr);
    lv_tabview_add_tab(tv, "P");
    lv_tabview_add_tab(tv, "Q");
    lv_tabview_add_tab(tv, "R");

    // Set active to last tab, then remove it
    lv_tabview_set_tab_act(tv, 2, LV_ANIM_OFF);
    lv_tabview_remove_tab(tv, 2);

    QCOMPARE(lv_tabview_get_tab_count(tv), static_cast<uint16_t>(2));
    // Active tab should clamp to 1
    QCOMPARE(lv_tabview_get_tab_act(tv), static_cast<uint16_t>(1));

    lv_obj_del(tv);
  }

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

  void removeOutOfBoundsIgnored() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *tv = lv_tabview_create(screen, nullptr);
    lv_tabview_add_tab(tv, "A");
    lv_tabview_add_tab(tv, "B");

    lv_tabview_remove_tab(tv, 99);  // out of bounds — no-op

    QCOMPARE(lv_tabview_get_tab_count(tv), static_cast<uint16_t>(2));

    lv_obj_del(tv);
  }
  void propertyRemovesTabAndObject() {
    // Create a tabview via builder objects
    const LVGLWidget *tvWidget = lvgl.widget("lv_tabview");
    QVERIFY(tvWidget != nullptr);
    lv_obj_t *screen = lv_scr_act();
    LVGLObject *tvObj = new LVGLObject(tvWidget, "test_tv", screen);
    lvgl.addObject(tvObj);

    // Use the Tabs property to add 3 tabs: "A", "B", "C"
    LVGLProperty *tabsProp = tvWidget->property("Tabs");
    QVERIFY(tabsProp != nullptr);
    tabsProp->setValue(tvObj, QVariant(QVariantList{{"A"}, {"B"}, {"C"}}));

    // Verify 3 tabs exist
    lv_tabview_ext_t *ext = reinterpret_cast<lv_tabview_ext_t *>(
        lv_obj_get_ext_attr(tvObj->obj()));
    QCOMPARE(ext->tab_cnt, static_cast<uint16_t>(3));
    // 3 page LVGLObjects should be children
    QCOMPARE(tvObj->childs().size(), 3);

    // Now set list to just "A", "C" — removing "B"
    tabsProp->setValue(tvObj, QVariant(QVariantList{{"A"}, {"C"}}));

    QCOMPARE(ext->tab_cnt, static_cast<uint16_t>(2));
    QCOMPARE(tvObj->childs().size(), 2);
    QCOMPARE(QString(ext->tab_name_ptr[0]), QString("A"));
    QCOMPARE(QString(ext->tab_name_ptr[1]), QString("C"));
  }
};

QTEST_MAIN(TestTabviewRemove)
#include "tst_tabview_remove.moc"
