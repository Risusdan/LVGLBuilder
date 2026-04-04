#ifndef LVGLPROPERTYSTATE_H
#define LVGLPROPERTYSTATE_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyState : public LVGLPropertyBool
{
public:
	QString name() const { return "State"; }

protected:
	bool get(LVGLObject *obj) const { return lv_sw_get_state(obj->obj()); }
	void set(LVGLObject *obj, bool state) {
		if (state)
			lv_sw_on(obj->obj(), LV_ANIM_OFF);
		else
			lv_sw_off(obj->obj(), LV_ANIM_OFF);
	}
};

#endif // LVGLPROPERTYSTATE_H
