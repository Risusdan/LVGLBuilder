#ifndef LVGLPROPERTYIMGOFFSETX_H
#define LVGLPROPERTYIMGOFFSETX_H

/**
 * @file LVGLPropertyImgOffsetX.h
 * @brief LVGLProperty subclass for lv_img horizontal scroll offset.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyImgOffsetX : public LVGLPropertyCoord
{
public:
	inline LVGLPropertyImgOffsetX(LVGLProperty *p) : LVGLPropertyCoord(Qt::Horizontal, p) {}
	inline QString name() const override { return "X"; }

	inline lv_coord_t get(LVGLObject *obj) const override { return lv_img_get_offset_x(obj->obj()); }
	inline void set(LVGLObject *obj, lv_coord_t value) override { lv_img_set_offset_x(obj->obj(), value); }

};

#endif // LVGLPROPERTYIMGOFFSETX_H
