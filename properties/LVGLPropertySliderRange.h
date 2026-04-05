#ifndef LVGLPROPERTYSLIDERRANGE_H
#define LVGLPROPERTYSLIDERRANGE_H

/**
 * @file LVGLPropertySliderRange.h
 * @brief LVGLProperty subclass for lv_slider min/max range.
 */

#include "LVGLPropertyRange.h"
#include "LVGLObject.h"

class LVGLPropertySliderRange : public LVGLPropertyRange
{
public:
	QStringList function(LVGLObject *obj) const override {
		return QStringList() << QString("lv_slider_set_range(%1, %2, %3);").arg(obj->codeName()).arg(getMin(obj)).arg(getMax(obj));
	}

protected:
	int getMin(LVGLObject *obj) const override { return lv_slider_get_min_value(obj->obj()); }
	int getMax(LVGLObject *obj) const override { return lv_slider_get_max_value(obj->obj()); }
	void set(LVGLObject *obj, int min, int max) override {
		lv_slider_set_range(obj->obj(), static_cast<int16_t>(min), static_cast<int16_t>(max));
	}
};

#endif // LVGLPROPERTYSLIDERRANGE_H
