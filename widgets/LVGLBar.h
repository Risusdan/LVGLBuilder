#ifndef LVGLBAR_H
#define LVGLBAR_H

/**
 * @file LVGLBar.h
 * @brief LVGLWidget subclass for lv_bar — progress bar with min/max range.
 */

#include "LVGLWidget.h"

class LVGLBar : public LVGLWidget
{
public:
	LVGLBar();

	virtual QString name() const;
	virtual QString className() const;
	virtual Type type() const;
	virtual QIcon icon() const;
	virtual lv_obj_t *newObject(lv_obj_t *parent) const;
	virtual QSize minimumSize() const;
	virtual QStringList styles() const;
	virtual lv_style_t *style(lv_obj_t *obj, int type) const;
	virtual void setStyle(lv_obj_t *obj, int type, lv_style_t *style) const;
	virtual lv_style_t *defaultStyle(int type) const;

};

#endif // LVGLBAR_H
