#include "LVGLFontManager.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "LVGLFontData.h"

LVGLFontManager::LVGLFontManager(QObject *parent)
    : QObject(parent), m_defaultFont(nullptr) {
  FT_Init_FreeType(&m_ft);
}

LVGLFontManager::~LVGLFontManager() {
  FT_Done_FreeType(m_ft);
  qDeleteAll(m_fonts);
}

void LVGLFontManager::initBuiltinFonts() {
#if LV_FONT_ROBOTO_12
  m_fonts << new LVGLFontData("Roboto 12", "lv_font_roboto_12", 12,
                              &lv_font_roboto_12);
#endif
#if LV_FONT_ROBOTO_16
  m_fonts << new LVGLFontData("Roboto 16", "lv_font_roboto_16", 16,
                              &lv_font_roboto_16);
#endif
#if LV_FONT_ROBOTO_22
  m_fonts << new LVGLFontData("Roboto 22", "lv_font_roboto_22", 22,
                              &lv_font_roboto_22);
#endif
#if LV_FONT_ROBOTO_28
  m_fonts << new LVGLFontData("Roboto 28", "lv_font_roboto_28", 28,
                              &lv_font_roboto_28);
#endif
#if LV_FONT_UNSCII_8
  m_fonts << new LVGLFontData("UNSCII 8", "lv_font_unscii_8", 8,
                              &lv_font_unscii_8);
#endif

  // search for default font name
  for (const LVGLFontData *f : m_fonts) {
    if (f->font() == LV_FONT_DEFAULT) {
      m_defaultFont = f;
      break;
    }
  }
  Q_ASSERT(m_defaultFont != nullptr);
}

LVGLFontData *LVGLFontManager::addFont(const QString &fileName, uint8_t size) {
  LVGLFontData *font =
      LVGLFontData::parse(m_ft, fileName, size, 4, 0x0020, 0x007f);
  if (font) m_fonts << font;
  return font;
}

void LVGLFontManager::addFont(LVGLFontData *font) {
  if (font) m_fonts << font;
}

bool LVGLFontManager::removeFont(LVGLFontData *font) {
  return m_fonts.removeOne(font);
}

QStringList LVGLFontManager::fontNames() const {
  QStringList ret;
  for (const LVGLFontData *f : m_fonts) ret << f->name();
  return ret;
}

QStringList LVGLFontManager::fontCodeNames() const {
  QStringList ret;
  for (const LVGLFontData *f : m_fonts) ret << f->codeName();
  return ret;
}

const lv_font_t *LVGLFontManager::font(int index) const {
  if ((index > 0) && (index < m_fonts.size())) return m_fonts.at(index)->font();
  return m_defaultFont->font();
}

const lv_font_t *LVGLFontManager::font(const QString &name,
                                        Qt::CaseSensitivity cs) const {
  for (const LVGLFontData *font : m_fonts) {
    if (name.compare(font->name(), cs) == 0) return font->font();
  }
  return m_defaultFont->font();
}

int LVGLFontManager::indexOfFont(const lv_font_t *font) const {
  int index = 0;
  for (auto it = m_fonts.begin(); it != m_fonts.end(); ++it, ++index) {
    if ((*it)->font() == font) return index;
  }
  return -1;
}

QString LVGLFontManager::fontName(const lv_font_t *font) const {
  for (const LVGLFontData *f : m_fonts) {
    if (f->font() == font) return f->name();
  }
  return m_defaultFont->name();
}

QString LVGLFontManager::fontCodeName(const lv_font_t *font) const {
  for (const LVGLFontData *f : m_fonts) {
    if (f->font() == font) return f->codeName();
  }
  return m_defaultFont->codeName();
}

QList<const LVGLFontData *> LVGLFontManager::customFonts() const {
  QList<const LVGLFontData *> ret;
  for (const LVGLFontData *font : m_fonts) {
    if (font->isCustomFont()) ret << font;
  }
  return ret;
}

void LVGLFontManager::removeCustomFonts() {
  for (auto it = m_fonts.begin(); it != m_fonts.end();) {
    if ((*it)->isCustomFont()) {
      delete *it;
      it = m_fonts.erase(it);
    } else {
      ++it;
    }
  }
}

FT_LibraryRec_ *LVGLFontManager::ftLibrary() const { return m_ft; }
