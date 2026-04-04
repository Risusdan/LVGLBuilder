#ifndef LVGLPROPERTYTACURSORTYPE_H
#define LVGLPROPERTYTACURSORTYPE_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyTACursorType : public LVGLPropertyEnum
{
public:
	LVGLPropertyTACursorType()
		: LVGLPropertyEnum(QStringList() << "None" << "Line" << "Block" << "Outline" << "Underline")
		, m_values({"LV_CURSOR_NONE", "LV_CURSOR_LINE", "LV_CURSOR_BLOCK", "LV_CURSOR_OUTLINE", "LV_CURSOR_UNDERLINE"})
	{}

	QString name() const { return "Cursor type"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_ta_set_cursor_type(%1, %2);").arg(obj->codeName()).arg(m_values.at(get(obj)));
	}

protected:
	int get(LVGLObject *obj) const { return lv_ta_get_cursor_type(obj->obj()) & 0x7f; }
	void set(LVGLObject *obj, int index) { lv_ta_set_cursor_type(obj->obj(), index & 0x7f); }

	QStringList m_values;
};

#endif // LVGLPROPERTYTACURSORTYPE_H
