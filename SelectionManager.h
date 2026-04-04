#ifndef SELECTIONMANAGER_H
#define SELECTIONMANAGER_H

#include <QList>
#include <QObject>
#include <QPoint>

class LVGLObject;

class SelectionManager : public QObject {
  Q_OBJECT
 public:
  explicit SelectionManager(QObject *parent = nullptr);

  LVGLObject *selectedObject() const;

  LVGLObject *hoverObject() const;
  void setHoverObject(LVGLObject *obj);

  LVGLObject *selectObject(const QList<LVGLObject *> &candidates,
                            bool doubleClick) const;
  QList<LVGLObject *> objectsUnderCoords(QPoint pos,
                                         bool includeLocked) const;

 public slots:
  void select(LVGLObject *obj);
  void clearSelection();

 signals:
  void selectionChanged(LVGLObject *obj);

 private:
  LVGLObject *m_selectedObject;
  LVGLObject *m_hoverObject;
};

#endif  // SELECTIONMANAGER_H
