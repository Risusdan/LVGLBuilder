#include "LVGLWidgetRegistry.h"

#include "widgets/LVGLWidget.h"

LVGLWidgetRegistry::LVGLWidgetRegistry(QObject *parent) : QObject(parent) {}

LVGLWidgetRegistry::~LVGLWidgetRegistry() { qDeleteAll(m_widgets); }

void LVGLWidgetRegistry::addWidget(LVGLWidget *w) {
  m_widgets.insert(w->className(), w);
}

QList<const LVGLWidget *> LVGLWidgetRegistry::widgets() const {
  return m_widgets.values();
}

const LVGLWidget *LVGLWidgetRegistry::widget(const QString &name) const {
  if (m_widgets.contains(name)) return m_widgets[name];
  return nullptr;
}
