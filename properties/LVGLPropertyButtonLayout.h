#ifndef LVGLPROPERTYBUTTONLAYOUT_H
#define LVGLPROPERTYBUTTONLAYOUT_H

#include "LVGLProperty.h"
#include "LVGLObject.h"
#include "lvgl/lvgl.h"

class LVGLPropertyButtonLayout : public LVGLPropertyEnum
{
public:
	LVGLPropertyButtonLayout()
		: LVGLPropertyEnum({"Off", "Center", "Column left align", "Column middle align", "Column right align",
								 "Row top align", "Row middle align", "Row bottom align", "Pretty", "Grid"})
		, m_values({"LV_LAYOUT_OFF", "LV_LAYOUT_CENTER", "LV_LAYOUT_COL_L", "LV_LAYOUT_COL_M", "LV_LAYOUT_COL_R",
						"LV_LAYOUT_ROW_T", "LV_LAYOUT_ROW_M", "LV_LAYOUT_ROW_B", "LV_LAYOUT_PRETTY", "LV_LAYOUT_GRID"})
	{}

	QString name() const { return "Layout"; }

	QStringList function(LVGLObject *obj) const {
		if (get(obj) != LV_LAYOUT_CENTER)
			return QStringList() << QString("lv_btn_set_layout(%1, %2);").arg(obj->codeName()).arg(m_values.at(get(obj)));
		return QStringList();
	}

protected:
	int get(LVGLObject *obj) const { return lv_btn_get_layout(obj->obj()); }
	void set(LVGLObject *obj, int index) { lv_btn_set_layout(obj->obj(), index & 0xff); }

	QStringList m_values;
};

#endif // LVGLPROPERTYBUTTONLAYOUT_H
