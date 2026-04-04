#ifndef LVGLPROPERTYDDLISTFIXEDWIDTH_H
#define LVGLPROPERTYDDLISTFIXEDWIDTH_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyDDListFixedWidth : public LVGLPropertyCoord
{
public:
	inline LVGLPropertyDDListFixedWidth(LVGLProperty *p = nullptr) : LVGLPropertyCoord(Qt::Vertical, p) {}
	inline QString name() const override { return "Fixed width"; }

protected:
	inline lv_coord_t get(LVGLObject *obj) const override { return lv_obj_get_width(obj->obj()); }
	inline void set(LVGLObject *obj, lv_coord_t value) override { lv_ddlist_set_fix_width(obj->obj(), value); }

};

#endif // LVGLPROPERTYDDLISTFIXEDWIDTH_H
