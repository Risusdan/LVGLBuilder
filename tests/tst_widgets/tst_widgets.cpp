#include <QtTest>

#include "LVGLCore.h"
#include "LVGLObject.h"
#include "LVGLWidgetRegistry.h"

class TestWidgets : public QObject {
  Q_OBJECT

 private slots:
  void initTestCase() {
    Q_INIT_RESOURCE(res);
    lvgl.init(320, 480);
  }

  void cleanup() { lvgl.removeAllObjects(); }

  void testAllWidgetTypesCreateSuccessfully() {
    auto widgets = lvgl.widgets();
    QVERIFY(widgets.size() > 0);

    for (const LVGLWidget *w : widgets) {
      auto *obj = new LVGLObject(w, QString("test_%1").arg(w->className()), lv_scr_act());
      lvgl.addObject(obj);

      QVERIFY2(obj->obj() != nullptr, qPrintable("Failed to create: " + w->className()));
      QVERIFY2(!obj->name().isEmpty(), qPrintable("Empty name for: " + w->className()));
      QVERIFY2(obj->widgetClass() == w, qPrintable("Widget class mismatch for: " + w->className()));
    }
  }

  void testAllWidgetTypesSetStyleWithoutCrash() {
    auto widgets = lvgl.widgets();

    for (const LVGLWidget *w : widgets) {
      auto *obj = new LVGLObject(w, QString("style_%1").arg(w->className()), lv_scr_act());
      lvgl.addObject(obj);

      QStringList styleNames = w->styles();
      for (int i = 0; i < styleNames.size(); ++i) {
        // Skip button sub-object styles for lv_mbox when no buttons are
        // configured -- LVGL dereferences a NULL btnm pointer internally.
        if (w->className() == "lv_mbox" && i >= 1)
          continue;

        lv_style_t *s = obj->style(i);
        QVERIFY2(s != nullptr, qPrintable(
            QString("Null style for %1 type %2").arg(w->className()).arg(i)));
        w->setStyle(obj->obj(), i, s);
      }
    }
  }
};

QTEST_MAIN(TestWidgets)
#include "tst_widgets.moc"
