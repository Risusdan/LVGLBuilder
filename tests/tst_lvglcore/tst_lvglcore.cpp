#include <QtTest>

#include "LVGLCore.h"
#include "LVGLObject.h"
#include "LVGLDisplay.h"
#include "LVGLFontManager.h"
#include "LVGLImageManager.h"
#include "LVGLObjectManager.h"
#include "LVGLWidgetRegistry.h"

class TestLVGLCore : public QObject {
  Q_OBJECT

 private slots:
  void initTestCase() {
    Q_INIT_RESOURCE(res);
    lvgl.init(320, 480);
  }

  void cleanup() { lvgl.removeAllObjects(); }

  void testServicesExist() {
    QVERIFY(lvgl.display() != nullptr);
    QVERIFY(lvgl.imageManager() != nullptr);
    QVERIFY(lvgl.fontManager() != nullptr);
    QVERIFY(lvgl.widgetRegistry() != nullptr);
    QVERIFY(lvgl.objectManager() != nullptr);
  }

  void testFacadeImageDelegation() {
    QStringList facadeNames = lvgl.imageNames();
    QStringList serviceNames = lvgl.imageManager()->imageNames();
    QCOMPARE(facadeNames, serviceNames);
  }

  void testFacadeFontDelegation() {
    QStringList facadeNames = lvgl.fontNames();
    QStringList serviceNames = lvgl.fontManager()->fontNames();
    QCOMPARE(facadeNames, serviceNames);
  }

  void testFacadeWidgetDelegation() {
    // Widget registry is keyed by className like "lv_btn"
    const LVGLWidget *facadeResult = lvgl.widget("lv_btn");
    const LVGLWidget *serviceResult = lvgl.widgetRegistry()->widget("lv_btn");
    QCOMPARE(facadeResult, serviceResult);
    QVERIFY(facadeResult != nullptr);
  }

  void testFacadeObjectDelegation() {
    const LVGLWidget *btnWidget = lvgl.widget("lv_btn");
    lv_obj_t *screen = lv_scr_act();
    LVGLObject *btn = new LVGLObject(btnWidget, "deleg_btn", screen);
    lvgl.addObject(btn);

    QCOMPARE(lvgl.allObjects().size(), 1);
    QCOMPARE(lvgl.objectManager()->allObjects().size(), 1);
    QCOMPARE(lvgl.allObjects().first(), lvgl.objectManager()->allObjects().first());
  }

  void testFacadeDisplayDelegation() {
    QCOMPARE(lvgl.width(), lvgl.display()->width());
    QCOMPARE(lvgl.height(), lvgl.display()->height());
    QCOMPARE(lvgl.size(), lvgl.display()->size());
    QCOMPARE(lvgl.screenColor(), lvgl.display()->screenColor());
  }
};

QTEST_MAIN(TestLVGLCore)
#include "tst_lvglcore.moc"
