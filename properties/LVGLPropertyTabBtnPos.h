#ifndef LVGLPROPERTYTABBTNPOS_H
#define LVGLPROPERTYTABBTNPOS_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyTabBtnPos : public LVGLPropertyEnum
{
public:
	LVGLPropertyTabBtnPos()
		: LVGLPropertyEnum({"Top", "Bottom", "Left", "Right"})
		, m_values({"LV_TABVIEW_BTNS_POS_TOP", "LV_TABVIEW_BTNS_POS_BOTTOM", "LV_TABVIEW_BTNS_POS_LEFT", "LV_TABVIEW_BTNS_POS_RIGHT"})
	{}

	QString name() const { return "Button position"; }

	QStringList function(LVGLObject *obj) const {
		if (get(obj) != LV_TABVIEW_BTNS_POS_TOP)
			return QStringList() << QString("lv_tabview_set_btns_pos(%1, %2);").arg(obj->codeName()).arg(m_values.at(get(obj)));
		return QStringList();
	}

protected:
	int get(LVGLObject *obj) const { return lv_tabview_get_btns_pos(obj->obj()); }
	void set(LVGLObject *obj, int index) { lv_tabview_set_btns_pos(obj->obj(), index & 0xff); }

	QStringList m_values;
};

#endif // LVGLPROPERTYTABBTNPOS_H
