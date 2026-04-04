#ifndef LVGLPROPERTYDDLISTSCROLLBARS_H
#define LVGLPROPERTYDDLISTSCROLLBARS_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyDDListScrollbars : public LVGLPropertyEnum
{
public:
	LVGLPropertyDDListScrollbars() : LVGLPropertyEnum(QStringList() << "Off"  << "On" << "Drag" << "Auto") {}
	QString name() const { return "Scrollbars"; }

protected:
	int get(LVGLObject *obj) const { return lv_ddlist_get_sb_mode(obj->obj()) & 0x3; }
	void set(LVGLObject *obj, int index) { lv_ddlist_set_sb_mode(obj->obj(), index & 0x3); }
};

#endif // LVGLPROPERTYDDLISTSCROLLBARS_H
