#ifndef LVGLPROPERTYPRELOADARCLEN_H
#define LVGLPROPERTYPRELOADARCLEN_H

/**
 * @file LVGLPropertyPreloadArcLen.h
 * @brief LVGLProperty subclass for lv_preload spinning arc length (degrees).
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyPreloadArcLen : public LVGLPropertyInt
{
public:
	LVGLPropertyPreloadArcLen() : LVGLPropertyInt(0, 360) {}

	QString name() const { return "Arc length"; }

protected:
	int get(LVGLObject *obj) const { return lv_preload_get_arc_length(obj->obj()); }
	void set(LVGLObject *obj, int value) { lv_preload_set_arc_length(obj->obj(), static_cast<lv_anim_value_t>(value)); }
};

#endif // LVGLPROPERTYPRELOADARCLEN_H
