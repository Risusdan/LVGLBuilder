#include <QtTest>

#include "LVGLCore.h"
#include "LVGLObject.h"

class TestLVGLObject : public QObject {
  Q_OBJECT

 private slots:
  void initTestCase() {
    Q_INIT_RESOURCE(res);
    lvgl.init(320, 480);
  }

  void cleanup() { lvgl.removeAllObjects(); }

  void testPlaceholder() { QVERIFY(true); }
};

QTEST_MAIN(TestLVGLObject)
#include "tst_lvglobject.moc"
