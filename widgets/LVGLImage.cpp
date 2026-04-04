#include "LVGLImage.h"

#include <QIcon>
#include <QComboBox>

#include "properties/LVGLPropertyImage.h"
#include "LVGLObject.h"
#include "LVGLCore.h"
#include "properties/LVGLPropertyImgOffsetX.h"
#include "properties/LVGLPropertyImgOffsetY.h"
#include "properties/LVGLPropertyImgOffset.h"
#include "properties/LVGLPropertyImgAutoSize.h"
#include "properties/LVGLPropertyImgSource.h"


LVGLImage::LVGLImage()
{
	m_properties << new LVGLPropertyImgOffset;
	m_properties << new LVGLPropertyImgAutoSize;
	m_properties << new LVGLPropertyImgSource;

	m_editableStyles << LVGL::Image; // LV_IMG_STYLE_MAIN
}

QString LVGLImage::name() const
{
	return "Image";
}

QString LVGLImage::className() const
{
	return "lv_img";
}

LVGLWidget::Type LVGLImage::type() const
{
	return Image;
}

QIcon LVGLImage::icon() const
{
	return QIcon();
}

QSize LVGLImage::minimumSize() const
{
	return QSize(50, 50);
}

lv_obj_t *LVGLImage::newObject(lv_obj_t *parent) const
{
	lv_obj_t *obj = lv_img_create(parent, nullptr);
	lv_img_set_src(obj, lvgl.defaultImage());
	return obj;
}

QStringList LVGLImage::styles() const
{
	return QStringList() << "LV_IMG_STYLE_MAIN";
}

lv_style_t *LVGLImage::style(lv_obj_t *obj, int type) const
{
	return const_cast<lv_style_t*>(lv_img_get_style(obj, type & 0xff));
}

void LVGLImage::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const
{
	lv_img_set_style(obj, static_cast<lv_img_style_t>(type), style);
}

lv_style_t *LVGLImage::defaultStyle(int type) const
{
	if (type == LV_IMG_STYLE_MAIN)
		return &lv_style_plain;
	return nullptr;
}
