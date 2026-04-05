#ifndef LVGLPROPERTYCHARTXDIV_H
#define LVGLPROPERTYCHARTXDIV_H

/**
 * @file LVGLPropertyChartXDiv.h
 * @brief LVGLProperty subclass for lv_chart number of vertical division lines.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyChartXDiv : public LVGLPropertyInt
{
public:
	LVGLPropertyChartXDiv() : LVGLPropertyInt(0, 255) {}

	QString name() const override { return "X division lines"; }

protected:
	virtual int get(LVGLObject *obj) const override {
		return reinterpret_cast<lv_chart_ext_t*>(lv_obj_get_ext_attr(obj->obj()))->vdiv_cnt;
	}
	virtual void set(LVGLObject *obj, int value) override {
		lv_chart_ext_t * ext = reinterpret_cast<lv_chart_ext_t*>(lv_obj_get_ext_attr(obj->obj()));
		lv_chart_set_div_line_count(obj->obj(), ext->hdiv_cnt, static_cast<uint8_t>(value));
	}

};

#endif // LVGLPROPERTYCHARTXDIV_H
