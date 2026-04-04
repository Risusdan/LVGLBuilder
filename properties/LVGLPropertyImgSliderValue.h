#ifndef LVGLPROPERTYIMGSLIDERVALUE_H
#define LVGLPROPERTYIMGSLIDERVALUE_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyImgSliderValue : public LVGLPropertyInt
{
public:
	LVGLPropertyImgSliderValue() : LVGLPropertyInt(INT16_MIN, INT16_MAX) {}

	QString name() const override { return "Value"; }

	QStringList function(LVGLObject *obj) const override {
		return QStringList() << QString("lv_imgslider_set_value(%1, %2, LV_ANIM_OFF);").arg(obj->codeName()).arg(get(obj));
	}

protected:
	int get(LVGLObject *obj) const override { return lv_imgslider_get_value(obj->obj()); }
	void set(LVGLObject *obj, int value) override { lv_imgslider_set_value(obj->obj(), static_cast<int16_t>(value), LV_ANIM_OFF); }
};

#endif // LVGLPROPERTYIMGSLIDERVALUE_H
