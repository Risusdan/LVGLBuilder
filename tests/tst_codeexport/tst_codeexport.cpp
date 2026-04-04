#include <QtTest>
#include <QTemporaryDir>

#include "LVGLCore.h"
#include "LVGLObject.h"
#include "LVGLProject.h"
#include "LVGLProperty.h"

class TestCodeExport : public QObject {
  Q_OBJECT

 private slots:
  void initTestCase() {
    Q_INIT_RESOURCE(res);
    lvgl.init(320, 480);
  }

  void cleanup() { lvgl.removeAllObjects(); }

  void testVal2FunctionGeneratesBothParams() {
    const LVGLWidget *arcWidget = lvgl.widget("lv_arc");
    QVERIFY(arcWidget != nullptr);
    auto *obj = new LVGLObject(arcWidget, "arc_test", lv_scr_act());
    lvgl.addObject(obj);

    // Set start=45, end=270
    lv_arc_set_angles(obj->obj(), 45, 270);

    // Find the "Angles" property and call function()
    LVGLProperty *anglesProp = nullptr;
    for (LVGLProperty *p : arcWidget->properties()) {
      if (p->name() == "Angles") {
        anglesProp = p;
        break;
      }
    }
    QVERIFY(anglesProp != nullptr);

    QStringList code = anglesProp->function(obj);
    QCOMPARE(code.size(), 1);

    QString line = code.first();
    QVERIFY2(line.contains("45"),
             qPrintable("Missing start angle in: " + line));
    QVERIFY2(line.contains("270"),
             qPrintable("Missing end angle in: " + line));
  }
};

QTEST_MAIN(TestCodeExport)
#include "tst_codeexport.moc"
