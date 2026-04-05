#ifndef LVGLPROPERTYCPICKERMODE_H
#define LVGLPROPERTYCPICKERMODE_H

/**
 * @file LVGLPropertyCPickerMode.h
 * @brief LVGLProperty subclass for lv_cpicker color channel mode (hue/saturation/value).
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyCPickerMode : public LVGLPropertyEnum
{
public:
	LVGLPropertyCPickerMode()
		: LVGLPropertyEnum(QStringList() << "Hue" << "Saturation" << "Value")
	{}

	QString name() const { return "Color mode"; }

protected:
	int get(LVGLObject *obj) const { return lv_cpicker_get_color_mode(obj->obj()); }
	void set(LVGLObject *obj, int index) { lv_cpicker_set_color_mode(obj->obj(), index & 0xff); }
};

#endif // LVGLPROPERTYCPICKERMODE_H
