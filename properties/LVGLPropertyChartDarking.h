#ifndef LVGLPROPERTYCHARTDARKING_H
#define LVGLPROPERTYCHARTDARKING_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyChartDarking : public LVGLPropertyInt
{
public:
	LVGLPropertyChartDarking(LVGLProperty *p) : LVGLPropertyInt(0, 255, p) {}

	QString name() const override { return "Dark fade"; }

protected:
	virtual int get(LVGLObject *obj) const override { return lv_chart_get_series_darking(obj->obj()); }
	virtual void set(LVGLObject *obj, int value) override { lv_chart_set_series_darking(obj->obj(), static_cast<lv_opa_t>(value)); }

};

#endif // LVGLPROPERTYCHARTDARKING_H
