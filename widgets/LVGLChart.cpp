#include "LVGLChart.h"

#include <QIcon>

#include "LVGLObject.h"
#include "properties/LVGLPropertyRange.h"
#include "properties/LVGLPropertySeries.h"
#include "properties/LVGLPropertyFlags.h"
#include "properties/LVGLPropertyChartType.h"
#include "properties/LVGLPropertyChartRange.h"
#include "properties/LVGLPropertyChartXDiv.h"
#include "properties/LVGLPropertyChartYDiv.h"
#include "properties/LVGLPropertyChartWidth.h"
#include "properties/LVGLPropertyChartOpa.h"
#include "properties/LVGLPropertyChartDarking.h"
#include "properties/LVGLPropertyChartSeries.h"
#include "properties/LVGLPropertyChartMargin.h"


LVGLChart::LVGLChart()
{
	m_properties << new LVGLPropertyChartType;
	m_properties << new LVGLPropertyChartRange;
	m_properties << new LVGLPropertyChartXDiv;
	m_properties << new LVGLPropertyChartYDiv;
	m_properties << new LVGLPropertyChartSeries;
	m_properties << new LVGLPropertyChartMargin;
	m_properties << new LVGLPropertySeries;

	m_editableStyles << LVGL::StyleParts(LVGL::Body | LVGL::Line | LVGL::Text); // LV_CHART_STYLE_MAIN
}

QString LVGLChart::name() const
{
	return "Chart";
}

QString LVGLChart::className() const
{
	return "lv_chart";
}

LVGLWidget::Type LVGLChart::type() const
{
	return Chart;
}

QIcon LVGLChart::icon() const
{
	return QIcon();
}

lv_obj_t *LVGLChart::newObject(lv_obj_t *parent) const
{
	lv_obj_t *obj = lv_chart_create(parent, nullptr);
	return obj;
}

QSize LVGLChart::minimumSize() const
{
	return QSize(200, 150);
}

QStringList LVGLChart::styles() const
{
	return QStringList() << "LV_CHART_STYLE_MAIN";
}

lv_style_t *LVGLChart::style(lv_obj_t *obj, int type) const
{
	return const_cast<lv_style_t*>(lv_chart_get_style(obj, type & 0xff));
}

void LVGLChart::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const
{
	lv_chart_set_style(obj, static_cast<lv_chart_style_t>(type), style);
}

lv_style_t *LVGLChart::defaultStyle(int type) const
{
	if (type == LV_CHART_STYLE_MAIN)
		return &lv_style_pretty;
	return nullptr;
}
