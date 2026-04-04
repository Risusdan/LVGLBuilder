#include <QtTest>

#include "LVGLCore.h"
#include "LVGLObject.h"
#include "SelectionManager.h"
#include "widgets/LVGLWidget.h"

class TestSelectionManager : public QObject {
  Q_OBJECT

 private slots:
  void initTestCase() {
    Q_INIT_RESOURCE(res);
    lvgl.init(320, 480);
  }

  void cleanup() { lvgl.removeAllObjects(); }

  void testInitialState() {
    SelectionManager mgr;
    QVERIFY(mgr.selectedObject() == nullptr);
    QVERIFY(mgr.hoverObject() == nullptr);
  }

  void testSelectEmitsSignal() {
    SelectionManager mgr;
    LVGLWidget *btnWidget =
        const_cast<LVGLWidget *>(lvgl.widget("lv_btn"));
    QVERIFY(btnWidget != nullptr);

    LVGLObject *obj =
        new LVGLObject(btnWidget, "test_btn", lv_scr_act());
    lvgl.addObject(obj);

    QSignalSpy spy(&mgr, &SelectionManager::selectionChanged);
    mgr.select(obj);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).value<LVGLObject *>(), obj);
    QCOMPARE(mgr.selectedObject(), obj);
  }

  void testSelectSameObjectNoSignal() {
    SelectionManager mgr;
    LVGLWidget *btnWidget =
        const_cast<LVGLWidget *>(lvgl.widget("lv_btn"));
    LVGLObject *obj =
        new LVGLObject(btnWidget, "test_btn", lv_scr_act());
    lvgl.addObject(obj);

    mgr.select(obj);

    QSignalSpy spy(&mgr, &SelectionManager::selectionChanged);
    mgr.select(obj);
    QCOMPARE(spy.count(), 0);
  }

  void testClearSelection() {
    SelectionManager mgr;
    LVGLWidget *btnWidget =
        const_cast<LVGLWidget *>(lvgl.widget("lv_btn"));
    LVGLObject *obj =
        new LVGLObject(btnWidget, "test_btn", lv_scr_act());
    lvgl.addObject(obj);

    mgr.select(obj);
    QCOMPARE(mgr.selectedObject(), obj);

    QSignalSpy spy(&mgr, &SelectionManager::selectionChanged);
    mgr.clearSelection();

    QCOMPARE(spy.count(), 1);
    QVERIFY(mgr.selectedObject() == nullptr);
  }

  void testHoverObject() {
    SelectionManager mgr;
    LVGLWidget *btnWidget =
        const_cast<LVGLWidget *>(lvgl.widget("lv_btn"));
    LVGLObject *obj =
        new LVGLObject(btnWidget, "test_btn", lv_scr_act());
    lvgl.addObject(obj);

    QVERIFY(mgr.hoverObject() == nullptr);
    mgr.setHoverObject(obj);
    QCOMPARE(mgr.hoverObject(), obj);
    mgr.setHoverObject(nullptr);
    QVERIFY(mgr.hoverObject() == nullptr);
  }

  void testObjectsUnderCoords() {
    SelectionManager mgr;
    LVGLWidget *btnWidget =
        const_cast<LVGLWidget *>(lvgl.widget("lv_btn"));
    LVGLObject *obj =
        new LVGLObject(btnWidget, "test_btn", lv_scr_act());
    lvgl.addObject(obj);

    // Position the button so it's at a known location
    obj->setPosition(QPoint(10, 10));
    obj->setWidth(100);
    obj->setHeight(50);
    // Force LVGL to process the layout
    lvgl.poll();

    QPoint inside(30, 30);
    QList<LVGLObject *> found = mgr.objectsUnderCoords(inside, false);
    QVERIFY(found.contains(obj));

    QPoint outside(300, 300);
    QList<LVGLObject *> notFound = mgr.objectsUnderCoords(outside, false);
    QVERIFY(!notFound.contains(obj));
  }

  void testSelectObjectFromCandidates() {
    SelectionManager mgr;
    LVGLWidget *btnWidget =
        const_cast<LVGLWidget *>(lvgl.widget("lv_btn"));

    LVGLObject *obj1 =
        new LVGLObject(btnWidget, "btn1", lv_scr_act());
    LVGLObject *obj2 =
        new LVGLObject(btnWidget, "btn2", lv_scr_act());
    lvgl.addObject(obj1);
    lvgl.addObject(obj2);

    QList<LVGLObject *> candidates;
    candidates << obj1 << obj2;

    // Non-double-click returns last
    LVGLObject *result = mgr.selectObject(candidates, false);
    QCOMPARE(result, obj2);

    // Empty list returns nullptr
    QList<LVGLObject *> empty;
    QVERIFY(mgr.selectObject(empty, false) == nullptr);
  }
};

QTEST_MAIN(TestSelectionManager)
#include "tst_selectionmanager.moc"
