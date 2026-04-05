#ifndef LVGLPROPERTYCONTAINERLAYOUT_H
#define LVGLPROPERTYCONTAINERLAYOUT_H

/**
 * @file LVGLPropertyContainerLayout.h
 * @brief LVGLProperty subclass for lv_cont child layout mode.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyContainerLayout : public LVGLPropertyEnum
{
public:
	LVGLPropertyContainerLayout()
		: LVGLPropertyEnum({"Off", "Center", "Left-justified column", "Centered column", "Right-justified column",
								  "Top-justified row", "Centered row", "Bottom-justified row", "Pretty", "Grid"})
		, m_values({"LV_LAYOUT_OFF", "LV_LAYOUT_CENTER", "LV_LAYOUT_COL_L", "LV_LAYOUT_COL_M", "LV_LAYOUT_COL_R",
						"LV_LAYOUT_ROW_T", "LV_LAYOUT_ROW_M", "LV_LAYOUT_ROW_B", "LV_LAYOUT_PRETTY", "LV_LAYOUT_GRID"})
	{}

	QString name() const { return "Layout"; }

	QStringList function(LVGLObject *obj) const {
		if (get(obj) != LV_BTN_STATE_REL)
			return QStringList() << QString("lv_cont_set_layout(%1, %2);").arg(obj->codeName()).arg(m_values.at(get(obj)));
		return QStringList();
	}

protected:
	int get(LVGLObject *obj) const { return lv_cont_get_layout(obj->obj()); }
	void set(LVGLObject *obj, int index) { lv_cont_set_layout(obj->obj(), index & 0xff); }

	QStringList m_values;
};

#endif // LVGLPROPERTYCONTAINERLAYOUT_H
