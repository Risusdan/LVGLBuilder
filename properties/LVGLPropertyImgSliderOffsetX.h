#ifndef LVGLPROPERTYIMGSLIDEROFFSETX_H
#define LVGLPROPERTYIMGSLIDEROFFSETX_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyImgSliderOffsetX : public LVGLPropertyCoord
{
public:
	inline LVGLPropertyImgSliderOffsetX(LVGLProperty *p) : LVGLPropertyCoord(Qt::Horizontal, p) {}
	inline QString name() const override { return "X"; }

	inline lv_coord_t get(LVGLObject *obj) const override { return lv_imgslider_get_offset_x(obj->obj()); }
	inline void set(LVGLObject *obj, lv_coord_t value) override { lv_imgslider_set_offset_x(obj->obj(), value); }

};

#endif // LVGLPROPERTYIMGSLIDEROFFSETX_H
