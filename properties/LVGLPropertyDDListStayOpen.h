#ifndef LVGLPROPERTYDDLISTSTAYOPEN_H
#define LVGLPROPERTYDDLISTSTAYOPEN_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyDDListStayOpen : public LVGLPropertyBool
{
public:
	QString name() const { return "Stay open"; }

protected:
	bool get(LVGLObject *obj) const { return lv_ddlist_get_stay_open(obj->obj()); }
	void set(LVGLObject *obj, bool statue) { lv_ddlist_set_stay_open(obj->obj(), statue); }
};

#endif // LVGLPROPERTYDDLISTSTAYOPEN_H
