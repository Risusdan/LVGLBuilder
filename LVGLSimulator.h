/**
 * @file LVGLSimulator.h
 * @brief Canvas view for rendering the LVGL display and handling interactive editing.
 *
 * Contains three classes:
 * - LVGLScene: QGraphicsScene that paints the LVGL framebuffer as the background.
 * - LVGLSimulator: QGraphicsView that manages widget selection, dragging, and
 *   drop-to-create from the palette.
 * - LVGLKeyPressEventFilter: Event filter for keyboard shortcuts (delete, arrow keys).
 */

#ifndef LVGLSIMULATOR_H
#define LVGLSIMULATOR_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTextStream>

#include "lvgl/lvgl.h"

class CanvasActions;
class DragDropHandler;
class LVGLObject;
class LVGLItem;
class LVGLObjectModel;
class LVGLPropertyModel;
class SelectionManager;
class QTimer;
class QThread;

/**
 * @class LVGLScene
 * @brief QGraphicsScene subclass that renders the LVGL framebuffer as its background.
 *
 * Overrides drawBackground() to poll LVGL (advancing tick/tasks), paint the
 * current framebuffer, and draw selection/hover rectangles over active objects.
 */
class LVGLScene : public QGraphicsScene {
  Q_OBJECT
 public:
  LVGLScene(QObject *parent = nullptr);

  /** @brief Returns the currently selected LVGLObject, or nullptr. */
  LVGLObject *selected() const;

  /** @brief Returns the object currently under the mouse cursor, or nullptr. */
  LVGLObject *hoverObject() const;
  /** @brief Sets the hover-tracked object (for visual feedback). */
  void setHoverObject(LVGLObject *hoverObject);

 public slots:
  /** @brief Sets the selected object and triggers scene update. */
  void setSelected(LVGLObject *selected);

 protected:
  virtual void drawBackground(QPainter *painter, const QRectF &rect) override;

 private:
  LVGLObject *m_selected;
  LVGLObject *m_hoverObject;
};

/**
 * @class LVGLSimulator
 * @brief Main canvas view — renders the LVGL display and handles all user interaction.
 *
 * Coordinates three concerns:
 * - **Rendering:** Periodically polls LVGLCore and repaints the scene with the
 *   updated framebuffer.
 * - **Selection and dragging:** In design mode, handles mouse events to select,
 *   move, and resize widgets. Hit-testing uses SelectionManager (Qt-side geometry
 *   checks), not LVGL. Drag state is tracked manually (start position + delta)
 *   to coordinate between LVGLItem resize and whole-object moves.
 * - **Run mode:** When mouse is enabled, forwards mouse coordinates to LVGL
 *   via sendMouseEvent() so the UI can be interacted with as on a real device.
 * - **Widget creation:** Accepts drag-and-drop from the widget palette (LVGLWidgetModel)
 *   to create new widget instances on the canvas.
 *
 * Delegates specific responsibilities to helper classes: CanvasActions (object
 * creation, deletion, and movement), SelectionManager (hit-testing and selection
 * tracking), and DragDropHandler (palette drop processing).
 */
class LVGLSimulator : public QGraphicsView {
  Q_OBJECT
 public:
  explicit LVGLSimulator(QWidget *parent = nullptr);
  ~LVGLSimulator() override;

  /** @brief Returns the currently selected LVGLObject, or nullptr. */
  LVGLObject *selectedObject() const;

  /** @brief Returns the LVGLItem graphics item. */
  LVGLItem *item() const;
  /** @brief Returns the object manipulation handler (create, delete, move). */
  CanvasActions *canvasActions() const;
  /** @brief Returns the selection and hit-testing manager. */
  SelectionManager *selectionManager() const;

  /**
   * @brief Moves a widget by a relative offset.
   * @param obj The widget to move.
   * @param dx Horizontal offset in pixels.
   * @param dy Vertical offset in pixels.
   */
  void moveObject(LVGLObject *obj, int dx, int dy);
  /** @brief Registers a widget with the object tracker and notifies the object model. */
  void addObject(LVGLObject *obj);
  /** @brief Deletes a widget and notifies the object model. Clears selection first. */
  void removeObject(LVGLObject *obj);

  /** @brief Connects the object tree model for insert/remove notifications. */
  void setObjectModel(LVGLObjectModel *objectModel);
  /** @brief Connects the property model for selection-triggered updates. */
  void setPropertyModel(LVGLPropertyModel *propertyModel);
  /**
   * @brief Sets the active LVGL screen and parent for new widget creation.
   * @param parent The lv_obj_t screen to activate via lv_scr_load().
   *
   * Called when switching between screens/tabs. Also stores the parent
   * so that newly drag-dropped widgets are created under it.
   */
  void setobjParent(lv_obj_t *parent);

 public slots:
  /** @brief Selects a widget on the canvas and notifies listeners. */
  void setSelectedObject(LVGLObject *obj);
  /** @brief Sets the canvas zoom level (percentage). */
  void setZoomLevel(int level);
  /** @brief Clears all widgets from the canvas. */
  void clear();
  /** @brief Enables or disables mouse input to the LVGL display (for run mode). */
  void setMouseEnable(bool enable);
  /** @brief Resizes the canvas to match a new display resolution. */
  void changeResolution(QSize size);

 signals:
  void objectSelected(LVGLObject *obj);
  void objectPositionChanged();
  void objectAdded(LVGLObject *obj);

 protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;

  void dropEvent(QDropEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dragEnterEvent(QDragEnterEvent *event) override;

 private slots:
  void update();

 private:
  void onSelectionChanged(LVGLObject *obj);
  void generateCodeObject(QTextStream &stream, LVGLObject *o);

  LVGLScene *m_scene;
  CanvasActions *m_canvasActions;
  SelectionManager *m_selectionManager;
  DragDropHandler *m_dragDropHandler;
  bool m_mouseEnabled;
  LVGLItem *m_item;
  LVGLObjectModel *m_objectModel;
  lv_obj_t *m_parent;
  LVGLPropertyModel *m_propertyModel;
  QThread *m_thread;
  QTimer *m_timer;
  bool m_dragging;
  QPoint m_dragStartPos;
  QPoint m_dragObjStartPos;
};

/**
 * @class LVGLKeyPressEventFilter
 * @brief Application-level event filter for keyboard shortcuts on the canvas.
 *
 * Installed on the QApplication but only active when the LVGLSimulator has
 * focus. Handles Delete (remove widget), arrow keys (nudge selected widget
 * by 1px), and Ctrl+C/X/V (clipboard operations).
 */
class LVGLKeyPressEventFilter : public QObject {
  Q_OBJECT
 public:
  explicit LVGLKeyPressEventFilter(LVGLSimulator *sim,
                                   QObject *parent = nullptr);

 protected:
  bool eventFilter(QObject *obj, QEvent *event) override;

 private:
  LVGLSimulator *m_sim;
};

#endif  // LVGLSIMULATOR_H
