#ifndef LVGLPROPERTYCHARTWIDTH_H
#define LVGLPROPERTYCHARTWIDTH_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyChartWidth : public LVGLPropertyCoord
{
public:
	LVGLPropertyChartWidth(LVGLProperty *p) : LVGLPropertyCoord(p) {}

	QString name() const override { return "Width"; }

protected:
	virtual lv_coord_t get(LVGLObject *obj) const override { return lv_chart_get_series_width(obj->obj()); }
	virtual void set(LVGLObject *obj, lv_coord_t value) override { lv_chart_set_series_width(obj->obj(), value); }

};

#endif // LVGLPROPERTYCHARTWIDTH_H
