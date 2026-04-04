#include "LVGLSlider.h"

#include <QIcon>

#include "LVGLObject.h"
#include "properties/LVGLPropertyRange.h"
#include "properties/LVGLPropertySliderValue.h"
#include "properties/LVGLPropertySliderRange.h"


LVGLSlider::LVGLSlider()
{
	m_properties << new LVGLPropertySliderValue;
	m_properties << new LVGLPropertySliderRange;
	m_properties << new LVGLPropertyBool("Knob inside", "lv_slider_set_knob_in", lv_slider_set_knob_in, lv_slider_get_knob_in);

	m_editableStyles << LVGL::Body; // LV_SLIDER_STYLE_BG
	m_editableStyles << LVGL::Body; // LV_SLIDER_STYLE_INDIC
	m_editableStyles << LVGL::Body; // LV_SLIDER_STYLE_KNOB
}

QString LVGLSlider::name() const
{
	return "Slider";
}

QString LVGLSlider::className() const
{
	return "lv_slider";
}

LVGLWidget::Type LVGLSlider::type() const
{
	return Slider;
}

QIcon LVGLSlider::icon() const
{
	return QIcon();
}

lv_obj_t *LVGLSlider::newObject(lv_obj_t *parent) const
{
	lv_obj_t *obj = lv_slider_create(parent, nullptr);
	/*for (const LVGLProperty &p:m_properties)
		p.set(obj, p.defaultVal);*/
	return obj;
}

QSize LVGLSlider::minimumSize() const
{
	return QSize(150, 25);
}

QStringList LVGLSlider::styles() const
{
	return QStringList() << "LV_SLIDER_STYLE_BG"
								<< "LV_SLIDER_STYLE_INDIC"
								<< "LV_SLIDER_STYLE_KNOB";
}

lv_style_t *LVGLSlider::style(lv_obj_t *obj, int type) const
{
	return const_cast<lv_style_t*>(lv_slider_get_style(obj, type & 0xff));
}

void LVGLSlider::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const
{
	lv_slider_set_style(obj, static_cast<lv_slider_style_t>(type), style);
}

lv_style_t *LVGLSlider::defaultStyle(int type) const
{
	if (type == LV_SLIDER_STYLE_BG)
		return &lv_style_pretty;
	else if (type == LV_SLIDER_STYLE_INDIC)
		return &lv_style_pretty_color;
	else if (type == LV_SLIDER_STYLE_KNOB)
		return &lv_style_pretty;
	return nullptr;
}
