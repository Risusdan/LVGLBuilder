#ifndef LVGLPROPERTYKBMODE_H
#define LVGLPROPERTYKBMODE_H

/**
 * @file LVGLPropertyKBMode.h
 * @brief LVGLProperty subclass for lv_kb keyboard mode (text/number/uppercase).
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyKBMode : public LVGLPropertyEnum
{
public:
	LVGLPropertyKBMode()
		: LVGLPropertyEnum({"Text", "Number", "Text Upper"})
		, m_values({"LV_KB_MODE_TEXT", "LV_KB_MODE_NUM", "LV_KB_MODE_TEXT_UPPER"})
	{}

	QString name() const { return "Mode"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_kb_set_mode(%1, %2);").arg(obj->codeName()).arg(m_values.at(get(obj)));
	}

protected:
	int get(LVGLObject *obj) const { return lv_kb_get_mode(obj->obj()); }
	void set(LVGLObject *obj, int index) { lv_kb_set_mode(obj->obj(), index & 0xff); }

	QStringList m_values;
};

#endif // LVGLPROPERTYKBMODE_H
