#ifndef LVGLPROPERTYPRELOADTYPE_H
#define LVGLPROPERTYPRELOADTYPE_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyPreloadType : public LVGLPropertyEnum
{
public:
	LVGLPropertyPreloadType()
		: LVGLPropertyEnum(QStringList() << "Spinning arc" << "Fill and spin arc")
	{}

	QString name() const { return "Spin type"; }

protected:
	int get(LVGLObject *obj) const { return lv_preload_get_type(obj->obj()); }
	void set(LVGLObject *obj, int index) { lv_preload_set_type(obj->obj(), index & 0xff); }
};

#endif // LVGLPROPERTYPRELOADTYPE_H
