#ifndef LVGLPROPERTYTABSCROLLBARS_H
#define LVGLPROPERTYTABSCROLLBARS_H

/**
 * @file LVGLPropertyTabScrollbars.h
 * @brief LVGLProperty subclass for lv_tabview per-page scrollbar mode.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyTabScrollbars : public LVGLPropertyEnum
{
public:
	LVGLPropertyTabScrollbars()
		: LVGLPropertyEnum(QStringList() << "Off" << "On" << "Drag" << "Auto")
		, m_values({"LV_SB_MODE_OFF", "LV_SB_MODE_ON", "LV_SB_MODE_DRAG", "LV_SB_MODE_AUTO"})
	{}

	QString name() const { return "Scrollbars"; }

protected:
	int get(LVGLObject *obj) const {
		lv_sb_mode_t mode = LV_SB_MODE_AUTO;
		for (uint16_t i = 0; i < lv_tabview_get_tab_count(obj->obj()); ++i) {
			lv_obj_t *page = lv_tabview_get_tab(obj->obj(), i);
			mode = lv_page_get_sb_mode(page);
		}
		return mode;
	}
	void set(LVGLObject *obj, int index) {
		for (uint16_t i = 0; i < lv_tabview_get_tab_count(obj->obj()); ++i) {
			lv_obj_t *page = lv_tabview_get_tab(obj->obj(), i);
			lv_page_set_sb_mode(page, index & 0xff);
		}
	}

	QStringList m_values;
};

#endif // LVGLPROPERTYTABSCROLLBARS_H
