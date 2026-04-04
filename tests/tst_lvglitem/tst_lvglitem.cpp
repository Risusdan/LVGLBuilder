#include <QtTest>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include "LVGLCore.h"
#include "LVGLItem.h"
#include "LVGLObject.h"

class TestLVGLItem : public QObject {
  Q_OBJECT

 private slots:
  void initTestCase() {
    Q_INIT_RESOURCE(res);
    lvgl.init(320, 480);
  }

  void cleanup() { lvgl.removeAllObjects(); }

  void testSetObjectUpdatesPosition() {
    const LVGLWidget *btnWidget = lvgl.widget("lv_btn");
    auto *obj = new LVGLObject(btnWidget, "test_btn", lv_scr_act());
    lvgl.addObject(obj);
    obj->setPosition(QPoint(50, 100));

    QGraphicsScene scene;
    LVGLItem item;
    scene.addItem(&item);
    item.setObject(obj);

    QCOMPARE(item.pos().toPoint(), obj->absolutePosition());
  }

  void testBoundingRectMatchesObjectSize() {
    const LVGLWidget *btnWidget = lvgl.widget("lv_btn");
    auto *obj = new LVGLObject(btnWidget, "test_btn2", lv_scr_act());
    lvgl.addObject(obj);
    obj->setWidth(150);
    obj->setHeight(60);

    QGraphicsScene scene;
    LVGLItem item;
    scene.addItem(&item);
    item.setObject(obj);

    QRectF br = item.boundingRect();
    QCOMPARE(br.width(), 150.0);
    QCOMPARE(br.height(), 60.0);
  }

  void testNullObjectBoundingRect() {
    LVGLItem item;
    QRectF br = item.boundingRect();
    QVERIFY(br.isEmpty());
  }

  void testIsManipulatingInitiallyFalse() {
    LVGLItem item;
    QVERIFY(!item.isManipolating());
  }
};

QTEST_MAIN(TestLVGLItem)
#include "tst_lvglitem.moc"
