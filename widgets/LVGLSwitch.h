#ifndef LVGLSWITCH_H
#define LVGLSWITCH_H

/**
 * @file LVGLSwitch.h
 * @brief LVGLWidget subclass for lv_sw — on/off toggle switch.
 */

#include "LVGLWidget.h"

class LVGLSwitch : public LVGLWidget
{
public:
	LVGLSwitch();

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

#endif // LVGLSWITCH_H
