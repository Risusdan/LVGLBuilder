#include "LVGLSimulator.h"

#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QGraphicsSceneMouseEvent>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QMimeData>
#include <QMouseEvent>
#include <QTextStream>
#include <QThread>

#include "CanvasActions.h"
#include "DragDropHandler.h"
#include "LVGLCore.h"
#include "LVGLFontData.h"
#include "LVGLItem.h"
#include "LVGLObject.h"
#include "LVGLObjectModel.h"
#include "LVGLProject.h"
#include "LVGLPropertyModel.h"
#include "SelectionManager.h"
#include "properties/LVGLPropertyGeometry.h"
#include "widgets/LVGLWidget.h"

#define IS_PAGE_OF_TABVIEW(o)                                    \
  ((o->widgetType() == LVGLWidget::Page) && (o->index() >= 0) && \
   o->parent() && (o->parent()->widgetType() == LVGLWidget::Tabview))

LVGLScene::LVGLScene(QObject *parent)
    : QGraphicsScene(parent), m_selected(nullptr), m_hoverObject(nullptr) {}

void LVGLScene::drawBackground(QPainter *painter, const QRectF &rect) {
  lvgl.poll();
  painter->save();
  painter->drawPixmap(rect, lvgl.framebuffer(), rect);
  if (m_selected != nullptr) {
    painter->setPen(Qt::red);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(m_selected->geometry());
  }
  if (m_hoverObject != nullptr) {
    painter->setPen(QPen(Qt::red, 2));
    painter->setBrush(Qt::NoBrush);
    if (m_hoverObject->widgetType() == LVGLWidget::TabView) {
      lv_obj_t *obj = m_hoverObject->obj();
      lv_obj_t *tab = lv_tabview_get_tab(obj, lv_tabview_get_tab_act(obj));
      painter->drawRect(lvgl.get_object_rect(tab));
    } else {
      painter->drawRect(m_hoverObject->geometry());
    }
  }
  painter->setPen(Qt::black);
  painter->setBrush(Qt::NoBrush);
  painter->drawRect(sceneRect());
  painter->restore();
}

LVGLObject *LVGLScene::hoverObject() const { return m_hoverObject; }

void LVGLScene::setHoverObject(LVGLObject *hoverObject) {
  m_hoverObject = hoverObject;
}

LVGLObject *LVGLScene::selected() const { return m_selected; }

void LVGLScene::setSelected(LVGLObject *selected) { m_selected = selected; }

LVGLSimulator::LVGLSimulator(QWidget *parent)
    : QGraphicsView(parent),
      m_scene(new LVGLScene),
      m_canvasActions(new CanvasActions(this)),
      m_selectionManager(new SelectionManager(this)),
      m_dragDropHandler(new DragDropHandler(m_canvasActions, m_selectionManager, this)),
      m_mouseEnabled(false),
      m_item(new LVGLItem),
      m_objectModel(nullptr),
      m_dragging(false) {
  connect(m_item, &LVGLItem::geometryChanged, this, &LVGLSimulator::update);
  connect(m_selectionManager, &SelectionManager::selectionChanged, this,
          &LVGLSimulator::onSelectionChanged);

  setAcceptDrops(true);

  m_scene->setSceneRect(0, 0, lvgl.width(), lvgl.height());
  setScene(m_scene);
  m_scene->addItem(m_item);

  m_thread = new QThread;
  m_timer = new QTimer;
  LVGLSimulator *self = this;
  connect(m_thread, &QThread::started, [=]() {
    qInfo() << "LVGL Thread started";
    connect(m_timer, &QTimer::timeout, self, &LVGLSimulator::update);
    m_timer->start(20);
  });
  m_thread->start();
}

LVGLSimulator::~LVGLSimulator() {
  m_timer->stop();
  m_thread->quit();
  m_thread->wait();
  delete m_timer;
  delete m_thread;
}

void LVGLSimulator::onSelectionChanged(LVGLObject *obj) {
  m_item->setObject(obj);
  if (m_objectModel) m_objectModel->setCurrentObject(obj);
  m_scene->setSelected(obj);
  emit objectSelected(obj);
  update();
}

void LVGLSimulator::setSelectedObject(LVGLObject *obj) {
  m_selectionManager->select(obj);
}

void LVGLSimulator::setZoomLevel(int level) {
  QTransform transform;
  const qreal zoom =
      (level < 0 ? -1.0 / (level - 1) : (level == 0 ? 1 : 1.0 + 0.5 * level));
  transform.scale(zoom, zoom);
  qDebug() << "Zoom:" << zoom;
  setTransform(transform);
}

void LVGLSimulator::clear() { m_selectionManager->clearSelection(); }

void LVGLSimulator::setMouseEnable(bool enable) { m_mouseEnabled = enable; }

void LVGLSimulator::changeResolution(QSize size) {
  m_scene->setSceneRect(0, 0, size.width(), size.height());
}

