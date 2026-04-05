#ifndef LVGLPROPERTYCHARTSERIES_H
#define LVGLPROPERTYCHARTSERIES_H

/**
 * @file LVGLPropertyChartSeries.h
 * @brief LVGLProperty subclass grouping lv_chart series appearance (width, opacity, darking).
 */

#include "LVGLProperty.h"
#include "LVGLPropertyChartWidth.h"
#include "LVGLPropertyChartOpa.h"
#include "LVGLPropertyChartDarking.h"

class LVGLPropertyChartSeries : public LVGLProperty
{
	LVGLPropertyChartWidth *m_w;
	LVGLPropertyChartOpa *m_opa;
	LVGLPropertyChartDarking *m_darking;
public:
	LVGLPropertyChartSeries()
		: m_w(new LVGLPropertyChartWidth(this))
		, m_opa(new LVGLPropertyChartOpa(this))
		, m_darking(new LVGLPropertyChartDarking(this))
	{
		m_childs << m_w << m_opa << m_darking;
	}

	QString name() const override { return "Series"; }
};

#endif // LVGLPROPERTYCHARTSERIES_H
