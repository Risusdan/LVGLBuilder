#include <QtTest>
#include <QJsonArray>
#include <QRegularExpression>

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
  void testJsonStylesLineOpaUsesCorrectField();
  void testCodeStyleLineSpaceComparison();
  void testRemoveChildClearsParentPointer();
  void testStyleCodeNameSanitizesSpecialChars();
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

void TestLVGLObject::testJsonStylesLineOpaUsesCorrectField() {
    const LVGLWidget *lineWidget = lvgl.widget("lv_line");
    QVERIFY(lineWidget != nullptr);
    auto *obj = new LVGLObject(lineWidget, "line_opa_test", lv_scr_act());
    lvgl.addObject(obj);

    lv_style_t *s = obj->style(0);
    s->line.opa = 128;
    s->image.opa = 77; // sentinel — if serialized, this is the bug
    lineWidget->setStyle(obj->obj(), 0, s);

    QJsonArray styles = obj->jsonStyles();
    QVERIFY(styles.size() > 0);

    QJsonObject firstStyle = styles[0].toObject();
    QJsonObject line = firstStyle["line"].toObject();

    QVERIFY(line.contains("opa"));
    QCOMPARE(line["opa"].toInt(), 128);
}

void TestLVGLObject::testCodeStyleLineSpaceComparison() {
    // Use dropdown list: it has Text editable style at index 0 AND
    // a non-null defaultStyle (lv_style_pretty) with line_space=2, letter_space=0.
    const LVGLWidget *ddWidget = lvgl.widget("lv_ddlist");
    QVERIFY(ddWidget != nullptr);
    auto *obj = new LVGLObject(ddWidget, "linespace_test", lv_scr_act());
    lvgl.addObject(obj);

    // Get style for LV_DDLIST_STYLE_BG (index 0)
    lv_style_t *s = obj->style(0);

    // Set line_space to a non-default value so it SHOULD appear in generated code
    s->text.line_space = 42;
    // Set letter_space to the default line_space value (2) so the buggy comparison
    // (defaultStyle->text.line_space != objStyle.text.letter_space) evaluates to
    // (2 != 2) = false, causing line_space to be wrongly skipped.
    s->text.letter_space = 2;
    ddWidget->setStyle(obj->obj(), 0, s);

    QStringList code = obj->codeStyle("mystyle", 0);

    // line_space was changed to 42, so it must appear in generated code
    bool hasLineSpace = false;
    for (const QString &line : code) {
        if (line.contains("text.line_space"))
            hasLineSpace = true;
    }
    QVERIFY2(hasLineSpace, "codeStyle() should emit line_space when it differs from default");
}

void TestLVGLObject::testRemoveChildClearsParentPointer() {
    const LVGLWidget *btnWidget = lvgl.widget("lv_btn");
    auto *parent = new LVGLObject(btnWidget, "parent_btn", lv_scr_act());
    lvgl.addObject(parent);

    // Use the constructor that takes LVGLObject* parent — this adds child to parent's child list
    auto *child = new LVGLObject(btnWidget, "child_btn", parent);
    lvgl.addObject(child);

    QCOMPARE(parent->childs().size(), 1);
    QCOMPARE(child->parent(), parent);

    parent->removeChild(child);

    QCOMPARE(parent->childs().size(), 0);
    // After removal, child's parent pointer should be null
    QVERIFY(child->parent() == nullptr);
}

void TestLVGLObject::testStyleCodeNameSanitizesSpecialChars() {
    const LVGLWidget *btnWidget = lvgl.widget("lv_btn");
    auto *obj = new LVGLObject(btnWidget, "btn-test#1", lv_scr_act());
    lvgl.addObject(obj);

    QString styleName = obj->styleCodeName(0);
    // Must be a valid C identifier
    QRegularExpression validCIdent("^[a-zA-Z_][a-zA-Z0-9_]*$");
    QVERIFY2(validCIdent.match(styleName).hasMatch(),
             qPrintable("styleCodeName produced invalid C identifier: " + styleName));
}

QTEST_MAIN(TestLVGLObject)
#include "tst_lvglobject.moc"
