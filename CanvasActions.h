#ifndef CANVASACTIONS_H
#define CANVASACTIONS_H

/**
 * @file CanvasActions.h
 * @brief Object manipulation operations — create, delete, move, and z-order for canvas widgets.
 */

#include <QColor>
#include <QObject>
#include <QPoint>
#include <QSize>

#include "lvgl/lvgl.h"

class LVGLObject;
class LVGLWidget;

/**
 * @class CanvasActions
 * @brief Provides canvas-level operations on LVGLObjects (create, delete, move, z-order).
 *
 * Extracted from LVGLSimulator to keep canvas mutation logic separate from
 * rendering and event handling concerns.
 */
class CanvasActions : public QObject {
  Q_OBJECT
 public:
  explicit CanvasActions(QObject *parent = nullptr);

  LVGLObject *createObject(LVGLWidget *widgetClass, LVGLObject *parent,
                           lv_obj_t *screenParent, const QPoint &pos,
                           const QSize &canvasSize);

  void deleteObject(LVGLObject *obj);

  void moveObject(LVGLObject *obj, int dx, int dy, const QSize &canvasSize);
  void moveObjectTo(LVGLObject *obj, const QPoint &pos,
                    const QSize &canvasSize);

  void moveForeground(LVGLObject *obj);
  void moveBackground(LVGLObject *obj);

  void setScreenColor(const QColor &color);
};

#endif  // CANVASACTIONS_H
