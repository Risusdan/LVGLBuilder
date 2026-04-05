#ifndef LVGLPROPERTYTABCURRENT_H
#define LVGLPROPERTYTABCURRENT_H

/**
 * @file LVGLPropertyTabCurrent.h
 * @brief LVGLProperty subclass for lv_tabview active tab index.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyTabCurrent : public LVGLPropertyInt
{
public:
	LVGLPropertyTabCurrent() : LVGLPropertyInt(0, UINT16_MAX) {}

	QString name() const { return "Current tab"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_tabview_set_tab_act(%1, %2, LV_ANIM_OFF);").arg(obj->codeName()).arg(get(obj));
	}

protected:
	int get(LVGLObject *obj) const { return lv_tabview_get_tab_act(obj->obj()); }
	void set(LVGLObject *obj, int value) { lv_tabview_set_tab_act(obj->obj(), static_cast<uint16_t>(value), LV_ANIM_OFF); }
};

#endif // LVGLPROPERTYTABCURRENT_H
