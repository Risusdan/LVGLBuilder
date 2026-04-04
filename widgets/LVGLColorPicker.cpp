#include "LVGLColorPicker.h"

#include "LVGLObject.h"
#include "properties/LVGLPropertyColor.h"

#include <QIcon>
#include "properties/LVGLPropertyCPickerType.h"
#include "properties/LVGLPropertyCPickerHue.h"
#include "properties/LVGLPropertyCPickerSaturation.h"
#include "properties/LVGLPropertyCPickerValue.h"
#include "properties/LVGLPropertyCPickerMode.h"
#include "properties/LVGLPropertyCPickerColor.h"


LVGLColorPicker::LVGLColorPicker()
{
	m_properties << new LVGLPropertyCPickerType;
	m_properties << new LVGLPropertyCPickerHue;
	m_properties << new LVGLPropertyCPickerSaturation;
	m_properties << new LVGLPropertyCPickerValue;
	m_properties << new LVGLPropertyCPickerMode;
	m_properties << new LVGLPropertyBool("Indicator Colored", "lv_cpicker_set_indic_colored", lv_cpicker_set_indic_colored, lv_cpicker_get_indic_colored);
	m_properties << new LVGLPropertyBool("Preview", "lv_cpicker_set_preview", lv_cpicker_set_preview, lv_cpicker_get_preview);
	m_properties << new LVGLPropertyCPickerColor;

	m_editableStyles << LVGL::StyleParts(LVGL::Body | LVGL::Line); // LV_CPICKER_STYLE_MAIN
	m_editableStyles << LVGL::Body; // LV_CPICKER_STYLE_INDICATOR
}

QString LVGLColorPicker::name() const
{
	return "Color picker";
}

QString LVGLColorPicker::className() const
{
	return "lv_cpicker";
}

LVGLWidget::Type LVGLColorPicker::type() const
{
	return ColorPicker;
}

QIcon LVGLColorPicker::icon() const
{
	return QIcon();
}

QSize LVGLColorPicker::minimumSize() const
{
	return QSize(100, 100);
}

lv_obj_t *LVGLColorPicker::newObject(lv_obj_t *parent) const
{
	lv_obj_t *obj = lv_cpicker_create(parent, nullptr);
	//for (const LVGLProperty &p:m_properties)
	//	p.set(obj, p.defaultVal);
	return obj;
}

QStringList LVGLColorPicker::styles() const
{
	return QStringList() << "LV_CPICKER_STYLE_MAIN"
								<< "LV_CPICKER_STYLE_INDICATOR";
}

lv_style_t *LVGLColorPicker::style(lv_obj_t *obj, int type) const
{
	return const_cast<lv_style_t*>(lv_cpicker_get_style(obj, type & 0xff));
}

void LVGLColorPicker::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const
{
	lv_cpicker_set_style(obj, static_cast<lv_cpicker_style_t>(type), style);
}

lv_style_t *LVGLColorPicker::defaultStyle(int type) const
{
	if (type == LV_CPICKER_STYLE_MAIN)
		return &lv_style_plain;
	else if (type == LV_CPICKER_STYLE_INDICATOR)
		return &lv_style_plain;
	return nullptr;
}
