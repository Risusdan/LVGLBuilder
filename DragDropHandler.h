#ifndef DRAGDROPHANDLER_H
#define DRAGDROPHANDLER_H

#include <QObject>
#include <QPoint>
#include <QSize>

struct _lv_obj_t;
typedef struct _lv_obj_t lv_obj_t;

class CanvasActions;
class SelectionManager;
class LVGLObject;
class LVGLWidget;

class DragDropHandler : public QObject {
  Q_OBJECT
 public:
  explicit DragDropHandler(CanvasActions *actions,
                           SelectionManager *selection,
                           QObject *parent = nullptr);

  LVGLObject *handleDrop(const QPoint &scenePos,
                         const LVGLWidget *widgetClass,
                         lv_obj_t *screenParent,
                         const QSize &canvasSize);
  void handleDragMove(const QPoint &scenePos);
  bool shouldAcceptDrag() const;

 private:
  CanvasActions *m_actions;
  SelectionManager *m_selection;
};

#endif  // DRAGDROPHANDLER_H
