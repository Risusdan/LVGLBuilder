#include "LVGLObjectManager.h"

#include "LVGLObject.h"

LVGLObjectManager::LVGLObjectManager(QObject *parent) : QObject(parent) {}

void LVGLObjectManager::addObject(LVGLObject *object) {
  m_objects << object;
}

void LVGLObjectManager::removeObject(LVGLObject *object) {
  auto childs = object->childs();
  while (!childs.isEmpty()) {
    removeObject(childs.at(0));
    childs = object->childs();
  }
  if (object->parent()) object->parent()->removeChild(object);
  m_objects.removeOne(object);
  delete object;
  object = nullptr;
}

void LVGLObjectManager::detachObject(LVGLObject *object) {
  m_objects.removeOne(object);
}

void LVGLObjectManager::removeAllObjects() {
  auto objs = m_objects;
  while (!objs.isEmpty()) {
    if (objs.at(0) && !objs.at(0)->parent()) removeObject(m_objects.at(0));
    objs = m_objects;
  }
}

void LVGLObjectManager::setAllObjects(QList<LVGLObject *> objs) {
  m_objects = objs;
}

QList<LVGLObject *> LVGLObjectManager::allObjects() const {
  return m_objects;
}

QList<LVGLObject *> LVGLObjectManager::topLevelObjects() const {
  QList<LVGLObject *> ret;
  for (LVGLObject *c : m_objects) {
    if (c->parent() == nullptr) ret << c;
  }
  return ret;
}

QList<LVGLObject *> LVGLObjectManager::objectsByType(QString className) const {
  QList<LVGLObject *> ret;
  for (LVGLObject *c : m_objects) {
    if (c->widgetClass()->className() == className) ret << c;
  }
  return ret;
}

LVGLObject *LVGLObjectManager::object(QString name) const {
  if (name.isEmpty()) return nullptr;
  for (LVGLObject *c : m_objects) {
    if (c->name() == name) return c;
  }
  return nullptr;
}

LVGLObject *LVGLObjectManager::object(lv_obj_t *obj) const {
  if (obj == nullptr) return nullptr;
  for (LVGLObject *c : m_objects) {
    if (c->obj() == obj) return c;
  }
  return nullptr;
}

LVGLObject *LVGLObjectManager::findByName(const QString &name) const {
  if (!m_objects.isEmpty()) {
    for (auto o : m_objects)
      if (o->name() == name) return o;
  }
  return nullptr;
}

void LVGLObjectManager::clear() { m_objects.clear(); }
