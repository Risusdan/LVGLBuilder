#include <QtTest>
#include <QImage>

#include "LVGLCore.h"
#include "LVGLImageManager.h"
#include "LVGLFontManager.h"
#include "LVGLFontData.h"

class TestAssetManagers : public QObject {
  Q_OBJECT

 private slots:
  void initTestCase() {
    Q_INIT_RESOURCE(res);
    lvgl.init(320, 480);
  }

  // NOTE: don't clear images/fonts in cleanup -- tests manage their own state

  // --- Image Manager tests ---

  void testRemoveDefaultImageClearsDefault() {
    LVGLImageManager mgr;

    // Create a small test image and add it
    QImage testImg(10, 10, QImage::Format_ARGB32);
    testImg.fill(Qt::red);
    LVGLImageData *img = mgr.addImage(testImg, "test_img");
    QVERIFY(img != nullptr);

    // It should be the default (first image added)
    QVERIFY(mgr.defaultImage() != nullptr);

    // Remove it -- default should become null, not dangling
    QVERIFY(mgr.removeImage(img));
    QVERIFY(mgr.defaultImage() == nullptr);
  }

  // --- Font Manager tests ---

  void testRemoveFontReturnsTrue() {
    LVGLFontManager *mgr = lvgl.fontManager();

    // Add a custom font via parse
    LVGLFontData *font = LVGLFontData::parse(
        mgr->ftLibrary(), QString(), 16, 4, 0x0020, 0x007f);

    // If parse returns null (no font file), skip this test gracefully
    if (!font) {
      QSKIP("Could not create test font -- no font file available");
    }

    mgr->addFont(font);
    int countBefore = mgr->fontNames().size();

    QVERIFY(mgr->removeFont(font));
    QCOMPARE(mgr->fontNames().size(), countBefore - 1);

    // Clean up -- font was removed from list but not deleted by removeFont
    delete font;
  }

  void testRemoveCustomFontsCleansUp() {
    LVGLFontManager *mgr = lvgl.fontManager();
    int builtinCount = mgr->fontNames().size();

    // removeCustomFonts should not affect builtin fonts
    mgr->removeCustomFonts();
    QCOMPARE(mgr->fontNames().size(), builtinCount);
  }
};

QTEST_MAIN(TestAssetManagers)
#include "tst_assetmanagers.moc"
