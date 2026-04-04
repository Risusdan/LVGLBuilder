#include "LVGLTabview.h"

#include <QIcon>

#include "LVGLObject.h"
#include "properties/LVGLPropertyTextList.h"
#include "LVGLCore.h"
#include "properties/LVGLPropertyTabBtnPos.h"
#include "properties/LVGLPropertyTabs.h"
#include "properties/LVGLPropertyTabCurrent.h"
#include "properties/LVGLPropertyTabBtnHide.h"
#include "properties/LVGLPropertyTabAnimTime.h"
#include "properties/LVGLPropertyTabSliding.h"
#include "properties/LVGLPropertyTabScrollbars.h"


LVGLTabview::LVGLTabview()
{
	m_properties << new LVGLPropertyTabs;
	m_properties << new LVGLPropertyTabBtnPos;
	m_properties << new LVGLPropertyTabCurrent;
	m_properties << new LVGLPropertyTabBtnHide;
	m_properties << new LVGLPropertyTabSliding;
	//m_properties << new LVGLPropertyTabScrollbars;

	m_editableStyles << LVGL::Body; // LV_TABVIEW_STYLE_BG
	m_editableStyles << LVGL::Body; // LV_TABVIEW_STYLE_INDIC
	m_editableStyles << LVGL::Body; // LV_TABVIEW_STYLE_BTN_BG
	m_editableStyles << LVGL::Body; // LV_TABVIEW_STYLE_BTN_REL
	m_editableStyles << LVGL::Body; // LV_TABVIEW_STYLE_BTN_PR
	m_editableStyles << LVGL::Body; // LV_TABVIEW_STYLE_BTN_TGL_REL
	m_editableStyles << LVGL::Body; // LV_TABVIEW_STYLE_BTN_TGL_PR
}

QString LVGLTabview::name() const
{
	return "Tabview";
}

QString LVGLTabview::className() const
{
	return "lv_tabview";
}

LVGLWidget::Type LVGLTabview::type() const
{
	return TabView;
}

QIcon LVGLTabview::icon() const
{
	return QIcon();
}

lv_obj_t *LVGLTabview::newObject(lv_obj_t *parent) const
{
	lv_obj_t *obj = lv_tabview_create(parent, nullptr);
	return obj;
}

QSize LVGLTabview::minimumSize() const
{
	return QSize(100, 150);
}

QStringList LVGLTabview::styles() const
{
	return QStringList() << "LV_TABVIEW_STYLE_BG"
								<< "LV_TABVIEW_STYLE_INDIC"
								<< "LV_TABVIEW_STYLE_BTN_BG"
								<< "LV_TABVIEW_STYLE_BTN_REL"
								<< "LV_TABVIEW_STYLE_BTN_PR"
								<< "LV_TABVIEW_STYLE_BTN_TGL_REL"
								<< "LV_TABVIEW_STYLE_BTN_TGL_PR";
}

lv_style_t *LVGLTabview::style(lv_obj_t *obj, int type) const
{
	return const_cast<lv_style_t*>(lv_tabview_get_style(obj, type & 0xff));
}

void LVGLTabview::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const
{
	lv_tabview_set_style(obj, static_cast<lv_tabview_style_t>(type), style);
}

lv_style_t *LVGLTabview::defaultStyle(int type) const
{
	if (type == LV_TABVIEW_STYLE_BG)
		return &lv_style_plain;
	else if (type == LV_TABVIEW_STYLE_INDIC)
		return &lv_style_plain_color;
	else if (type == LV_TABVIEW_STYLE_BTN_BG)
		return &lv_style_transp;
	else if (type == LV_TABVIEW_STYLE_BTN_REL)
		return &lv_style_btn_rel;
	else if (type == LV_TABVIEW_STYLE_BTN_PR)
		return &lv_style_btn_pr;
	else if (type == LV_TABVIEW_STYLE_BTN_TGL_REL)
		return &lv_style_btn_tgl_rel;
	else if (type == LV_TABVIEW_STYLE_BTN_TGL_PR)
		return &lv_style_btn_tgl_pr;
	return nullptr;
}
