#ifndef LVGLPROPERTYIMGBTNSRC_H
#define LVGLPROPERTYIMGBTNSRC_H

#include "LVGLPropertyImage.h"
#include "LVGLObject.h"
#include "LVGLCore.h"

class LVGLPropertyImgBtnSrc : public LVGLPropertyImage
{
public:
	LVGLPropertyImgBtnSrc(lv_btn_state_t state, LVGLProperty *parent = nullptr)
		: LVGLPropertyImage(parent)
		, m_values({"Released", "Pressed", "Toggled released", "Toggled pressed", "Inactive"})
		, m_types({"LV_BTN_STATE_REL", "LV_BTN_STATE_PR", "LV_BTN_STATE_TGL_REL", "LV_BTN_STATE_TGL_PR", "LV_BTN_STATE_INA"})
		, m_state(state)
	{
	}

	QString name() const override { return "Source " + m_values.at(m_state); }

	QStringList function(LVGLObject *obj) const override {
		LVGLImageData *img = lvgl.imageByDesc(get(obj));
		if (img == nullptr) return QStringList();
		return QStringList() << QString("lv_imgbtn_set_src(%1, %2, &%3);").arg(obj->codeName()).arg(m_types.at(m_state)).arg(img->codeName());
	}

protected:
	QStringList m_values;
	QStringList m_types;
	lv_btn_state_t m_state;

	virtual const lv_img_dsc_t *get(LVGLObject *obj) const override { return reinterpret_cast<const lv_img_dsc_t*>(lv_imgbtn_get_src(obj->obj(), m_state)); }
	virtual void set(LVGLObject *obj, const lv_img_dsc_t *img) override { lv_imgbtn_set_src(obj->obj(), m_state, img); }

};

#endif // LVGLPROPERTYIMGBTNSRC_H
