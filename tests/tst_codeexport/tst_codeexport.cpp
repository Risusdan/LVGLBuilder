#include <QtTest>
#include <QTemporaryDir>

#include "LVGLCore.h"
#include "LVGLObject.h"
#include "LVGLProject.h"

class TestCodeExport : public QObject {
  Q_OBJECT

 private slots:
  void initTestCase() {
    Q_INIT_RESOURCE(res);
    lvgl.init(320, 480);
  }

  void cleanup() { lvgl.removeAllObjects(); }

  void testPlaceholder() { QVERIFY(true); }
};

QTEST_MAIN(TestCodeExport)
#include "tst_codeexport.moc"
