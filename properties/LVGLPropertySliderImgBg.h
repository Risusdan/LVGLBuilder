#ifndef LVGLPROPERTYSLIDERIMGBG_H
#define LVGLPROPERTYSLIDERIMGBG_H

/**
 * @file LVGLPropertySliderImgBg.h
 * @brief LVGLProperty subclass for image-slider background image source.
 */

#include "LVGLPropertyImage.h"
#include "LVGLObject.h"
#include "LVGLCore.h"

class LVGLPropertySliderImgBg : public LVGLPropertyImage
{
public:
	inline QString name() const override { return "Image background"; }

	QStringList function(LVGLObject *obj) const override {
		LVGLImageData *img = lvgl.imageByDesc(get(obj));
		if (img == nullptr) return QStringList();
		return QStringList() << QString("lv_imgslider_set_bg_src(%1, &%2);").arg(obj->codeName()).arg(img->codeName());
	}

protected:
	const lv_img_dsc_t *get(LVGLObject *obj) const override {
		return reinterpret_cast<const lv_img_dsc_t *>(lv_imgslide_get_bg_src(obj->obj()));
	}
	void set(LVGLObject *obj, const lv_img_dsc_t *img) override { lv_imgslider_set_bg_src(obj->obj(), img); }
};

#endif // LVGLPROPERTYSLIDERIMGBG_H
