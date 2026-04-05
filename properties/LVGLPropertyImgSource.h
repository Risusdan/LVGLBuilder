#ifndef LVGLPROPERTYIMGSOURCE_H
#define LVGLPROPERTYIMGSOURCE_H

/**
 * @file LVGLPropertyImgSource.h
 * @brief LVGLProperty subclass for lv_img image source asset.
 */

#include "LVGLPropertyImage.h"
#include "LVGLObject.h"
#include "LVGLCore.h"

class LVGLPropertyImgSource : public LVGLPropertyImage
{
public:
	inline QString name() const override { return "Source"; }

	QStringList function(LVGLObject *obj) const override {
		LVGLImageData *img = lvgl.imageByDesc(get(obj));
		if (img == nullptr) return QStringList();
		return QStringList() << QString("lv_img_set_src(%1, &%2);").arg(obj->codeName()).arg(img->codeName());
	}

protected:
	const lv_img_dsc_t *get(LVGLObject *obj) const override {
		return reinterpret_cast<const lv_img_dsc_t*>(lv_img_get_src(obj->obj()));
	}
	void set(LVGLObject *obj, const lv_img_dsc_t *img) override { lv_img_set_src(obj->obj(), img); }
};

#endif // LVGLPROPERTYIMGSOURCE_H