void LVGLSimulator::mousePressEvent(QMouseEvent *event) {
  const QPoint pos = mapToScene(event->position().toPoint()).toPoint();
  if (m_mouseEnabled) {
    lvgl.sendMouseEvent(pos.x(), pos.y(), event->buttons() & Qt::LeftButton);
  } else {
    if (event->button() == Qt::RightButton) {
      auto obj = m_selectionManager->selectObject(
          m_selectionManager->objectsUnderCoords(pos, true), false);
      QAction *sel = nullptr;
      QAction *scolor = nullptr;
      if (obj) {
        // LVGL right click menu
        bool locked = obj->isLocked();
        QMenu menu(obj->name(), this);
        QAction *lock = menu.addAction(locked ? "Unlock" : "Lock");
        QAction *remove = menu.addAction("Remove");
        QAction *mfore = menu.addAction("Move foreground");
        QAction *mback = menu.addAction("Move background");
        scolor = menu.addAction("Set screen color ...");
        sel = menu.exec(mapToGlobal(event->position().toPoint()));
        if ((sel == lock) && locked) {
          obj->setLocked(false);
        } else if ((sel == lock) && !locked) {
          obj->setLocked(true);
          if (obj == m_selectionManager->selectedObject())
            setSelectedObject(nullptr);
        } else if (sel == remove) {
          if (obj == m_selectionManager->selectedObject())
            setSelectedObject(nullptr);
          if (m_objectModel) m_objectModel->beginRemoveObject(obj);
          m_canvasActions->deleteObject(obj);
          if (m_objectModel) m_objectModel->endRemoveObject();
        } else if (sel == mfore) {
          m_canvasActions->moveForeground(obj);
        } else if (sel == mback) {
          m_canvasActions->moveBackground(obj);
        }
      } else {
        QMenu menu(this);
        scolor = menu.addAction("Set screen color ...");
        sel = menu.exec(mapToGlobal(event->position().toPoint()));
      }

      if (sel == scolor) {
        QColorDialog dialog(this);
        dialog.setCurrentColor(lvgl.screenColor());
        if (dialog.exec() == QDialog::Accepted)
          m_canvasActions->setScreenColor(dialog.selectedColor());
      }
    } else if (event->button() == Qt::LeftButton) {
      if (!m_item->isManipolating()) {
        auto obj = m_selectionManager->selectObject(
            m_selectionManager->objectsUnderCoords(pos, false), false);
        setSelectedObject(obj);
        if (obj && obj->isMovable()) {
          m_dragging = true;
          m_dragStartPos = pos;
          m_dragObjStartPos = obj->position();
        }
      }
    }
  }
  QGraphicsView::mousePressEvent(event);
}

void LVGLSimulator::mouseDoubleClickEvent(QMouseEvent *event) {
  if (!m_item->isManipolating() && !m_mouseEnabled) {
    const QPoint pos = mapToScene(event->position().toPoint()).toPoint();
    auto obj = m_selectionManager->selectObject(
        m_selectionManager->objectsUnderCoords(pos, false), true);
    if (obj == nullptr)
      setSelectedObject(nullptr);
    else if (!obj->isLocked())
      setSelectedObject(obj);
  }
  QGraphicsView::mouseDoubleClickEvent(event);
}

void LVGLSimulator::mouseReleaseEvent(QMouseEvent *event) {
  if (m_mouseEnabled) {
    const QPoint pos = mapToScene(event->position().toPoint()).toPoint();
    lvgl.sendMouseEvent(pos.x(), pos.y(), false);
  }
  m_dragging = false;
  QGraphicsView::mouseReleaseEvent(event);
}

void LVGLSimulator::mouseMoveEvent(QMouseEvent *event) {
  if (m_mouseEnabled) {
    const QPoint pos = mapToScene(event->position().toPoint()).toPoint();
    lvgl.sendMouseEvent(pos.x(), pos.y(), event->buttons() & Qt::LeftButton);
  } else if (m_dragging && m_selectionManager->selectedObject() &&
             m_selectionManager->selectedObject()->isMovable()) {
    const QPoint pos = mapToScene(event->position().toPoint()).toPoint();
    QPoint delta = pos - m_dragStartPos;
    QPoint newPos = m_dragObjStartPos + delta;
    m_canvasActions->moveObjectTo(m_selectionManager->selectedObject(), newPos,
                                  lvgl.size());
    m_item->updateGeometry();
    update();
    return;
  }
  QGraphicsView::mouseMoveEvent(event);
}

void LVGLSimulator::dropEvent(QDropEvent *event) {
  if (m_mouseEnabled) return;

  m_selectionManager->setHoverObject(nullptr);

  const QMimeData *mimeData = event->mimeData();
  if (mimeData->hasFormat("application/x-widget")) {
    LVGLWidgetCast cast;
    QByteArray encoded = mimeData->data("application/x-widget");
    QDataStream stream(&encoded, QIODevice::ReadOnly);
    stream >> cast.i;
    LVGLWidget *widgetClass = cast.ptr;

    QPoint pos = mapToScene(event->position().toPoint()).toPoint();
    LVGLObject *newObj = m_dragDropHandler->handleDrop(
        pos, widgetClass, m_parent, lvgl.size());

    if (newObj) {
      qDebug().noquote() << "Class:" << widgetClass->className()
                         << "Id:" << newObj->name();
      addObject(newObj);
    }
  }
}

