#ifndef LVGLPROPERTYBUTTONSTATE_H
#define LVGLPROPERTYBUTTONSTATE_H

#include "LVGLProperty.h"
#include "LVGLObject.h"
#include "lvgl/lvgl.h"

class LVGLPropertyButtonState : public LVGLPropertyEnum
{
public:
	LVGLPropertyButtonState()
		: LVGLPropertyEnum({"Released", "Pressed", "Toggled released", "Toggled pressed", "Inactive"})
		, m_values({"LV_BTN_STATE_REL", "LV_BTN_STATE_PR", "LV_BTN_STATE_TGL_REL", "LV_BTN_STATE_TGL_PR", "LV_BTN_STATE_INA"})
	{}

	QString name() const { return "State"; }

	QStringList function(LVGLObject *obj) const {
		if (get(obj) != LV_BTN_STATE_REL)
			return QStringList() << QString("lv_btn_set_state(%1, %2);").arg(obj->codeName()).arg(m_values.at(get(obj)));
		return QStringList();
	}

protected:
	int get(LVGLObject *obj) const { return lv_btn_get_state(obj->obj()); }
	void set(LVGLObject *obj, int index) { lv_btn_set_state(obj->obj(), index & 0xff); }

	QStringList m_values;
};

#endif // LVGLPROPERTYBUTTONSTATE_H
