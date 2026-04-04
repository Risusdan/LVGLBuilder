#ifndef LVGLWIDGETREGISTRY_H
#define LVGLWIDGETREGISTRY_H

#include <QHash>
#include <QList>
#include <QObject>
#include <QString>

class LVGLWidget;

class LVGLWidgetRegistry : public QObject {
  Q_OBJECT
 public:
  explicit LVGLWidgetRegistry(QObject *parent = nullptr);
  ~LVGLWidgetRegistry();

  void addWidget(LVGLWidget *w);
  QList<const LVGLWidget *> widgets() const;
  const LVGLWidget *widget(const QString &name) const;

 private:
  QHash<QString, const LVGLWidget *> m_widgets;
};

#endif  // LVGLWIDGETREGISTRY_H
