#ifndef LVGLPROPERTYCPICKERCOLOR_H
#define LVGLPROPERTYCPICKERCOLOR_H

#include "LVGLPropertyColor.h"
#include "LVGLObject.h"

class LVGLPropertyCPickerColor : public LVGLPropertyColor
{
public:
	QString name() const { return "Color"; }

protected:
	lv_color_t get(LVGLObject *obj) const { return lv_cpicker_get_color(obj->obj()); }
	void set(LVGLObject *obj, lv_color_t boolean) { lv_cpicker_set_color(obj->obj(), boolean); }
};

#endif // LVGLPROPERTYCPICKERCOLOR_H
