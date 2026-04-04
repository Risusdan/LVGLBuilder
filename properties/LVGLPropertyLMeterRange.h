#ifndef LVGLPROPERTYLMETERRANGE_H
#define LVGLPROPERTYLMETERRANGE_H

#include "LVGLPropertyRange.h"
#include "LVGLObject.h"

class LVGLPropertyLMeterRange : public LVGLPropertyRange
{
public:
	QStringList function(LVGLObject *obj) const {
		return { QString("lv_lmeter_set_range(%1, %2, %2);").arg(obj->codeName()).arg(getMin(obj)).arg(getMax(obj)) };
	}

protected:
	int getMin(LVGLObject *obj) const { return lv_lmeter_get_min_value(obj->obj()); }
	int getMax(LVGLObject *obj) const { return lv_lmeter_get_max_value(obj->obj()); }
	void set(LVGLObject *obj, int min, int max) {
		lv_lmeter_set_range(obj->obj(), static_cast<int16_t>(min), static_cast<int16_t>(max));
	}
};

#endif // LVGLPROPERTYLMETERRANGE_H
