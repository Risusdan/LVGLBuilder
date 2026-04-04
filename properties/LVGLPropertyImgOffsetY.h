#ifndef LVGLPROPERTYIMGOFFSETY_H
#define LVGLPROPERTYIMGOFFSETY_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyImgOffsetY : public LVGLPropertyCoord
{
public:
	inline LVGLPropertyImgOffsetY(LVGLProperty *p) : LVGLPropertyCoord(Qt::Vertical, p) {}
	inline QString name() const override { return "Y"; }

	inline lv_coord_t get(LVGLObject *obj) const override { return lv_img_get_offset_y(obj->obj()); }
	inline void set(LVGLObject *obj, lv_coord_t value) override { lv_img_set_offset_y(obj->obj(), value); }

};

#endif // LVGLPROPERTYIMGOFFSETY_H
