#include <QtTest>

#include "CanvasActions.h"
#include "LVGLCore.h"
#include "LVGLObject.h"
#include "widgets/LVGLWidget.h"

class TestCanvasActions : public QObject {
  Q_OBJECT

 private slots:
  void initTestCase() {
    Q_INIT_RESOURCE(res);
    lvgl.init(320, 480);
  }

  void cleanup() { lvgl.removeAllObjects(); }

  void testCreateObject() {
    CanvasActions actions;
    LVGLWidget *btnWidget =
        const_cast<LVGLWidget *>(lvgl.widget("lv_btn"));
    QVERIFY(btnWidget != nullptr);

    LVGLObject *obj = actions.createObject(
        btnWidget, nullptr, lv_scr_act(), QPoint(50, 50), QSize(320, 480));
    QVERIFY(obj != nullptr);
    QCOMPARE(obj->widgetClass()->className(), QString("lv_btn"));

    lvgl.addObject(obj);
  }

  void testMoveObject() {
    CanvasActions actions;
    LVGLWidget *btnWidget =
        const_cast<LVGLWidget *>(lvgl.widget("lv_btn"));
    LVGLObject *obj = actions.createObject(
        btnWidget, nullptr, lv_scr_act(), QPoint(50, 50), QSize(320, 480));
    lvgl.addObject(obj);

    actions.moveObject(obj, 10, 0, QSize(320, 480));
    QCOMPARE(obj->x(), 60);

    actions.moveObject(obj, 0, 20, QSize(320, 480));
    QCOMPARE(obj->y(), 70);
  }

  void testMoveObjectBoundsCheck() {
    CanvasActions actions;
    LVGLWidget *btnWidget =
        const_cast<LVGLWidget *>(lvgl.widget("lv_btn"));
    LVGLObject *obj = actions.createObject(
        btnWidget, nullptr, lv_scr_act(), QPoint(10, 10), QSize(320, 480));
    lvgl.addObject(obj);

    actions.moveObject(obj, 5000, 0, QSize(320, 480));
    QVERIFY(obj->x() <= 320 - obj->width());

    actions.moveObject(obj, 0, 5000, QSize(320, 480));
    QVERIFY(obj->y() <= 480 - obj->height());
  }

  void testMoveObjectTo() {
    CanvasActions actions;
    LVGLWidget *btnWidget =
        const_cast<LVGLWidget *>(lvgl.widget("lv_btn"));
    LVGLObject *obj = actions.createObject(
        btnWidget, nullptr, lv_scr_act(), QPoint(10, 10), QSize(320, 480));
    lvgl.addObject(obj);

    actions.moveObjectTo(obj, QPoint(100, 200), QSize(320, 480));
    QCOMPARE(obj->x(), 100);
    QCOMPARE(obj->y(), 200);
  }

  void testDeleteObject() {
    CanvasActions actions;
    LVGLWidget *btnWidget =
        const_cast<LVGLWidget *>(lvgl.widget("lv_btn"));
    LVGLObject *obj = actions.createObject(
        btnWidget, nullptr, lv_scr_act(), QPoint(50, 50), QSize(320, 480));
    lvgl.addObject(obj);
    QCOMPARE(lvgl.allObjects().size(), 1);

    actions.deleteObject(obj);
    QCOMPARE(lvgl.allObjects().size(), 0);
  }
};

QTEST_MAIN(TestCanvasActions)
#include "tst_canvasactions.moc"
