#include "LVGLPreloader.h"

#include <QIcon>
#include "LVGLObject.h"
#include "properties/LVGLPropertyPreloadArcLen.h"
#include "properties/LVGLPropertyPreloadSpinTime.h"
#include "properties/LVGLPropertyPreloadType.h"
#include "properties/LVGLPropertyPreloadDir.h"


LVGLPreloader::LVGLPreloader()
{
	m_properties << new LVGLPropertyPreloadArcLen;
	m_properties << new LVGLPropertyPreloadSpinTime;
	m_properties << new LVGLPropertyPreloadType;
	m_properties << new LVGLPropertyPreloadDir;

	m_editableStyles << LVGL::StyleParts(LVGL::BodyBorder | LVGL::BodyPadding | LVGL::Line); // LV_PRELOAD_STYLE_MAIN
}

QString LVGLPreloader::name() const
{
	return "Preload";
}

QString LVGLPreloader::className() const
{
	return "lv_preload";
}

LVGLWidget::Type LVGLPreloader::type() const
{
	return Preloader;
}

QIcon LVGLPreloader::icon() const
{
	return QIcon();
}

lv_obj_t *LVGLPreloader::newObject(lv_obj_t *parent) const
{
	lv_obj_t *obj = lv_preload_create(parent, nullptr);
	return obj;
}

QSize LVGLPreloader::minimumSize() const
{
	return QSize(100, 100);
}

QStringList LVGLPreloader::styles() const
{
	return QStringList() << "LV_PRELOAD_STYLE_MAIN";
}

lv_style_t *LVGLPreloader::style(lv_obj_t *obj, int type) const
{
	return const_cast<lv_style_t*>(lv_preload_get_style(obj, type & 0xff));
}

void LVGLPreloader::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const
{
	lv_preload_set_style(obj, static_cast<lv_preload_style_t>(type), style);
}

lv_style_t *LVGLPreloader::defaultStyle(int type) const
{
	if (type == LV_PRELOAD_STYLE_MAIN)
		return &lv_style_pretty_color;
	return nullptr;
}
