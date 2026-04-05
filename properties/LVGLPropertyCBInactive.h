#ifndef LVGLPROPERTYCBINACTIVE_H
#define LVGLPROPERTYCBINACTIVE_H

/**
 * @file LVGLPropertyCBInactive.h
 * @brief LVGLProperty subclass for lv_cb inactive (disabled) state.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyCBInactive : public LVGLPropertyBool
{
public:
	QString name() const { return "Inactive"; }

protected:
	bool get(LVGLObject *obj) const { return lv_cb_is_inactive(obj->obj()); }
	void set(LVGLObject *obj, bool boolean) { lv_btn_set_state(obj->obj(), boolean ? LV_BTN_STATE_INA : LV_BTN_STATE_TGL_REL); }
};

#endif // LVGLPROPERTYCBINACTIVE_H
