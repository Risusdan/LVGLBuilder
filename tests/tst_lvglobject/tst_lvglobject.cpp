#include <QtTest>
#include <QJsonArray>

#include "LVGLCore.h"
#include "LVGLObject.h"

class TestLVGLObject : public QObject {
  Q_OBJECT

 private slots:
  void initTestCase() {
    Q_INIT_RESOURCE(res);
    lvgl.init(320, 480);
  }

  void cleanup() { lvgl.removeAllObjects(); }

  void testCodeNameSanitizesSpaces();
  void testCodeNameSanitizesSpecialChars();
  void testCodeNameSanitizesLeadingDigit();
  void testJsonStylesShadowWidthUsesCorrectField();
};

void TestLVGLObject::testCodeNameSanitizesSpaces() {
    const LVGLWidget *btnWidget = lvgl.widget("lv_btn");
    auto *obj = new LVGLObject(btnWidget, "My Button", lv_scr_act());
    lvgl.addObject(obj);
    QCOMPARE(obj->codeName(), QString("my_button"));
}

void TestLVGLObject::testCodeNameSanitizesSpecialChars() {
    const LVGLWidget *btnWidget = lvgl.widget("lv_btn");

    auto *obj1 = new LVGLObject(btnWidget, "button-1", lv_scr_act());
    lvgl.addObject(obj1);
    QCOMPARE(obj1->codeName(), QString("button_1"));

    auto *obj2 = new LVGLObject(btnWidget, "btn@home", lv_scr_act());
    lvgl.addObject(obj2);
    QCOMPARE(obj2->codeName(), QString("btn_home"));

    auto *obj3 = new LVGLObject(btnWidget, "test#1", lv_scr_act());
    lvgl.addObject(obj3);
    QCOMPARE(obj3->codeName(), QString("test_1"));

    auto *obj4 = new LVGLObject(btnWidget, "btn(click)", lv_scr_act());
    lvgl.addObject(obj4);
    QCOMPARE(obj4->codeName(), QString("btn_click_"));
}

void TestLVGLObject::testCodeNameSanitizesLeadingDigit() {
    const LVGLWidget *btnWidget = lvgl.widget("lv_btn");
    auto *obj = new LVGLObject(btnWidget, "1button", lv_scr_act());
    lvgl.addObject(obj);
    QVERIFY(!obj->codeName().isEmpty());
    QVERIFY(!obj->codeName().at(0).isDigit());
}

void TestLVGLObject::testJsonStylesShadowWidthUsesCorrectField() {
    const LVGLWidget *btnWidget = lvgl.widget("lv_btn");
    QVERIFY(btnWidget != nullptr);
    auto *obj = new LVGLObject(btnWidget, "shadow_test", lv_scr_act());
    lvgl.addObject(obj);

    // Get style and set shadow.width to a known value, border.width to a different one
    lv_style_t *s = obj->style(0); // LV_BTN_STYLE_REL
    s->body.shadow.width = 10;
    s->body.border.width = 99; // sentinel — if serialized, this is the bug
    btnWidget->setStyle(obj->obj(), 0, s);

    // Serialize
    QJsonArray styles = obj->jsonStyles();
    QVERIFY(styles.size() > 0);

    // Find the shadow object in the first style
    QJsonObject firstStyle = styles[0].toObject();
    QJsonObject body = firstStyle["body"].toObject();
    QJsonObject shadow = body["shadow"].toObject();

    // The serialized width must be 10 (shadow.width), NOT 99 (border.width)
    QVERIFY(shadow.contains("width"));
    QCOMPARE(shadow["width"].toInt(), 10);
}

QTEST_MAIN(TestLVGLObject)
#include "tst_lvglobject.moc"
