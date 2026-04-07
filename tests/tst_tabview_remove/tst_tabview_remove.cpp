// tests/tst_tabview_remove/tst_tabview_remove.cpp
#include <QtTest>

#include "LVGLCore.h"
#include "LVGLObject.h"

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
};

QTEST_MAIN(TestTabviewRemove)
#include "tst_tabview_remove.moc"
