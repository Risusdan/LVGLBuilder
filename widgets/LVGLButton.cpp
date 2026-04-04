#include "LVGLButton.h"

#include <QIcon>

#include "LVGLObject.h"
#include "properties/LVGLPropertyButtonState.h"
#include "properties/LVGLPropertyButtonLayout.h"

LVGLButton::LVGLButton()
{
	m_properties << new LVGLPropertyButtonState;
	m_properties << new LVGLPropertyBool("Toggle", "lv_btn_set_toggle", lv_btn_set_toggle, lv_btn_get_toggle);
	m_properties << new LVGLPropertyButtonLayout;

	m_editableStyles << LVGL::Body; // LV_BTN_STYLE_REL
	m_editableStyles << LVGL::Body; // LV_BTN_STYLE_PR
	m_editableStyles << LVGL::Body; // LV_BTN_STYLE_TGL_REL
	m_editableStyles << LVGL::Body; // LV_BTN_STYLE_TGL_PR
	m_editableStyles << LVGL::Body; // LV_BTN_STYLE_INA
}

QString LVGLButton::name() const
{
	return "Button";
}

QString LVGLButton::className() const
{
	return "lv_btn";
}

LVGLWidget::Type LVGLButton::type() const
{
	return Button;
}

QIcon LVGLButton::icon() const
{
	return QIcon();
}

lv_obj_t *LVGLButton::newObject(lv_obj_t *parent) const
{
	lv_obj_t *obj = lv_btn_create(parent, nullptr);
	return obj;
}

QSize LVGLButton::minimumSize() const
{
	return QSize(100, 35);
}

QStringList LVGLButton::styles() const
{
	return QStringList() << "LV_BTN_STYLE_REL"
								<< "LV_BTN_STYLE_PR"
								<< "LV_BTN_STYLE_TGL_REL"
								<< "LV_BTN_STYLE_TGL_PR"
								<< "LV_BTN_STYLE_INA";
}

lv_style_t *LVGLButton::style(lv_obj_t *obj, int type) const
{
	return const_cast<lv_style_t*>(lv_btn_get_style(obj, type & 0xff));
}

void LVGLButton::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const
{
	lv_btn_set_style(obj, static_cast<lv_btn_style_t>(type), style);
}

lv_style_t *LVGLButton::defaultStyle(int type) const
{
	if (type == LV_BTN_STYLE_REL)
		return &lv_style_btn_rel;
	else if (type == LV_BTN_STYLE_PR)
		return &lv_style_btn_pr;
	else if (type == LV_BTN_STYLE_TGL_REL)
		return &lv_style_btn_tgl_rel;
	else if (type == LV_BTN_STYLE_TGL_PR)
		return &lv_style_btn_tgl_pr;
	else if (type == LV_BTN_STYLE_INA)
		return &lv_style_btn_ina;
	return nullptr;
}
