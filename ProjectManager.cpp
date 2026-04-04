#include "ProjectManager.h"

#include "LVGLProject.h"

ProjectManager::ProjectManager(QObject *parent)
    : QObject(parent), m_project(nullptr) {}

ProjectManager::~ProjectManager() { delete m_project; }

LVGLProject *ProjectManager::project() const { return m_project; }

bool ProjectManager::hasProject() const { return m_project != nullptr; }

void ProjectManager::newProject(const QString &name, QSize resolution) {
  delete m_project;
  m_project = new LVGLProject(name, resolution);
  emit projectCreated(name, resolution);
}

bool ProjectManager::saveProject(const QString &fileName) {
  if (m_project == nullptr) return false;
  bool ok = m_project->save(fileName);
  if (ok) emit projectSaved(fileName);
  return ok;
}

bool ProjectManager::loadProject(const QString &fileName) {
  delete m_project;
  m_project = LVGLProject::load(fileName);
  if (m_project == nullptr) {
    emit loadFailed(fileName);
    return false;
  }
  emit projectLoaded(fileName);
  return true;
}

bool ProjectManager::exportCode(const QString &path) const {
  if (m_project == nullptr) return false;
  return m_project->exportCode(path);
}

QString ProjectManager::fileName() const {
  if (m_project == nullptr) return QString();
  return m_project->fileName();
}

QString ProjectManager::projectName() const {
  if (m_project == nullptr) return QString();
  return m_project->name();
}
