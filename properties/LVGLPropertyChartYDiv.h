#ifndef LVGLPROPERTYCHARTYDIV_H
#define LVGLPROPERTYCHARTYDIV_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyChartYDiv : public LVGLPropertyInt
{
public:
	LVGLPropertyChartYDiv() : LVGLPropertyInt(0, 255) {}

	QString name() const override { return "Y division lines"; }

protected:
	virtual int get(LVGLObject *obj) const override {
		return reinterpret_cast<lv_chart_ext_t*>(lv_obj_get_ext_attr(obj->obj()))->hdiv_cnt;
	}
	virtual void set(LVGLObject *obj, int value) override {
		lv_chart_ext_t * ext = reinterpret_cast<lv_chart_ext_t*>(lv_obj_get_ext_attr(obj->obj()));
		lv_chart_set_div_line_count(obj->obj(), static_cast<uint8_t>(value), ext->vdiv_cnt);
	}

};

#endif // LVGLPROPERTYCHARTYDIV_H
