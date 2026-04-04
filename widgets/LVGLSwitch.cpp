#include "LVGLSwitch.h"

#include <QIcon>

#include "LVGLObject.h"
#include "properties/LVGLPropertyState.h"


LVGLSwitch::LVGLSwitch()
{
	m_properties << new LVGLPropertyState;

	m_editableStyles << LVGL::Body; // LV_SW_STYLE_BG
	m_editableStyles << LVGL::Body; // LV_SW_STYLE_INDIC
	m_editableStyles << LVGL::Body; // LV_SW_STYLE_KNOB_OFF
	m_editableStyles << LVGL::Body; // LV_SW_STYLE_KNOB_ON
}

QString LVGLSwitch::name() const
{
	return "Switch";
}

QString LVGLSwitch::className() const
{
	return "lv_sw";
}

LVGLWidget::Type LVGLSwitch::type() const
{
	return Switch;
}

QIcon LVGLSwitch::icon() const
{
	return QIcon();
}

lv_obj_t *LVGLSwitch::newObject(lv_obj_t *parent) const
{
	lv_obj_t *obj = lv_sw_create(parent, nullptr);
	return obj;
}

QSize LVGLSwitch::minimumSize() const
{
	return QSize(70, 35);
}

QStringList LVGLSwitch::styles() const
{
	return QStringList() << "LV_SW_STYLE_BG"
								<< "LV_SW_STYLE_INDIC"
								<< "LV_SW_STYLE_KNOB_OFF"
								<< "LV_SW_STYLE_KNOB_ON";
}

lv_style_t *LVGLSwitch::style(lv_obj_t *obj, int type) const
{
	return const_cast<lv_style_t*>(lv_sw_get_style(obj, type & 0xff));
}

void LVGLSwitch::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const
{
	lv_sw_set_style(obj, static_cast<lv_sw_style_t>(type), style);
}

lv_style_t *LVGLSwitch::defaultStyle(int type) const
{
	if (type == LV_SW_STYLE_BG)
		return &lv_style_pretty;
	else if (type == LV_SW_STYLE_INDIC)
		return &lv_style_pretty_color;
	else if (type == LV_SW_STYLE_KNOB_OFF)
		return &lv_style_pretty;
	else if (type == LV_SW_STYLE_KNOB_ON)
		return &lv_style_pretty;
	return nullptr;
}
