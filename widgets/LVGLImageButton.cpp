#include "LVGLImageButton.h"

#include <QIcon>

#include "LVGLCore.h"
#include "LVGLObject.h"
#include "properties/LVGLPropertyImage.h"
#include "properties/LVGLPropertyImgBtnSrc.h"
#include "properties/LVGLPropertyImgBtnState.h"


LVGLImageButton::LVGLImageButton()
{
	m_properties << new LVGLPropertyImgBtnState;
	m_properties << new LVGLPropertyBool("Toggle", "lv_imgbtn_set_toggle", lv_imgbtn_set_toggle, lv_imgbtn_get_toggle);
	m_properties << new LVGLPropertyImgBtnSrc(LV_IMGBTN_STYLE_REL);
	m_properties << new LVGLPropertyImgBtnSrc(LV_IMGBTN_STYLE_PR);
	m_properties << new LVGLPropertyImgBtnSrc(LV_IMGBTN_STYLE_TGL_REL);
	m_properties << new LVGLPropertyImgBtnSrc(LV_IMGBTN_STYLE_TGL_PR);
	m_properties << new LVGLPropertyImgBtnSrc(LV_IMGBTN_STYLE_INA);

	m_editableStyles << LVGL::Image; // LV_IMGBTN_STYLE_REL
	m_editableStyles << LVGL::Image; // LV_IMGBTN_STYLE_PR
	m_editableStyles << LVGL::Image; // LV_IMGBTN_STYLE_TGL_REL
	m_editableStyles << LVGL::Image; // LV_IMGBTN_STYLE_TGL_PR
	m_editableStyles << LVGL::Image; // LV_IMGBTN_STYLE_INA
}

QString LVGLImageButton::name() const
{
	return "Image button";
}

QString LVGLImageButton::className() const
{
	return "lv_imgbtn";
}

LVGLWidget::Type LVGLImageButton::type() const
{
	return ImageButton;
}

QIcon LVGLImageButton::icon() const
{
	return QIcon();
}

QSize LVGLImageButton::minimumSize() const
{
	return QSize(100, 35);
}

lv_obj_t *LVGLImageButton::newObject(lv_obj_t *parent) const
{
	lv_obj_t *obj = lv_imgbtn_create(parent, nullptr);
	/*for (const LVGLProperty &p:m_properties)
		p.set(obj, p.defaultVal);*/
	return obj;
}

QStringList LVGLImageButton::styles() const
{
	return QStringList() << "LV_IMGBTN_STYLE_REL"
								<< "LV_IMGBTN_STYLE_PR"
								<< "LV_IMGBTN_STYLE_TGL_REL"
								<< "LV_IMGBTN_STYLE_TGL_PR"
								<< "LV_IMGBTN_STYLE_INA";
}

lv_style_t *LVGLImageButton::style(lv_obj_t *obj, int type) const
{
	return const_cast<lv_style_t*>(lv_imgbtn_get_style(obj, type & 0xff));
}

void LVGLImageButton::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const
{
	lv_imgbtn_set_style(obj, static_cast<lv_imgbtn_style_t>(type), style);
}

lv_style_t *LVGLImageButton::defaultStyle(int type) const
{
	if (type == LV_IMGBTN_STYLE_REL)
		return &lv_style_btn_rel;
	else if (type == LV_IMGBTN_STYLE_PR)
		return &lv_style_btn_pr;
	else if (type == LV_IMGBTN_STYLE_TGL_REL)
		return &lv_style_btn_tgl_rel;
	else if (type == LV_IMGBTN_STYLE_TGL_PR)
		return &lv_style_btn_tgl_pr;
	else if (type == LV_IMGBTN_STYLE_INA)
		return &lv_style_btn_ina;
	return nullptr;
}
