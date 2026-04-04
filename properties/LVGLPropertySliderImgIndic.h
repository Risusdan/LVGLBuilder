#ifndef LVGLPROPERTYSLIDERIMGINDIC_H
#define LVGLPROPERTYSLIDERIMGINDIC_H

#include "LVGLPropertyImage.h"
#include "LVGLObject.h"
#include "LVGLCore.h"

class LVGLPropertySliderImgIndic : public LVGLPropertyImage
{
public:
	inline QString name() const override { return "Image indicator"; }

	QStringList function(LVGLObject *obj) const override {
		LVGLImageData *img = lvgl.imageByDesc(get(obj));
		if (img == nullptr) return QStringList();
		return QStringList() << QString("lv_imgslide_get_indic_src(%1, &%2);").arg(obj->codeName()).arg(img->codeName());
	}

protected:
	const lv_img_dsc_t *get(LVGLObject *obj) const override {
		return reinterpret_cast<const lv_img_dsc_t *>(lv_imgslide_get_indic_src(obj->obj()));
	}
	void set(LVGLObject *obj, const lv_img_dsc_t *img) override { lv_imgslider_set_indic_src(obj->obj(), img); }
};

#endif // LVGLPROPERTYSLIDERIMGINDIC_H
