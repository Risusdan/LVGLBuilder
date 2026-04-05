#ifndef LVGLPROPERTYPRELOADSPINTIME_H
#define LVGLPROPERTYPRELOADSPINTIME_H

/**
 * @file LVGLPropertyPreloadSpinTime.h
 * @brief LVGLProperty subclass for lv_preload one full spin duration (ms).
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyPreloadSpinTime : public LVGLPropertyInt
{
public:
	LVGLPropertyPreloadSpinTime() : LVGLPropertyInt(0, UINT16_MAX) {}

	QString name() const { return "Spin time"; }

protected:
	int get(LVGLObject *obj) const { return lv_preload_get_spin_time(obj->obj()); }
	void set(LVGLObject *obj, int value) { lv_preload_set_spin_time(obj->obj(), static_cast<uint16_t>(value)); }
};

#endif // LVGLPROPERTYPRELOADSPINTIME_H
