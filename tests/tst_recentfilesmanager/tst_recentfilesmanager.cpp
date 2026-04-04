#include <QMenu>
#include <QSettings>
#include <QSignalSpy>
#include <QTemporaryFile>
#include <QTest>

#include "RecentFilesManager.h"

class tst_RecentFilesManager : public QObject {
  Q_OBJECT

 private:
  void clearRecentSettings() {
    QSettings settings("at.fhooe.lvgl", "LVGL Builder");
    settings.remove("recentFiles");
    settings.sync();
  }

 private slots:
  void init() { clearRecentSettings(); }
  void cleanup() { clearRecentSettings(); }

  void constructorSetsMaxFiles() {
    RecentFilesManager mgr(3);
    // No files added yet, recentFiles should be empty
    QVERIFY(mgr.recentFiles().isEmpty());
  }

  void setupMenuCreatesActions() {
    RecentFilesManager mgr(4);
    QMenu menu;
    mgr.setupMenu(&menu);
    QCOMPARE(menu.actions().size(), 4);
    // All actions should be hidden initially (no recent files)
    for (QAction *a : menu.actions()) {
      QVERIFY(!a->isVisible());
    }
  }

  void addFilePersistsToSettings() {
    // Create a real temp file so recentFiles() finds it existing
    QTemporaryFile tmp;
    QVERIFY(tmp.open());
    QString path = tmp.fileName();

    RecentFilesManager mgr(5);
    mgr.addFile(path);

    QStringList files = mgr.recentFiles();
    QCOMPARE(files.size(), 1);
    QCOMPARE(files.first(), path);
  }

  void addFilePrependsAndDeduplicates() {
    QTemporaryFile tmp1, tmp2;
    QVERIFY(tmp1.open());
    QVERIFY(tmp2.open());

    RecentFilesManager mgr(5);
    mgr.addFile(tmp1.fileName());
    mgr.addFile(tmp2.fileName());
    mgr.addFile(tmp1.fileName());  // re-add tmp1, should move to front

    QStringList files = mgr.recentFiles();
    QCOMPARE(files.size(), 2);
    QCOMPARE(files[0], tmp1.fileName());
    QCOMPARE(files[1], tmp2.fileName());
  }

  void addFileRespectsMaxFiles() {
    QTemporaryFile tmp1, tmp2, tmp3;
    QVERIFY(tmp1.open());
    QVERIFY(tmp2.open());
    QVERIFY(tmp3.open());

    RecentFilesManager mgr(2);  // max 2
    mgr.addFile(tmp1.fileName());
    mgr.addFile(tmp2.fileName());
    mgr.addFile(tmp3.fileName());

    QStringList files = mgr.recentFiles();
    QCOMPARE(files.size(), 2);
    QCOMPARE(files[0], tmp3.fileName());
    QCOMPARE(files[1], tmp2.fileName());
  }

  void recentFilesFiltersNonExistent() {
    RecentFilesManager mgr(5);
    // Directly write a non-existent path to settings
    QSettings settings("at.fhooe.lvgl", "LVGL Builder");
    settings.setValue("recentFiles",
                      QStringList{"/no/such/file/ever.lvgl"});
    settings.sync();

    QVERIFY(mgr.recentFiles().isEmpty());
  }

  void setupMenuUpdatesActionsOnAdd() {
    QTemporaryFile tmp;
    QVERIFY(tmp.open());

    RecentFilesManager mgr(5);
    QMenu menu;
    mgr.setupMenu(&menu);

    // All hidden initially
    for (QAction *a : menu.actions()) QVERIFY(!a->isVisible());

    mgr.addFile(tmp.fileName());

    // First action should now be visible with the file name
    QVERIFY(menu.actions()[0]->isVisible());
    QCOMPARE(menu.actions()[0]->data().toString(), tmp.fileName());
    // Rest should still be hidden
    for (int i = 1; i < menu.actions().size(); ++i)
      QVERIFY(!menu.actions()[i]->isVisible());
  }

  void actionTriggerEmitsFileSelected() {
    QTemporaryFile tmp;
    QVERIFY(tmp.open());

    RecentFilesManager mgr(5);
    QMenu menu;
    mgr.setupMenu(&menu);
    mgr.addFile(tmp.fileName());

    QSignalSpy spy(&mgr, &RecentFilesManager::fileSelected);
    QVERIFY(spy.isValid());

    menu.actions()[0]->trigger();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0].toString(), tmp.fileName());
  }
};

QTEST_MAIN(tst_RecentFilesManager)
#include "tst_recentfilesmanager.moc"
