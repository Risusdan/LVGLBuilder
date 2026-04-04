#include "RecentFilesManager.h"

#include <QFile>
#include <QFileInfo>
#include <QSettings>

RecentFilesManager::RecentFilesManager(int maxFiles, QObject *parent)
    : QObject(parent), m_maxFiles(maxFiles) {}

void RecentFilesManager::setupMenu(QMenu *menu) {
  for (int i = 0; i < m_maxFiles; ++i) {
    QAction *action = new QAction(this);
    action->setVisible(false);
    connect(action, &QAction::triggered, this,
            &RecentFilesManager::onActionTriggered);
    m_actions.append(action);
    menu->addAction(action);
  }
  updateActions();
}

void RecentFilesManager::addFile(const QString &fileName) {
  QSettings settings("at.fhooe.lvgl", "LVGL Builder");
  QStringList recentFilePaths = settings.value("recentFiles").toStringList();
  recentFilePaths.removeAll(fileName);
  recentFilePaths.prepend(fileName);
  while (recentFilePaths.size() > m_maxFiles) recentFilePaths.removeLast();
  settings.setValue("recentFiles", recentFilePaths);

  updateActions();
}

QStringList RecentFilesManager::recentFiles() const {
  QSettings settings("at.fhooe.lvgl", "LVGL Builder");
  QStringList result;
  for (const QString &f : settings.value("recentFiles").toStringList()) {
    if (QFile(f).exists()) result.push_back(f);
  }
  return result;
}

void RecentFilesManager::onActionTriggered() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (action) emit fileSelected(action->data().toString());
}

void RecentFilesManager::updateActions() {
  if (m_actions.isEmpty()) return;

  QStringList files = recentFiles();
  int count = qMin(files.size(), m_actions.size());

  for (int i = 0; i < count; ++i) {
    m_actions[i]->setText(QFileInfo(files[i]).fileName());
    m_actions[i]->setData(files[i]);
    m_actions[i]->setVisible(true);
  }

  for (int i = count; i < m_actions.size(); ++i)
    m_actions[i]->setVisible(false);
}
