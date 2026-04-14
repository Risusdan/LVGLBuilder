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
