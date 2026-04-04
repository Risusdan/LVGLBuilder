#ifndef LVGLPROPERTYCPICKERHUE_H
#define LVGLPROPERTYCPICKERHUE_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyCPickerHue : public LVGLPropertyInt
{
public:
	LVGLPropertyCPickerHue() : LVGLPropertyInt(0, 360) {}

	QString name() const { return "Hue"; }

protected:
	int get(LVGLObject *obj) const { return lv_cpicker_get_hue(obj->obj()); }
	void set(LVGLObject *obj, int value) { lv_cpicker_set_hue(obj->obj(), static_cast<uint16_t>(value)); }
};

#endif // LVGLPROPERTYCPICKERHUE_H
