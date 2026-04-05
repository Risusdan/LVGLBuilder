#ifndef LVGLPROPERTYLABELLONGMODE_H
#define LVGLPROPERTYLABELLONGMODE_H

/**
 * @file LVGLPropertyLabelLongMode.h
 * @brief LVGLProperty subclass for lv_label long-text overflow mode.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyLabelLongMode : public LVGLPropertyEnum
{
public:
	LVGLPropertyLabelLongMode()
		: LVGLPropertyEnum(QStringList() << "Expand" << "Break" << "Dot" << "Scroll" << "Circular scroll" << "Corp")
		, m_values({"LV_LABEL_LONG_EXPAND", "LV_LABEL_LONG_BREAK", "LV_LABEL_LONG_DOT", "LV_LABEL_LONG_SROLL",
						"LV_LABEL_LONG_SROLL_CIRC", "LV_LABEL_LONG_CROP"})
	{}

	QString name() const { return "Long mode"; }

	QStringList function(LVGLObject *obj) const {
		if (get(obj) == LV_LABEL_LONG_EXPAND) return QStringList();
		return QStringList() << QString("lv_label_set_long_mode(%1, %2);").arg(obj->codeName()).arg(m_values.at(get(obj)));
	}

protected:
	int get(LVGLObject *obj) const { return lv_label_get_long_mode(obj->obj()); }
	void set(LVGLObject *obj, int index) { lv_label_set_long_mode(obj->obj(), index & 0xff); }

	QStringList m_values;
};

#endif // LVGLPROPERTYLABELLONGMODE_H
