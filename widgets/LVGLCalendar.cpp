#include "LVGLCalendar.h"

#include <QIcon>
#include "LVGLObject.h"
#include "properties/LVGLPropertyDate.h"
#include "properties/LVGLPropertyDateList.h"
#include "properties/LVGLPropertyTextList.h"
#include "LVGLCore.h"
#include "properties/LVGLPropertyDayNames.h"
#include "properties/LVGLPropertyMonthNames.h"


LVGLCalendar::LVGLCalendar()
{
	m_properties << new LVGLPropertyDate("Current date", "lv_calendar_set_today_date", lv_calendar_set_today_date, lv_calendar_get_today_date);
	m_properties << new LVGLPropertyDate("Shown date", "lv_calendar_set_showed_date", lv_calendar_set_showed_date, lv_calendar_get_showed_date);
	m_properties << new LVGLPropertyDateList("Highlighted dates", "lv_calendar_set_highlighted_dates",
														  lv_calendar_set_highlighted_dates, lv_calendar_get_highlighted_dates, lv_calendar_get_highlighted_dates_num);

	m_properties << new LVGLPropertyDayNames;
	m_properties << new LVGLPropertyMonthNames;

	m_editableStyles << LVGL::StyleParts(LVGL::Body | LVGL::Text); // LV_CALENDAR_STYLE_BG
	m_editableStyles << LVGL::StyleParts(LVGL::Body | LVGL::Text); // LV_CALENDAR_STYLE_HEADER
	m_editableStyles << LVGL::StyleParts(LVGL::Body | LVGL::Text); // LV_CALENDAR_STYLE_HEADER_PR
	m_editableStyles << LVGL::Text; // LV_CALENDAR_STYLE_DAY_NAMES
	m_editableStyles << LVGL::Text; // LV_CALENDAR_STYLE_HIGHLIGHTED_DAYS
	m_editableStyles << LVGL::Text; // LV_CALENDAR_STYLE_INACTIVE_DAYS
	m_editableStyles << LVGL::Body; // LV_CALENDAR_STYLE_WEEK_BOX
	m_editableStyles << LVGL::StyleParts(LVGL::Body | LVGL::Text); // LV_CALENDAR_STYLE_TODAY_BOX
}

QString LVGLCalendar::name() const
{
	return "Calendar";
}

QString LVGLCalendar::className() const
{
	return "lv_calendar";
}

LVGLWidget::Type LVGLCalendar::type() const
{
	return Calendar;
}

QIcon LVGLCalendar::icon() const
{
	return QIcon();
}

lv_obj_t *LVGLCalendar::newObject(lv_obj_t *parent) const
{
	lv_obj_t *obj = lv_calendar_create(parent, nullptr);
	return obj;
}

QSize LVGLCalendar::minimumSize() const
{
	return QSize(250, 200);
}

QStringList LVGLCalendar::styles() const
{
	return QStringList() << "LV_CALENDAR_STYLE_BG"
								<< "LV_CALENDAR_STYLE_HEADER"
								<< "LV_CALENDAR_STYLE_HEADER_PR"
								<< "LV_CALENDAR_STYLE_DAY_NAMES"
								<< "LV_CALENDAR_STYLE_HIGHLIGHTED_DAYS"
								<< "LV_CALENDAR_STYLE_INACTIVE_DAYS"
								<< "LV_CALENDAR_STYLE_WEEK_BOX"
								<< "LV_CALENDAR_STYLE_TODAY_BOX";
}

lv_style_t *LVGLCalendar::style(lv_obj_t *obj, int type) const
{
	return const_cast<lv_style_t*>(lv_calendar_get_style(obj, type & 0xff));
}

void LVGLCalendar::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const
{
	lv_calendar_set_style(obj, static_cast<lv_calendar_style_t>(type), style);
}

lv_style_t *LVGLCalendar::defaultStyle(int type) const
{
	if (type == LV_CALENDAR_STYLE_BG)
		return &lv_style_pretty;
	else if (type == LV_CALENDAR_STYLE_HEADER)
		return &lv_style_plain_color;
	else if (type == LV_CALENDAR_STYLE_HEADER_PR)
		return &lv_style_pretty_color;
	else if (type == LV_CALENDAR_STYLE_DAY_NAMES)
		return &lv_style_pretty;
	else if (type == LV_CALENDAR_STYLE_HIGHLIGHTED_DAYS)
		return &lv_style_plain_color;
	else if (type == LV_CALENDAR_STYLE_INACTIVE_DAYS)
		return &lv_style_btn_ina;
	else if (type == LV_CALENDAR_STYLE_WEEK_BOX)
		return &lv_style_plain_color;
	else if (type == LV_CALENDAR_STYLE_TODAY_BOX)
		return &lv_style_pretty_color;
	return nullptr;
}
