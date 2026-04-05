#ifndef LVGLPROPERTYLINEINVERTY_H
#define LVGLPROPERTYLINEINVERTY_H

/**
 * @file LVGLPropertyLineInvertY.h
 * @brief LVGLProperty subclass for lv_line Y-axis inversion flag.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyLineInvertY : public LVGLPropertyBool
{
public:
	QString name() const { return "InvertY"; }

protected:
	bool get(LVGLObject *obj) const { return lv_line_get_y_invert(obj->obj()); }
	void set(LVGLObject *obj, bool statue) { lv_line_set_y_invert(obj->obj(), statue); }
};

#endif // LVGLPROPERTYLINEINVERTY_H
