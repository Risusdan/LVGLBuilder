#include "DragDropHandler.h"

#include "CanvasActions.h"
#include "LVGLCore.h"
#include "LVGLObject.h"
#include "SelectionManager.h"
#include "widgets/LVGLWidget.h"

DragDropHandler::DragDropHandler(CanvasActions *actions,
                                 SelectionManager *selection,
                                 QObject *parent)
    : QObject(parent), m_actions(actions), m_selection(selection) {}

LVGLObject *DragDropHandler::handleDrop(const QPoint &scenePos,
                                         const LVGLWidget *widgetClass,
                                         lv_obj_t *screenParent,
                                         const QSize &canvasSize) {
  auto parent = m_selection->selectObject(
      m_selection->objectsUnderCoords(scenePos, true), false);

  return m_actions->createObject(
      const_cast<LVGLWidget *>(widgetClass), parent, screenParent,
      scenePos, canvasSize);
}

void DragDropHandler::handleDragMove(const QPoint &scenePos) {
  auto sel = m_selection->selectObject(
      m_selection->objectsUnderCoords(scenePos, true), false);
  m_selection->setHoverObject(sel);
}

bool DragDropHandler::shouldAcceptDrag() const { return true; }
