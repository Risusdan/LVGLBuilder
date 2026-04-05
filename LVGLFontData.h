#ifndef LVGLFONTDATA_H
#define LVGLFONTDATA_H

/**
 * @file LVGLFontData.h
 * @brief Font asset wrapper — loads TTF via FreeType, serializes to JSON and C code.
 */

#include <lvgl/lvgl.h>
#include <QString>
#include <QJsonObject>

struct FT_LibraryRec_;

/**
 * @class LVGLFontData
 * @brief Represents a single font asset (built-in or user-loaded).
 *
 * Wraps an lv_font_t and provides serialization for project save/load (JSON)
 * and C code export. Custom fonts are loaded from TTF files via FreeType;
 * built-in fonts reference LVGL's compiled-in font data.
 */
class LVGLFontData
{
public:
	LVGLFontData(const QString &name, const QString &codeName, uint8_t size, const lv_font_t *font);
	~LVGLFontData();

	static LVGLFontData *parse(FT_LibraryRec_ *ft, const QString &fileName, uint8_t size, uint8_t bpp, uint32_t unicodeFirst, uint32_t unicodeLast);
	static LVGLFontData *parse(FT_LibraryRec_ *ft, QJsonObject object);

	const lv_font_t *font() const;
	QString name() const;

	QString codeName() const;
	bool saveAsCode(const QString &fileName) const;

	bool isCustomFont() const;

	QString fileName() const;

	QJsonObject toJson() const;

	uint8_t size() const;

private:
	lv_font_t *m_font;
	QString m_fileName;
	QString m_name;
	QString m_codeName;
	uint8_t m_size;
	bool m_customFont;

};

#endif // LVGLFONTDATA_H
