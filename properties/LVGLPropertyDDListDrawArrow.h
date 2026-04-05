#ifndef LVGLPROPERTYDDLISTDRAWARROW_H
#define LVGLPROPERTYDDLISTDRAWARROW_H

/**
 * @file LVGLPropertyDDListDrawArrow.h
 * @brief LVGLProperty subclass for lv_ddlist decoration arrow visibility.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyDDListDrawArrow : public LVGLPropertyBool
{
public:
	QString name() const { return "Decoration arrow"; }

protected:
	bool get(LVGLObject *obj) const { return lv_ddlist_get_draw_arrow(obj->obj()); }
	void set(LVGLObject *obj, bool statue) { lv_ddlist_set_draw_arrow(obj->obj(), statue); }
};

#endif // LVGLPROPERTYDDLISTDRAWARROW_H
