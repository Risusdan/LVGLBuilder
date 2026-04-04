#include "LVGLDropDownList.h"

#include <QIcon>

#include "LVGLObject.h"
#include "properties/LVGLPropertyDDListAlign.h"
#include "properties/LVGLPropertyDDListFixedWidth.h"
#include "properties/LVGLPropertyDDListFixedHeight.h"
#include "properties/LVGLPropertyDDListScrollbars.h"
#include "properties/LVGLPropertyDDListAnimationTime.h"
#include "properties/LVGLPropertyDDListDrawArrow.h"
#include "properties/LVGLPropertyDDListStayOpen.h"


LVGLDropDownList::LVGLDropDownList()
{
	m_properties << new LVGLPropertyDDListAlign;
	m_properties << new LVGLPropertyDDListFixedWidth;
	m_properties << new LVGLPropertyDDListFixedHeight;
	m_properties << new LVGLPropertyDDListScrollbars;
	m_properties << new LVGLPropertyDDListAnimationTime;
	m_properties << new LVGLPropertyDDListDrawArrow;
	m_properties << new LVGLPropertyDDListStayOpen;

	m_editableStyles << LVGL::StyleParts(LVGL::Body | LVGL::Text); // LV_DDLIST_STYLE_BG
	m_editableStyles << LVGL::StyleParts(LVGL::Body | LVGL::Text); // LV_DDLIST_STYLE_SEL
	m_editableStyles << LVGL::Body; // LV_DDLIST_STYLE_SB
}

QString LVGLDropDownList::name() const
{
	return "Drop down list";
}

QString LVGLDropDownList::className() const
{
	return "lv_ddlist";
}

LVGLWidget::Type LVGLDropDownList::type() const
{
	return DropDownList;
}

QIcon LVGLDropDownList::icon() const
{
	return QIcon();
}

lv_obj_t *LVGLDropDownList::newObject(lv_obj_t *parent) const
{
	lv_obj_t *obj = lv_ddlist_create(parent, nullptr);
	return obj;
}

QSize LVGLDropDownList::minimumSize() const
{
	return QSize(100, 35);
}

QStringList LVGLDropDownList::styles() const
{
	return QStringList() << "LV_DDLIST_STYLE_BG"
								<< "LV_DDLIST_STYLE_SEL"
								<< "LV_DDLIST_STYLE_SB";
}

lv_style_t *LVGLDropDownList::style(lv_obj_t *obj, int type) const
{
	return const_cast<lv_style_t*>(lv_ddlist_get_style(obj, type & 0xff));
}

void LVGLDropDownList::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const
{
	lv_ddlist_set_style(obj, static_cast<lv_ddlist_style_t>(type), style);
}

lv_style_t *LVGLDropDownList::defaultStyle(int type) const
{
	if (type == LV_DDLIST_STYLE_BG)
		return &lv_style_pretty;
	else if (type == LV_DDLIST_STYLE_SEL)
		return &lv_style_plain_color;
	else if (type == LV_DDLIST_STYLE_SB)
		return &lv_style_plain_color;
	return nullptr;
}
