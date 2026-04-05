#ifndef LVGLPROPERTYCHARTTYPE_H
#define LVGLPROPERTYCHARTTYPE_H

/**
 * @file LVGLPropertyChartType.h
 * @brief LVGLProperty subclass for lv_chart draw type flags (line, column, point, area).
 */

#include "LVGLPropertyFlags.h"
#include "LVGLObject.h"

class LVGLPropertyChartType : public LVGLPropertyFlags
{
public:
	LVGLPropertyChartType()
		: LVGLPropertyFlags({"None", "Line", "Columns", "Points", "Area", "Vertical lines"},
								  {"LV_CHART_TYPE_NONE", "LV_CHART_TYPE_LINE", "LV_CHART_TYPE_COLUMN", "LV_CHART_TYPE_POINT", "LV_CHART_TYPE_AREA", "LV_CHART_TYPE_VERTICAL_LINE"},
								  {LV_CHART_TYPE_NONE, LV_CHART_TYPE_LINE, LV_CHART_TYPE_COLUMN, LV_CHART_TYPE_POINT, LV_CHART_TYPE_AREA, LV_CHART_TYPE_VERTICAL_LINE})
	{}

	QString name() const { return "Type"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_chart_set_type(%1, %2);").arg(obj->codeName()).arg(codeValue(obj));
	}

protected:
	int get(LVGLObject *obj) const { return lv_chart_get_type(obj->obj()); }
	void set(LVGLObject *obj, int index) { lv_chart_set_type(obj->obj(), index & 0xff); }

};

#endif // LVGLPROPERTYCHARTTYPE_H
