#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

/**
 * @file ProjectManager.h
 * @brief Manages project lifecycle — new, open, save, save-as, export operations.
 */

#include <QObject>
#include <QSize>
#include <QString>

class LVGLProject;

/**
 * @class ProjectManager
 * @brief Coordinates project file operations and emits signals on state changes.
 */
class ProjectManager : public QObject {
  Q_OBJECT

 public:
  explicit ProjectManager(QObject *parent = nullptr);
  ~ProjectManager();

  LVGLProject *project() const;
  bool hasProject() const;

  void newProject(const QString &name, QSize resolution);
  bool saveProject(const QString &fileName);
  bool loadProject(const QString &fileName);
  bool exportCode(const QString &path) const;

  QString fileName() const;
  QString projectName() const;

 signals:
  void projectLoaded(const QString &fileName);
  void projectSaved(const QString &fileName);
  void projectCreated(const QString &name, QSize resolution);
  void loadFailed(const QString &fileName);

 private:
  LVGLProject *m_project;
};

#endif  // PROJECTMANAGER_H
