#include "CanvasActions.h"

#include "LVGLCore.h"
#include "LVGLObject.h"
#include "widgets/LVGLWidget.h"

CanvasActions::CanvasActions(QObject *parent) : QObject(parent) {}

LVGLObject *CanvasActions::createObject(LVGLWidget *widgetClass,
                                        LVGLObject *parent,
                                        lv_obj_t *screenParent,
                                        const QPoint &pos,
                                        const QSize &canvasSize) {
  LVGLObject *newObj = nullptr;
  QPoint adjustedPos = pos;

  if (parent) {
    if (parent->widgetType() == LVGLWidget::TabView) {
      lv_obj_t *obj = parent->obj();
      parent = parent->findChildByIndex(lv_tabview_get_tab_act(obj));
      Q_ASSERT(parent);
    }
    newObj = new LVGLObject(widgetClass, "", parent);
    QPoint parentPos = parent->absolutePosition();
    newObj->setGeometry(QRect(pos - parentPos, widgetClass->minimumSize()));
  } else {
    newObj = new LVGLObject(widgetClass, "", screenParent);
    QSize size(std::min(widgetClass->minimumSize().width(), canvasSize.width()),
               std::min(widgetClass->minimumSize().height(),
                        canvasSize.height()));
    if (adjustedPos.x() + size.width() >= canvasSize.width())
      adjustedPos.setX(canvasSize.width() - size.width());
    if (adjustedPos.y() + size.height() >= canvasSize.height())
      adjustedPos.setY(canvasSize.height() - size.height());
    newObj->setGeometry(QRect(adjustedPos, size));
  }

  return newObj;
}

void CanvasActions::deleteObject(LVGLObject *obj) { lvgl.removeObject(obj); }

void CanvasActions::moveObject(LVGLObject *obj, int dx, int dy,
                               const QSize &canvasSize) {
  if (!obj) return;

  if ((dx != 0) && (dy != 0)) {
    obj->setPosition(obj->position() + QPoint(dx, dy));
  } else if (dx != 0) {
    obj->setX(
        qBound(0, obj->x() + dx, canvasSize.width() - obj->width() - 1));
  } else if (dy != 0) {
    obj->setY(
        qBound(0, obj->y() + dy, canvasSize.height() - obj->height() - 1));
  }
}

void CanvasActions::moveObjectTo(LVGLObject *obj, const QPoint &pos,
                                 const QSize &canvasSize) {
  if (!obj) return;

  QPoint newPos = pos;
  newPos.setX(
      qBound(0, newPos.x(), canvasSize.width() - obj->width() - 1));
  newPos.setY(
      qBound(0, newPos.y(), canvasSize.height() - obj->height() - 1));
  obj->setPosition(newPos);
}

void CanvasActions::moveForeground(LVGLObject *obj) {
  if (obj) lv_obj_move_foreground(obj->obj());
}

void CanvasActions::moveBackground(LVGLObject *obj) {
  if (obj) lv_obj_move_background(obj->obj());
}

void CanvasActions::setScreenColor(const QColor &color) {
  lvgl.setScreenColor(color);
}
