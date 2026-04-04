#include "LVGLImageSlider.h"

#include <QIcon>

#include "LVGLCore.h"
#include "LVGLObject.h"
#include "properties/LVGLPropertyImage.h"
#include "properties/LVGLPropertyRange.h"
#include "properties/LVGLPropertySliderImgBg.h"
#include "properties/LVGLPropertySliderImgIndic.h"
#include "properties/LVGLPropertySliderImgKnob.h"
#include "properties/LVGLPropertyImgSliderValue.h"
#include "properties/LVGLPropertyImgSliderRange.h"
#include "properties/LVGLPropertyImgSliderOffsetX.h"
#include "properties/LVGLPropertyImgSliderOffsetY.h"
#include "properties/LVGLPropertyImgSliderOffset.h"


LVGLImageSlider::LVGLImageSlider()
{
	m_properties << new LVGLPropertySliderImgBg;
	m_properties << new LVGLPropertySliderImgIndic;
	m_properties << new LVGLPropertySliderImgKnob;
	m_properties << new LVGLPropertyImgSliderValue;
	m_properties << new LVGLPropertyImgSliderRange;
	m_properties << new LVGLPropertyImgSliderOffset;

	m_editableStyles << LVGL::Image; // LV_IMGSLIDER_STYLE_BG
	m_editableStyles << LVGL::Image; // LV_IMGSLIDER_STYLE_INDIC
	m_editableStyles << LVGL::Image; // LV_IMGSLIDER_STYLE_KNOB
}

QString LVGLImageSlider::name() const
{
	return "Image slider";
}

QString LVGLImageSlider::className() const
{
	return "lv_imgslider";
}

LVGLWidget::Type LVGLImageSlider::type() const
{
	return ImageSlider;
}

QIcon LVGLImageSlider::icon() const
{
	return QIcon();
}

QSize LVGLImageSlider::minimumSize() const
{
	return QSize(100, 35);
}

lv_obj_t *LVGLImageSlider::newObject(lv_obj_t *parent) const
{
	lv_obj_t *obj = lv_imgslider_create(parent, nullptr);
	return obj;
}

QStringList LVGLImageSlider::styles() const
{
	return QStringList() << "LV_IMGSLIDER_STYLE_BG"
								<< "LV_IMGSLIDER_STYLE_INDIC"
								<< "LV_IMGSLIDER_STYLE_KNOB";
}

lv_style_t *LVGLImageSlider::style(lv_obj_t *obj, int type) const
{
	return const_cast<lv_style_t*>(lv_imgslider_get_style(obj, type & 0xff));
}

void LVGLImageSlider::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const
{
	lv_imgslider_set_style(obj, static_cast<lv_imgslider_style_t>(type), style);
}

lv_style_t *LVGLImageSlider::defaultStyle(int type) const
{
	Q_UNUSED(type)
	return nullptr;
}
