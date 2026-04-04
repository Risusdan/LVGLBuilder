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
  void testExportHeaderHasValidIdentifiers() {
    const LVGLWidget *btnWidget = lvgl.widget("lv_btn");
    auto *obj = new LVGLObject(btnWidget, "btn-test#1", lv_scr_act());
    obj->setAccessible(true);
    lvgl.addObject(obj);

    LVGLProject project("Test App", QSize(320, 480));
    QTemporaryDir tmpDir;
    QVERIFY(tmpDir.isValid());
    QVERIFY(project.exportCode(tmpDir.path()));

    // Read the generated .h file
    QFile hFile(tmpDir.path() + "/test app.h");
    QVERIFY(hFile.open(QIODevice::ReadOnly));
    QString hContent = hFile.readAll();
    hFile.close();

    // The extern declaration must use a sanitized name
    QVERIFY(hContent.contains("extern lv_obj_t *btn_test_1;"));
    // Must NOT contain the raw unsanitized name
    QVERIFY(!hContent.contains("btn-test#1"));
  }

  void testExportSanitizesProjectName() {
    const LVGLWidget *btnWidget = lvgl.widget("lv_btn");
    auto *obj = new LVGLObject(btnWidget, "btn1", lv_scr_act());
    lvgl.addObject(obj);

    LVGLProject project("My App-v2", QSize(320, 480));
    QTemporaryDir tmpDir;
    QVERIFY(tmpDir.isValid());
    QVERIFY(project.exportCode(tmpDir.path()));

    // Read the .c file
    QFile cFile(tmpDir.path() + "/my app-v2.c");
    QVERIFY(cFile.open(QIODevice::ReadOnly));
    QString cContent = cFile.readAll();
    cFile.close();

    // The function name must use underscores, not spaces or dashes
    QVERIFY(cContent.contains("void my_app_v2_create(lv_obj_t *parent)"));

    // Read the .h file — check header guard
    QFile hFile(tmpDir.path() + "/my app-v2.h");
    QVERIFY(hFile.open(QIODevice::ReadOnly));
    QString hContent = hFile.readAll();
    hFile.close();

    // Header guard must be valid C preprocessor identifier
    QVERIFY(hContent.contains("#ifndef MY_APP_V2_H"));
    QVERIFY(hContent.contains("#define MY_APP_V2_H"));
  }

  void testExportCFileStructure() {
    const LVGLWidget *btnWidget = lvgl.widget("lv_btn");
    auto *obj = new LVGLObject(btnWidget, "my_button", lv_scr_act());
    lvgl.addObject(obj);

    LVGLProject project("TestApp", QSize(320, 480));
    QTemporaryDir tmpDir;
    QVERIFY(tmpDir.isValid());
    QVERIFY(project.exportCode(tmpDir.path()));

    // Read the generated .c file
    QFile cFile(tmpDir.path() + "/testapp.c");
    QVERIFY(cFile.open(QIODevice::ReadOnly));
    QString cContent = cFile.readAll();
    cFile.close();

    // Must include the header
    QVERIFY(cContent.contains("#include \"testapp.h\""));
    // Must have the create function
    QVERIFY(cContent.contains("void testapp_create(lv_obj_t *parent)"));
    // Must have the widget creation call
    QVERIFY(cContent.contains("lv_btn_create(parent, NULL)"));
  }
};

QTEST_MAIN(TestCodeExport)
#include "tst_codeexport.moc"
