#ifndef RECENTFILESMANAGER_H
#define RECENTFILESMANAGER_H

/**
 * @file RecentFilesManager.h
 * @brief Tracks and displays recently opened project files in the File menu.
 */

#include <QAction>
#include <QList>
#include <QMenu>
#include <QObject>
#include <QString>

/**
 * @class RecentFilesManager
 * @brief Maintains a bounded list of recently used file paths and populates a QMenu.
 */
class RecentFilesManager : public QObject {
  Q_OBJECT

 public:
  explicit RecentFilesManager(int maxFiles, QObject *parent = nullptr);

  void setupMenu(QMenu *menu);
  void addFile(const QString &fileName);
  QStringList recentFiles() const;

 signals:
  void fileSelected(const QString &fileName);

 private slots:
  void onActionTriggered();

 private:
  void updateActions();

  QList<QAction *> m_actions;
  int m_maxFiles;
};

#endif  // RECENTFILESMANAGER_H
