#ifndef LVGLPROPERTYCHARTRANGE_H
#define LVGLPROPERTYCHARTRANGE_H

/**
 * @file LVGLPropertyChartRange.h
 * @brief LVGLProperty subclass for lv_chart Y-axis min/max range.
 */

#include "LVGLPropertyRange.h"
#include "LVGLObject.h"

class LVGLPropertyChartRange : public LVGLPropertyRange
{
public:
	QString name() const override { return "Y range"; }

	QStringList function(LVGLObject *obj) const override {
		return QStringList() << QString("lv_chart_set_range(%1, %2, %3);").arg(obj->codeName()).arg(getMin(obj)).arg(getMax(obj));
	}

protected:
	int getMin(LVGLObject *obj) const override {
		return reinterpret_cast<lv_chart_ext_t*>(lv_obj_get_ext_attr(obj->obj()))->ymin;
	}
	int getMax(LVGLObject *obj) const override {
		return reinterpret_cast<lv_chart_ext_t*>(lv_obj_get_ext_attr(obj->obj()))->ymax;
	}
	void set(LVGLObject *obj, int min, int max) override {
		lv_chart_set_range(obj->obj(), static_cast<lv_coord_t>(min), static_cast<lv_coord_t>(max));
	}

};

#endif // LVGLPROPERTYCHARTRANGE_H
