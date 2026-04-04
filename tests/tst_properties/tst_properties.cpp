#include <QtTest>

#include "LVGLCore.h"
#include "LVGLObject.h"
#include "LVGLProperty.h"

class TestProperties : public QObject {
  Q_OBJECT

 private slots:
  void initTestCase() {
    Q_INIT_RESOURCE(res);
    lvgl.init(320, 480);
  }

  void cleanup() { lvgl.removeAllObjects(); }

  void testGeometryProperty() {
    const LVGLWidget *btnWidget = lvgl.widget("lv_btn");
    QVERIFY(btnWidget != nullptr);

    lv_obj_t *screen = lv_scr_act();
    LVGLObject *btn = new LVGLObject(btnWidget, "geo_btn", screen);
    lvgl.addObject(btn);

    btn->setGeometry(QRect(15, 25, 120, 40));
    QCOMPARE(btn->x(), 15);
    QCOMPARE(btn->y(), 25);
    QCOMPARE(btn->width(), 120);
    QCOMPARE(btn->height(), 40);

    LVGLProperty *geoProp = btnWidget->property("Geometry");
    QVERIFY(geoProp != nullptr);
    QVERIFY(geoProp->count() >= 4);
  }

  void testPropertyJsonSerialization() {
    const LVGLWidget *labelWidget = lvgl.widget("lv_label");
    QVERIFY(labelWidget != nullptr);

    lv_obj_t *screen = lv_scr_act();
    LVGLObject *label = new LVGLObject(labelWidget, "json_label", screen);
    label->setGeometry(QRect(5, 10, 200, 30));
    lvgl.addObject(label);

    QJsonObject json = label->toJson();
    QVERIFY(json.contains("widget"));
    QVERIFY(json.contains("class"));
    QVERIFY(json.contains("name"));
    QCOMPARE(json["widget"].toString(), QString("Label"));
    QCOMPARE(json["class"].toString(), QString("lv_label"));
    QCOMPARE(json["name"].toString(), QString("json_label"));
  }

  void testWidgetRegistryLookup() {
    QList<const LVGLWidget *> widgets = lvgl.widgets();
    QVERIFY(widgets.size() >= 26);

    // Lookup by className (lv_btn, lv_label, lv_slider)
    QVERIFY(lvgl.widget("lv_btn") != nullptr);
    QVERIFY(lvgl.widget("lv_label") != nullptr);
    QVERIFY(lvgl.widget("lv_slider") != nullptr);
    QVERIFY(lvgl.widget("Nonexistent") == nullptr);
  }

  void testObjectAddRemove() {
    const LVGLWidget *btnWidget = lvgl.widget("lv_btn");
    lv_obj_t *screen = lv_scr_act();

    LVGLObject *btn = new LVGLObject(btnWidget, "add_rm_btn", screen);
    lvgl.addObject(btn);
    QCOMPARE(lvgl.allObjects().size(), 1);

    lvgl.removeObject(btn);
    QCOMPARE(lvgl.allObjects().size(), 0);
  }
};

QTEST_MAIN(TestProperties)
#include "tst_properties.moc"
