#ifndef LVGLOBJECTMANAGER_H
#define LVGLOBJECTMANAGER_H

/**
 * @file LVGLObjectManager.h
 * @brief Tracks all LVGLObject instances in the current project.
 */

#include <QList>
#include <QObject>
#include <QString>

#include <lvgl/lvgl.h>

class LVGLObject;

/**
 * @class LVGLObjectManager
 * @brief Registry for widget instances — add, remove, and query by name, type, or lv_obj_t.
 */
class LVGLObjectManager : public QObject {
  Q_OBJECT
 public:
  explicit LVGLObjectManager(QObject *parent = nullptr);

  void addObject(LVGLObject *object);
  void removeObject(LVGLObject *object);
  void removeAllObjects();

  void setAllObjects(QList<LVGLObject *> objs);
  QList<LVGLObject *> allObjects() const;
  QList<LVGLObject *> topLevelObjects() const;
  QList<LVGLObject *> objectsByType(QString className) const;
  LVGLObject *object(QString name) const;
  LVGLObject *object(lv_obj_t *obj) const;
  LVGLObject *findByName(const QString &name) const;

  void clear();

 private:
  QList<LVGLObject *> m_objects;
};

#endif  // LVGLOBJECTMANAGER_H
