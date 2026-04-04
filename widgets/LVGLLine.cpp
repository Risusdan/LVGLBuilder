#include "LVGLLine.h"

#include <QIcon>
#include "LVGLObject.h"
#include "properties/LVGLPropertyPoints.h"
#include "properties/LVGLPropertyLineAutoSize.h"
#include "properties/LVGLPropertyLineInvertY.h"
#include "properties/LVGLPropertyLinePoints.h"


LVGLLine::LVGLLine()
{
	m_properties << new LVGLPropertyLineAutoSize;
	m_properties << new LVGLPropertyLineInvertY;
	m_properties << new LVGLPropertyLinePoints;

	m_editableStyles << LVGL::Line; // LV_LINE_STYLE_MAIN
}

QString LVGLLine::name() const
{
	return "Line";
}

QString LVGLLine::className() const
{
	return "lv_line";
}

LVGLWidget::Type LVGLLine::type() const
{
	return Line;
}

QIcon LVGLLine::icon() const
{
	return QIcon();
}

lv_obj_t *LVGLLine::newObject(lv_obj_t *parent) const
{
	lv_obj_t *obj = lv_line_create(parent, nullptr);
	return obj;
}

QSize LVGLLine::minimumSize() const
{
	return QSize(200, 30);
}

QStringList LVGLLine::styles() const
{
	return QStringList() << "LV_LINE_STYLE_MAIN";
}

lv_style_t *LVGLLine::style(lv_obj_t *obj, int type) const
{
	return const_cast<lv_style_t*>(lv_line_get_style(obj, type & 0xff));
}

void LVGLLine::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const
{
	lv_line_set_style(obj, static_cast<lv_bar_style_t>(type), style);
}

lv_style_t *LVGLLine::defaultStyle(int type) const
{
	if (type == LV_LINE_STYLE_MAIN)
		return &lv_style_pretty;
	return nullptr;
}
