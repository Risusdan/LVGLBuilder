#include "SelectionManager.h"

#include "LVGLCore.h"
#include "LVGLObject.h"
#include "widgets/LVGLWidget.h"

SelectionManager::SelectionManager(QObject *parent)
    : QObject(parent), m_selectedObject(nullptr), m_hoverObject(nullptr) {}

LVGLObject *SelectionManager::selectedObject() const {
  return m_selectedObject;
}

LVGLObject *SelectionManager::hoverObject() const { return m_hoverObject; }

void SelectionManager::setHoverObject(LVGLObject *obj) {
  m_hoverObject = obj;
}

void SelectionManager::select(LVGLObject *obj) {
  if (m_selectedObject == obj) return;
  if (obj && obj->isLocked()) return;

  m_selectedObject = obj;
  emit selectionChanged(m_selectedObject);
}

void SelectionManager::clearSelection() { select(nullptr); }

LVGLObject *SelectionManager::selectObject(
    const QList<LVGLObject *> &candidates, bool doubleClick) const {
  if (candidates.empty()) return nullptr;

  if (doubleClick) {
    for (LVGLObject *obj : candidates) {
      if (obj->widgetType() == LVGLWidget::Label) return obj;
    }
  }
  return candidates.last();
}

QList<LVGLObject *> SelectionManager::objectsUnderCoords(
    QPoint pos, bool includeLocked) const {
  QList<LVGLObject *> ret;
  QRect screen = QRect(QPoint(0, 0), lvgl.size()).adjusted(-50, -50, 50, 50);
  for (LVGLObject *o : lvgl.allObjects()) {
    if (!includeLocked && o->isLocked()) continue;
    QRect geo = o->geometry();
    if (screen.contains(geo) && geo.contains(pos)) ret << o;
  }
  return ret;
}
