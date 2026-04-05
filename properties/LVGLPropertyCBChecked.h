#ifndef LVGLPROPERTYCBCHECKED_H
#define LVGLPROPERTYCBCHECKED_H

/**
 * @file LVGLPropertyCBChecked.h
 * @brief LVGLProperty subclass for lv_cb checked state.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyCBChecked : public LVGLPropertyBool
{
public:
	QString name() const { return "Checked"; }

protected:
	bool get(LVGLObject *obj) const { return lv_cb_is_checked(obj->obj()); }
	void set(LVGLObject *obj, bool boolean) { lv_cb_set_checked(obj->obj(), boolean); }
};

#endif // LVGLPROPERTYCBCHECKED_H
