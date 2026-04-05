#ifndef LVGLPROPERTYTASCROLLBARS_H
#define LVGLPROPERTYTASCROLLBARS_H

/**
 * @file LVGLPropertyTAScrollbars.h
 * @brief LVGLProperty subclass for lv_ta scrollbar visibility mode.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyTAScrollbars : public LVGLPropertyEnum
{
public:
	LVGLPropertyTAScrollbars()
		: LVGLPropertyEnum(QStringList() << "Off" << "On" << "Drag" << "Auto") {}

	QString name() const { return "Scrollbars"; }

protected:
	int get(LVGLObject *obj) const { return lv_ta_get_sb_mode(lv_ta_get_label(obj->obj())) & 0x3;  }
	void set(LVGLObject *obj, int index) { lv_ta_set_sb_mode(obj->obj(), index & 0xff); }
};

#endif // LVGLPROPERTYTASCROLLBARS_H
