#include <QSignalSpy>
#include <QTemporaryFile>
#include <QtTest>

#include "LVGLCore.h"
#include "LVGLProject.h"
#include "ProjectManager.h"

class TestProjectManager : public QObject {
  Q_OBJECT

 private slots:
  void initTestCase() {
    Q_INIT_RESOURCE(res);
    lvgl.init(320, 480);
  }

  void cleanup() { lvgl.removeAllObjects(); }

  void testNewProject() {
    ProjectManager pm;
    QSignalSpy spy(&pm, &ProjectManager::projectCreated);
    pm.newProject("Test", QSize(320, 480));
    QVERIFY(pm.hasProject());
    QCOMPARE(pm.projectName(), QString("Test"));
    QCOMPARE(spy.count(), 1);
  }

  void testSaveAndLoad() {
    ProjectManager pm;
    pm.newProject("SaveTest", QSize(320, 480));
    QTemporaryFile tmpFile;
    tmpFile.setAutoRemove(true);
    QVERIFY(tmpFile.open());
    QString filePath = tmpFile.fileName();
    tmpFile.close();
    QSignalSpy saveSpy(&pm, &ProjectManager::projectSaved);
    QVERIFY(pm.saveProject(filePath));
    QCOMPARE(saveSpy.count(), 1);

    ProjectManager pm2;
    QSignalSpy loadSpy(&pm2, &ProjectManager::projectLoaded);
    QVERIFY(pm2.loadProject(filePath));
    QCOMPARE(loadSpy.count(), 1);
    QCOMPARE(pm2.projectName(), QString("SaveTest"));
  }

  void testLoadNonexistentFails() {
    ProjectManager pm;
    QSignalSpy failSpy(&pm, &ProjectManager::loadFailed);
    QVERIFY(!pm.loadProject("/nonexistent.lvgl"));
    QCOMPARE(failSpy.count(), 1);
  }

  void testSaveWithoutProjectFails() {
    ProjectManager pm;
    QVERIFY(!pm.saveProject("/tmp/test.lvgl"));
  }
};

QTEST_MAIN(TestProjectManager)
#include "tst_projectmanager.moc"
