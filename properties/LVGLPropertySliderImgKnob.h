#ifndef LVGLPROPERTYSLIDERIMGKNOB_H
#define LVGLPROPERTYSLIDERIMGKNOB_H

/**
 * @file LVGLPropertySliderImgKnob.h
 * @brief LVGLProperty subclass for image-slider knob image source.
 */

#include "LVGLPropertyImage.h"
#include "LVGLObject.h"
#include "LVGLCore.h"

class LVGLPropertySliderImgKnob : public LVGLPropertyImage
{
public:
	inline QString name() const override { return "Image knob"; }

	QStringList function(LVGLObject *obj) const override {
		LVGLImageData *img = lvgl.imageByDesc(get(obj));
		if (img == nullptr) return QStringList();
		return QStringList() << QString("lv_imgslider_set_knob_src(%1, &%2);").arg(obj->codeName()).arg(img->codeName());
	}

protected:
	const lv_img_dsc_t *get(LVGLObject *obj) const override {
		return reinterpret_cast<const lv_img_dsc_t *>(lv_imgslide_get_knob_src(obj->obj()));
	}
	void set(LVGLObject *obj, const lv_img_dsc_t *img) override { lv_imgslider_set_knob_src(obj->obj(), img); }
};

#endif // LVGLPROPERTYSLIDERIMGKNOB_H
