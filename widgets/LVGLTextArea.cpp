#include "LVGLTextArea.h"

#include <QIcon>
#include "LVGLObject.h"
#include "properties/LVGLPropertyTAText.h"
#include "properties/LVGLPropertyTAPlaceholder.h"
#include "properties/LVGLPropertyTACursorPos.h"
#include "properties/LVGLPropertyTACursorType.h"
#include "properties/LVGLPropertyTACursorBlinkTime.h"
#include "properties/LVGLPropertyTAOneLineMode.h"
#include "properties/LVGLPropertyTAPasswordMode.h"
#include "properties/LVGLPropertyTAMaxLen.h"
#include "properties/LVGLPropertyTATextAlign.h"
#include "properties/LVGLPropertyTAScrollbars.h"
#include "properties/LVGLPropertyTAScrollPropagation.h"
#include "properties/LVGLPropertyTAEdgeFlash.h"


LVGLTextArea::LVGLTextArea()
{
	m_properties << new LVGLPropertyTAText;
	m_properties << new LVGLPropertyTAPlaceholder;
	m_properties << new LVGLPropertyTACursorPos;
	m_properties << new LVGLPropertyTACursorType;
	m_properties << new LVGLPropertyTACursorBlinkTime;
	m_properties << new LVGLPropertyTAOneLineMode;
	m_properties << new LVGLPropertyTAPasswordMode;
	m_properties << new LVGLPropertyTAMaxLen;
	m_properties << new LVGLPropertyTATextAlign;
	m_properties << new LVGLPropertyTAScrollbars;
	m_properties << new LVGLPropertyTAScrollPropagation;
	m_properties << new LVGLPropertyTAEdgeFlash;

	m_editableStyles << LVGL::StyleParts(LVGL::Body | LVGL::Text); // LV_TA_STYLE_BG
	m_editableStyles << LVGL::Body; // LV_TA_STYLE_SB
	m_editableStyles << LVGL::StyleParts(LVGL::BodyPadding | LVGL::Line); // LV_TA_STYLE_CURSOR
	m_editableStyles << LVGL::Body; // LV_TA_STYLE_EDGE_FLASH
	m_editableStyles << LVGL::StyleParts(LVGL::Body | LVGL::Text); // LV_TA_STYLE_PLACEHOLDER
}

QString LVGLTextArea::name() const
{
	return "Text area";
}

QString LVGLTextArea::className() const
{
	return "lv_ta";
}

LVGLWidget::Type LVGLTextArea::type() const
{
	return TextArea;
}

QIcon LVGLTextArea::icon() const
{
	return QIcon();
}

lv_obj_t *LVGLTextArea::newObject(lv_obj_t *parent) const
{
	lv_obj_t *obj = lv_ta_create(parent, nullptr);
	return obj;
}

QSize LVGLTextArea::minimumSize() const
{
	return QSize(100, 30);
}

QStringList LVGLTextArea::styles() const
{
	return QStringList() << "LV_TA_STYLE_BG"
								<< "LV_TA_STYLE_SB"
								<< "LV_TA_STYLE_CURSOR"
								<< "LV_TA_STYLE_EDGE_FLASH"
								<< "LV_TA_STYLE_PLACEHOLDER";
}

lv_style_t *LVGLTextArea::style(lv_obj_t *obj, int type) const
{
	return const_cast<lv_style_t*>(lv_ta_get_style(obj, type & 0xff));
}

void LVGLTextArea::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const
{
	lv_ta_set_style(obj, static_cast<lv_ta_style_t>(type), style);
}

lv_style_t *LVGLTextArea::defaultStyle(int type) const
{
	if (type == LV_TA_STYLE_BG)
		return &lv_style_pretty;
	else if (type == LV_TA_STYLE_SB)
		return &lv_style_pretty_color;
	return nullptr;
}
