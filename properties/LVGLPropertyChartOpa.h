#ifndef LVGLPROPERTYCHARTOPA_H
#define LVGLPROPERTYCHARTOPA_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyChartOpa : public LVGLPropertyInt
{
public:
	LVGLPropertyChartOpa(LVGLProperty *p) : LVGLPropertyInt(0, 255, p) {}

	QString name() const override { return "Opacity"; }

protected:
	virtual int get(LVGLObject *obj) const override { return lv_chart_get_series_opa(obj->obj()); }
	virtual void set(LVGLObject *obj, int value) override { lv_chart_set_series_opa(obj->obj(), static_cast<lv_opa_t>(value)); }

};

#endif // LVGLPROPERTYCHARTOPA_H
