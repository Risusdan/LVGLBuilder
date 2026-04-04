#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryFile>
#include <QtTest>

#include "LVGLCore.h"
#include "LVGLObject.h"
#include "LVGLProject.h"

class TestLVGLProject : public QObject {
  Q_OBJECT

 private slots:
  void initTestCase() {
    Q_INIT_RESOURCE(res);
    lvgl.init(320, 480);
  }

  void cleanup() { lvgl.removeAllObjects(); }

  void testSaveAndLoad() {
    LVGLProject project("TestProject", QSize(320, 480));
    const LVGLWidget *btnWidget = lvgl.widget("lv_btn");
    QVERIFY(btnWidget != nullptr);

    lv_obj_t *screen = lv_scr_act();
    LVGLObject *btn = new LVGLObject(btnWidget, "test_btn", screen);
    btn->setGeometry(QRect(10, 20, 100, 35));
    lvgl.addObject(btn);

    QTemporaryFile tmpFile;
    tmpFile.setAutoRemove(true);
    QVERIFY(tmpFile.open());
    QString filePath = tmpFile.fileName();
    tmpFile.close();

    QVERIFY(project.save(filePath));
    lvgl.removeAllObjects();

    LVGLProject *loaded = LVGLProject::load(filePath);
    QVERIFY(loaded != nullptr);
    QCOMPARE(loaded->name(), QString("TestProject"));
    QCOMPARE(loaded->resolution(), QSize(320, 480));

    QList<LVGLObject *> objects = lvgl.allObjects();
    QCOMPARE(objects.size(), 1);
    QCOMPARE(objects.first()->widgetClass()->name(), QString("Button"));

    lvgl.removeAllObjects();
    delete loaded;
  }

  void testSaveProducesValidJson() {
    LVGLProject project("JsonTest", QSize(640, 480));

    QTemporaryFile tmpFile;
    tmpFile.setAutoRemove(true);
    QVERIFY(tmpFile.open());
    QString filePath = tmpFile.fileName();
    tmpFile.close();

    QVERIFY(project.save(filePath));

    QFile file(filePath);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QVERIFY(!doc.isNull());
    QVERIFY(doc.object().contains("lvgl"));
    QJsonObject lvglObj = doc["lvgl"].toObject();
    QCOMPARE(lvglObj["name"].toString(), QString("JsonTest"));
    QVERIFY(lvglObj.contains("resolution"));
    QVERIFY(lvglObj.contains("widgets"));
  }

  void testLoadMissingFileFails() {
    LVGLProject *p = LVGLProject::load("/nonexistent/path.lvgl");
    QVERIFY(p == nullptr);
  }

  void testLoadInvalidJsonFails() {
    QTemporaryFile tmpFile;
    tmpFile.setAutoRemove(true);
    QVERIFY(tmpFile.open());
    tmpFile.write("not json");
    QString filePath = tmpFile.fileName();
    tmpFile.close();

    LVGLProject *p = LVGLProject::load(filePath);
    QVERIFY(p == nullptr);
  }

  void testResolutionRoundTrip() {
    LVGLProject project("ResTest", QSize(800, 600));

    QTemporaryFile tmpFile;
    tmpFile.setAutoRemove(true);
    QVERIFY(tmpFile.open());
    QString filePath = tmpFile.fileName();
    tmpFile.close();

    QVERIFY(project.save(filePath));

    LVGLProject *loaded = LVGLProject::load(filePath);
    QVERIFY(loaded != nullptr);
    QCOMPARE(loaded->resolution(), QSize(800, 600));
    delete loaded;
  }
};

QTEST_MAIN(TestLVGLProject)
#include "tst_lvglproject.moc"
