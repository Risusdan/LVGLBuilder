#include "LVGLLabel.h"

#include <QIcon>
#include "LVGLObject.h"
#include "properties/LVGLPropertyColor.h"
#include "properties/LVGLPropertyLabelAlign.h"
#include "properties/LVGLPropertyLabelLongMode.h"
#include "properties/LVGLPropertyLabelRecolor.h"
#include "properties/LVGLPropertyLabelBodyDraw.h"


LVGLLabel::LVGLLabel()
{
	m_properties << new LVGLPropertyLabelAlign;
	m_properties << new LVGLPropertyLabelLongMode;
	m_properties << new LVGLPropertyLabelRecolor;
	m_properties << new LVGLPropertyLabelBodyDraw;
	m_properties << new LVGLPropertyString("Text", "lv_label_set_text", lv_label_set_text, lv_label_get_text);

	// swap geometry in order to stop autosize
	const int index = m_properties.indexOf(m_geometryProp);
	qSwap(m_properties[index], m_properties.last());

	m_editableStyles << LVGL::Text; // LV_LABEL_STYLE_MAIN
}

QString LVGLLabel::name() const
{
	return "Label";
}

QString LVGLLabel::className() const
{
	return "lv_label";
}

LVGLWidget::Type LVGLLabel::type() const
{
	return Label;
}

QIcon LVGLLabel::icon() const
{
	return QIcon();
}

lv_obj_t *LVGLLabel::newObject(lv_obj_t *parent) const
{
	lv_obj_t *obj = lv_label_create(parent, nullptr);
	return obj;
}

QSize LVGLLabel::minimumSize() const
{
	return QSize(100, 35);
}

QStringList LVGLLabel::styles() const
{
	return QStringList() << "LV_LABEL_STYLE_MAIN";
}

lv_style_t *LVGLLabel::style(lv_obj_t *obj, int type) const
{
	return const_cast<lv_style_t*>(lv_label_get_style(obj, type & 0xff));
}

void LVGLLabel::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const
{
	lv_label_set_style(obj, static_cast<lv_label_style_t>(type), style);
}

lv_style_t *LVGLLabel::defaultStyle(int type) const
{
	Q_UNUSED(type)
	return nullptr;
}
