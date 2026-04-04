#include "LVGLBar.h"

#include <QIcon>
#include "LVGLObject.h"
#include "properties/LVGLPropertyBarValue.h"
#include "properties/LVGLPropertyBarRange.h"

LVGLBar::LVGLBar()
{
	m_properties << new LVGLPropertyBarValue;
	m_properties << new LVGLPropertyBarRange;

	m_editableStyles << LVGL::Body; // LV_BAR_STYLE_BG
	m_editableStyles << LVGL::Body; // LV_BAR_STYLE_INDIC
}

QString LVGLBar::name() const
{
	return "Bar";
}

QString LVGLBar::className() const
{
	return "lv_bar";
}

LVGLWidget::Type LVGLBar::type() const
{
	return Bar;
}

QIcon LVGLBar::icon() const
{
	return QIcon();
}

lv_obj_t *LVGLBar::newObject(lv_obj_t *parent) const
{
	lv_obj_t *obj = lv_bar_create(parent, nullptr);
	return obj;
}

QSize LVGLBar::minimumSize() const
{
	return QSize(200, 30);
}

QStringList LVGLBar::styles() const
{
	return QStringList() << "LV_BAR_STYLE_BG"
								<< "LV_BAR_STYLE_INDIC";
}

lv_style_t *LVGLBar::style(lv_obj_t *obj, int type) const
{
	return const_cast<lv_style_t*>(lv_bar_get_style(obj, type & 0xff));
}

void LVGLBar::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const
{
	lv_bar_set_style(obj, static_cast<lv_bar_style_t>(type), style);
}

lv_style_t *LVGLBar::defaultStyle(int type) const
{
	if (type == LV_BAR_STYLE_BG)
		return &lv_style_pretty;
	else if (type == LV_BAR_STYLE_INDIC)
		return &lv_style_pretty_color;
	return nullptr;
}
