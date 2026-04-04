#include <QApplication>
#include <QStyleHints>

#include "LVGLSimulator.h"
#include "MainWindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  a.styleHints()->setColorScheme(Qt::ColorScheme::Light);

  MainWindow w;
  LVGLKeyPressEventFilter filter(w.simulator(), &a);
  a.installEventFilter(&filter);

  w.showMaximized();

  return a.exec();
}
