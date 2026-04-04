#ifndef LVGLPROPERTYPRELOADDIR_H
#define LVGLPROPERTYPRELOADDIR_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyPreloadDir : public LVGLPropertyEnum
{
public:
	LVGLPropertyPreloadDir()
		: LVGLPropertyEnum(QStringList() << "Forward" << "Backward")
	{}

	QString name() const { return "Spin direction"; }

protected:
	int get(LVGLObject *obj) const { return lv_preload_get_dir(obj->obj()); }
	void set(LVGLObject *obj, int index) { lv_preload_set_dir(obj->obj(), index & 0xff); }
};

#endif // LVGLPROPERTYPRELOADDIR_H
