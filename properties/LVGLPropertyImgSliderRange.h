#ifndef LVGLPROPERTYIMGSLIDERRANGE_H
#define LVGLPROPERTYIMGSLIDERRANGE_H

#include "LVGLPropertyRange.h"
#include "LVGLObject.h"

class LVGLPropertyImgSliderRange : public LVGLPropertyRange
{
public:
	QStringList function(LVGLObject *obj) const override {
		return QStringList() << QString("lv_imgslider_set_range(%1, %2, %3);").arg(obj->codeName()).arg(getMin(obj)).arg(getMax(obj));
	}

protected:
	int getMin(LVGLObject *obj) const override { return lv_imgslider_get_min_value(obj->obj()); }
	int getMax(LVGLObject *obj) const override { return lv_imgslider_get_max_value(obj->obj()); }
	void set(LVGLObject *obj, int min, int max) override {
		lv_imgslider_set_range(obj->obj(), static_cast<int16_t>(min), static_cast<int16_t>(max));
	}

};

#endif // LVGLPROPERTYIMGSLIDERRANGE_H
