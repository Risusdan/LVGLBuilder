#ifndef LVGLPROPERTYCPICKERSATURATION_H
#define LVGLPROPERTYCPICKERSATURATION_H

/**
 * @file LVGLPropertyCPickerSaturation.h
 * @brief LVGLProperty subclass for lv_cpicker saturation value (0–100).
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyCPickerSaturation : public LVGLPropertyInt
{
public:
	LVGLPropertyCPickerSaturation() : LVGLPropertyInt(0, 100) {}

	QString name() const { return "Saturation"; }

protected:
	int get(LVGLObject *obj) const { return lv_cpicker_get_saturation(obj->obj()); }
	void set(LVGLObject *obj, int value) { lv_cpicker_set_saturation(obj->obj(), static_cast<uint8_t>(value)); }
};

#endif // LVGLPROPERTYCPICKERSATURATION_H
