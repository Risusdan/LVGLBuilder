#ifndef LVGLFONTMANAGER_H
#define LVGLFONTMANAGER_H

#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>

#include <lvgl/lvgl.h>

class LVGLFontData;
struct FT_LibraryRec_;

class LVGLFontManager : public QObject {
  Q_OBJECT
 public:
  explicit LVGLFontManager(QObject *parent = nullptr);
  ~LVGLFontManager();

  void initBuiltinFonts();

  LVGLFontData *addFont(const QString &fileName, uint8_t size);
  void addFont(LVGLFontData *font);
  bool removeFont(LVGLFontData *font);

  QStringList fontNames() const;
  QStringList fontCodeNames() const;
  const lv_font_t *font(int index) const;
  const lv_font_t *font(const QString &name,
                        Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
  int indexOfFont(const lv_font_t *font) const;
  QString fontName(const lv_font_t *font) const;
  QString fontCodeName(const lv_font_t *font) const;
  QList<const LVGLFontData *> customFonts() const;
  void removeCustomFonts();

  FT_LibraryRec_ *ftLibrary() const;

 private:
  QList<LVGLFontData *> m_fonts;
  const LVGLFontData *m_defaultFont;
  FT_LibraryRec_ *m_ft;
};

#endif  // LVGLFONTMANAGER_H
