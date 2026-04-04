#ifndef LVGLPROPERTYDDLISTFIXEDHEIGHT_H
#define LVGLPROPERTYDDLISTFIXEDHEIGHT_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyDDListFixedHeight : public LVGLPropertyCoord
{
public:
	inline LVGLPropertyDDListFixedHeight(LVGLProperty *p = nullptr) : LVGLPropertyCoord(Qt::Vertical, p) {}
	inline QString name() const override { return "Fixed height"; }

protected:
	inline lv_coord_t get(LVGLObject *obj) const override { return lv_ddlist_get_fix_height(obj->obj()); }
	inline void set(LVGLObject *obj, lv_coord_t value) override { lv_ddlist_set_fix_height(obj->obj(), value); }

};

#endif // LVGLPROPERTYDDLISTFIXEDHEIGHT_H
