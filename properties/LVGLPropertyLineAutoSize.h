#ifndef LVGLPROPERTYLINEAUTOSIZE_H
#define LVGLPROPERTYLINEAUTOSIZE_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyLineAutoSize : public LVGLPropertyBool
{
public:
	QString name() const { return "Auto size"; }

protected:
	bool get(LVGLObject *obj) const { return lv_line_get_auto_size(obj->obj()); }
	void set(LVGLObject *obj, bool statue) { lv_line_set_auto_size(obj->obj(), statue); }
};

#endif // LVGLPROPERTYLINEAUTOSIZE_H
