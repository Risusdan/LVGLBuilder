#ifndef LVGLPROPERTYDDLISTANIMATIONTIME_H
#define LVGLPROPERTYDDLISTANIMATIONTIME_H

/**
 * @file LVGLPropertyDDListAnimationTime.h
 * @brief LVGLProperty subclass for lv_ddlist open/close animation duration.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyDDListAnimationTime : public LVGLPropertyInt
{
public:
	LVGLPropertyDDListAnimationTime() : LVGLPropertyInt(0, UINT16_MAX, " ms") {}

	QString name() const { return "Animation time"; }

protected:
	int get(LVGLObject *obj) const { return lv_ddlist_get_anim_time(obj->obj()); }
	void set(LVGLObject *obj, int value) { lv_ddlist_set_anim_time(obj->obj(), static_cast<uint16_t>(value)); }
};

#endif // LVGLPROPERTYDDLISTANIMATIONTIME_H
