#ifndef LVGLPROPERTYPAGEWIDTH_H
#define LVGLPROPERTYPAGEWIDTH_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyPageWidth : public LVGLPropertyCoord
{
public:
	inline LVGLPropertyPageWidth(LVGLProperty *p = nullptr) : LVGLPropertyCoord(Qt::Horizontal, p) {}
	inline QString name() const override { return "Scroll width"; }

	inline lv_coord_t get(LVGLObject *obj) const override { return lv_page_get_scrl_width(obj->obj()); }
	inline void set(LVGLObject *obj, lv_coord_t value) override { lv_page_set_scrl_width(obj->obj(), value); }

};

#endif // LVGLPROPERTYPAGEWIDTH_H
