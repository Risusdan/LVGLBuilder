#ifndef LVGLPROPERTYIMGSLIDEROFFSETY_H
#define LVGLPROPERTYIMGSLIDEROFFSETY_H

/**
 * @file LVGLPropertyImgSliderOffsetY.h
 * @brief LVGLProperty subclass for image-slider knob vertical offset.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyImgSliderOffsetY : public LVGLPropertyCoord
{
public:
	inline LVGLPropertyImgSliderOffsetY(LVGLProperty *p) : LVGLPropertyCoord(Qt::Vertical, p) {}
	inline QString name() const override { return "Y"; }

	inline lv_coord_t get(LVGLObject *obj) const override { return lv_imgslider_get_offset_y(obj->obj()); }
	inline void set(LVGLObject *obj, lv_coord_t value) override { lv_imgslider_set_offset_y(obj->obj(), value); }

};

#endif // LVGLPROPERTYIMGSLIDEROFFSETY_H
