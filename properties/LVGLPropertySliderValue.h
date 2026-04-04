#ifndef LVGLPROPERTYSLIDERVALUE_H
#define LVGLPROPERTYSLIDERVALUE_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertySliderValue : public LVGLPropertyInt
{
public:
	LVGLPropertySliderValue() : LVGLPropertyInt(INT16_MIN, INT16_MAX) {}

	QString name() const { return "Value"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_slider_set_value(%1, %2, LV_ANIM_OFF);").arg(obj->codeName()).arg(get(obj));
	}

protected:
	int get(LVGLObject *obj) const { return lv_slider_get_value(obj->obj()); }
	void set(LVGLObject *obj, int value) { lv_slider_set_value(obj->obj(), static_cast<int16_t>(value), LV_ANIM_OFF); }
};

#endif // LVGLPROPERTYSLIDERVALUE_H
