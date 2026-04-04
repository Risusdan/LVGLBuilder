#include "LVGLPage.h"

#include <QIcon>

#include "LVGLObject.h"
#include "properties/LVGLPropertyPageScrollbars.h"
#include "properties/LVGLPropertyPageWidth.h"
#include "properties/LVGLPropertyPageHeight.h"
#include "properties/LVGLPropertyPageEdgeFlash.h"
#include "properties/LVGLPropertyPageScrollPropagation.h"


LVGLPage::LVGLPage()
{
	m_properties << new LVGLPropertyPageScrollbars;
	m_properties << new LVGLPropertyPageWidth;
	m_properties << new LVGLPropertyPageHeight;
	m_properties << new LVGLPropertyPageEdgeFlash;
	m_properties << new LVGLPropertyPageScrollPropagation;

	m_editableStyles << LVGL::Body; // LV_PAGE_STYLE_BG
	m_editableStyles << LVGL::Body; // LV_PAGE_STYLE_SCRL
	m_editableStyles << LVGL::Body; // LV_PAGE_STYLE_SB
}

QString LVGLPage::name() const
{
	return "Page";
}

QString LVGLPage::className() const
{
	return "lv_page";
}

LVGLWidget::Type LVGLPage::type() const
{
	return Page;
}

QIcon LVGLPage::icon() const
{
	return QIcon();
}

lv_obj_t *LVGLPage::newObject(lv_obj_t *parent) const
{
	lv_obj_t *obj = lv_page_create(parent, nullptr);
	return obj;
}

QSize LVGLPage::minimumSize() const
{
	return QSize(100, 35);
}

QStringList LVGLPage::styles() const
{
	return QStringList() << "LV_PAGE_STYLE_BG"
								<< "LV_PAGE_STYLE_SCRL"
								<< "LV_PAGE_STYLE_SB";
}

lv_style_t *LVGLPage::style(lv_obj_t *obj, int type) const
{
	return const_cast<lv_style_t*>(lv_page_get_style(obj, type & 0xff));
}

void LVGLPage::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const
{
	lv_page_set_style(obj, static_cast<lv_btn_style_t>(type), style);
}

lv_style_t *LVGLPage::defaultStyle(int type) const
{
	if (type == LV_PAGE_STYLE_BG)
		return &lv_style_pretty_color;
	else if (type == LV_PAGE_STYLE_SCRL)
		return &lv_style_pretty;
	else if (type == LV_PAGE_STYLE_SB)
		return &lv_style_pretty_color;
	return nullptr;
}
