#ifndef RECENTFILESMANAGER_H
#define RECENTFILESMANAGER_H

#include <QAction>
#include <QList>
#include <QMenu>
#include <QObject>
#include <QString>

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
