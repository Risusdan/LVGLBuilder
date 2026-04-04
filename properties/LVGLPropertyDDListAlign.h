#ifndef LVGLPROPERTYDDLISTALIGN_H
#define LVGLPROPERTYDDLISTALIGN_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyDDListAlign : public LVGLPropertyEnum
{
public:
	LVGLPropertyDDListAlign() : LVGLPropertyEnum(QStringList() << "Left" << "Center" << "Right") {}
	QString name() const { return "Align"; }

protected:
	int get(LVGLObject *obj) const { return lv_ddlist_get_align(obj->obj()); }
	void set(LVGLObject *obj, int index) { lv_ddlist_set_align(obj->obj(), index & 0xff); }
};

#endif // LVGLPROPERTYDDLISTALIGN_H
