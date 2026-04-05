#ifndef LVGLPROPERTYCPICKERVALUE_H
#define LVGLPROPERTYCPICKERVALUE_H

/**
 * @file LVGLPropertyCPickerValue.h
 * @brief LVGLProperty subclass for lv_cpicker brightness value (0–100).
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyCPickerValue : public LVGLPropertyInt
{
public:
	LVGLPropertyCPickerValue() : LVGLPropertyInt(0, 100) {}

	QString name() const { return "Value"; }

protected:
	int get(LVGLObject *obj) const { return lv_cpicker_get_value(obj->obj()); }
	void set(LVGLObject *obj, int value) { lv_cpicker_set_value(obj->obj(), static_cast<uint8_t>(value)); }
};

#endif // LVGLPROPERTYCPICKERVALUE_H
