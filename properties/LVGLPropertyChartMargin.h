#ifndef LVGLPROPERTYCHARTMARGIN_H
#define LVGLPROPERTYCHARTMARGIN_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyChartMargin : public LVGLPropertyInt
{
public:
	LVGLPropertyChartMargin(LVGLProperty *p = nullptr) : LVGLPropertyInt(0, UINT16_MAX, p) {}

	QString name() const override { return "Margin"; }

protected:
	virtual int get(LVGLObject *obj) const override { return lv_chart_get_margin(obj->obj()); }
	virtual void set(LVGLObject *obj, int value) override { lv_chart_set_margin(obj->obj(), static_cast<uint16_t>(value)); }

};

#endif // LVGLPROPERTYCHARTMARGIN_H
