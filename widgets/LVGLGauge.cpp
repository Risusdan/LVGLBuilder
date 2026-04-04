#include "LVGLGauge.h"

#include <QIcon>
#include "LVGLObject.h"
#include "properties/LVGLPropertyScale.h"
#include "properties/LVGLPropertyVal2.h"
#include "properties/LVGLPropertyGaugeCriticalValue.h"
#include "properties/LVGLPropertyGaugeVal.h"


LVGLGauge::LVGLGauge()
{
	m_properties << new LVGLPropertyScale;
	m_properties << new LVGLPropertyValInt16(INT16_MIN, INT16_MAX, "Critical value", "lv_gauge_set_critical_value", lv_gauge_set_critical_value, lv_gauge_get_critical_value);
	m_properties << new LVGLPropertyVal2Int16(INT16_MIN, INT16_MAX, "Min", lv_gauge_get_min_value,
															INT16_MIN, INT16_MAX, "Max", lv_gauge_get_max_value,
															"lv_gauge_set_range", lv_gauge_set_range, "Range");
	m_properties << new LVGLPropertyGaugeVal;

	m_editableStyles << LVGL::StyleParts(LVGL::Body | LVGL::Text | LVGL::Line); // LV_GAUGE_STYLE_MAIN
}

QString LVGLGauge::name() const
{
	return "Gauge";
}

QString LVGLGauge::className() const
{
	return "lv_gauge";
}

LVGLWidget::Type LVGLGauge::type() const
{
	return Gauge;
}

QIcon LVGLGauge::icon() const
{
	return QIcon();
}

lv_obj_t *LVGLGauge::newObject(lv_obj_t *parent) const
{
	lv_obj_t *obj = lv_gauge_create(parent, nullptr);
	return obj;
}

QSize LVGLGauge::minimumSize() const
{
	return QSize(100, 100);
}

QStringList LVGLGauge::styles() const
{
	return QStringList() << "LV_GAUGE_STYLE_MAIN";
}

lv_style_t *LVGLGauge::style(lv_obj_t *obj, int type) const
{
	return const_cast<lv_style_t*>(lv_gauge_get_style(obj, type & 0xff));
}

void LVGLGauge::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const
{
	lv_gauge_set_style(obj, static_cast<lv_gauge_style_t>(type), style);
}

lv_style_t *LVGLGauge::defaultStyle(int type) const
{
	if (type == LV_GAUGE_STYLE_MAIN)
		return &lv_style_pretty_color;
	return nullptr;
}