void LVGLSimulator::dragMoveEvent(QDragMoveEvent *event) {
  if (m_mouseEnabled) return;

  const QPoint pos = mapToScene(event->position().toPoint()).toPoint();
  m_dragDropHandler->handleDragMove(pos);

  event->acceptProposedAction();
}

void LVGLSimulator::dragEnterEvent(QDragEnterEvent *event) {
  if (m_mouseEnabled) return;

  event->acceptProposedAction();
}

void LVGLSimulator::update() {
  m_scene->invalidate(m_scene->sceneRect(), QGraphicsScene::BackgroundLayer);
}

void LVGLSimulator::setObjectModel(LVGLObjectModel *objectModel) {
  m_objectModel = objectModel;
}

void LVGLSimulator::setPropertyModel(LVGLPropertyModel *propertyModel) {
  m_propertyModel = propertyModel;
}

void LVGLSimulator::setobjParent(lv_obj_t *parent) {
  m_parent = parent;
  lv_scr_load(m_parent);
}

LVGLItem *LVGLSimulator::item() const { return m_item; }

CanvasActions *LVGLSimulator::canvasActions() const { return m_canvasActions; }

SelectionManager *LVGLSimulator::selectionManager() const {
  return m_selectionManager;
}

void LVGLSimulator::moveObject(LVGLObject *obj, int dx, int dy) {
  m_canvasActions->moveObject(obj, dx, dy, lvgl.size());
}

void LVGLSimulator::addObject(LVGLObject *obj) {
  connect(obj, &LVGLObject::positionChanged, m_item, &LVGLItem::updateGeometry);

  // add to object viewer
  if (m_objectModel) m_objectModel->beginInsertObject(obj);

  // add object to interal list
  lvgl.addObject(obj);

  if (m_objectModel) m_objectModel->endInsertObject();

  setSelectedObject(obj);
  setFocus();

  emit objectAdded(obj);
}

void LVGLSimulator::removeObject(LVGLObject *obj) {
  setSelectedObject(nullptr);
  if (m_objectModel) m_objectModel->beginRemoveObject(obj);
  m_canvasActions->deleteObject(obj);
  if (m_objectModel) m_objectModel->endRemoveObject();
}

LVGLObject *LVGLSimulator::selectedObject() const {
  return m_selectionManager->selectedObject();
}

LVGLKeyPressEventFilter::LVGLKeyPressEventFilter(LVGLSimulator *sim,
                                                 QObject *parent)
    : QObject(parent), m_sim(sim) {}

bool LVGLKeyPressEventFilter::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() != QEvent::KeyRelease)
    return QObject::eventFilter(obj, event);

  LVGLSimulator *sim =
      qobject_cast<LVGLSimulator *>(QApplication::focusWidget());
  if (sim == nullptr) return QObject::eventFilter(obj, event);

  QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
  if (keyEvent->key() == Qt::Key_Delete) {
    LVGLObject *obj = m_sim->selectedObject();
    m_sim->removeObject(obj);
    return true;
  } else if (keyEvent->key() == Qt::Key_Left) {
    m_sim->moveObject(m_sim->selectedObject(), -1, 0);
    return true;
  } else if (keyEvent->key() == Qt::Key_Right) {
    m_sim->moveObject(m_sim->selectedObject(), 1, 0);
    return true;
  } else if (keyEvent->key() == Qt::Key_Up) {
    m_sim->moveObject(m_sim->selectedObject(), 0, -1);
    return true;
  } else if (keyEvent->key() == Qt::Key_Down) {
    m_sim->moveObject(m_sim->selectedObject(), 0, 1);
    return true;
  } else if (keyEvent->modifiers() & Qt::ControlModifier &&
             ((keyEvent->key() == Qt::Key_C) ||
              (keyEvent->key() == Qt::Key_X))) {
    LVGLObject *obj = m_sim->selectedObject();
    if (obj) {
      QJsonDocument doc(obj->toJson());
      QApplication::clipboard()->setText(doc.toJson(QJsonDocument::Compact));
      if (keyEvent->key() == Qt::Key_X) m_sim->removeObject(obj);
    }
    return true;
  } else if (keyEvent->modifiers() & Qt::ControlModifier &&
             keyEvent->key() == Qt::Key_V) {
    QClipboard *clipboard = QApplication::clipboard();
    if (clipboard->mimeData()->hasText()) {
      QJsonDocument doc = QJsonDocument::fromJson(clipboard->text().toLatin1());
      LVGLObject *newObj =
          LVGLObject::parse(doc.object(), m_sim->selectedObject());
      if (newObj == nullptr) return false;
      connect(newObj, &LVGLObject::positionChanged, m_sim->item(),
              &LVGLItem::updateGeometry);
      for (const LVGLObject *child : newObj->childs())
        connect(child, &LVGLObject::positionChanged, m_sim->item(),
                &LVGLItem::updateGeometry);
      m_sim->setSelectedObject(newObj);
      m_sim->setFocus();
    }
    return true;
  }
  return QObject::eventFilter(obj, event);
}
