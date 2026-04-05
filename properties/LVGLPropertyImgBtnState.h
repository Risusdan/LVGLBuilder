#ifndef LVGLPROPERTYIMGBTNSTATE_H
#define LVGLPROPERTYIMGBTNSTATE_H

/**
 * @file LVGLPropertyImgBtnState.h
 * @brief LVGLProperty subclass for lv_imgbtn active button state.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyImgBtnState : public LVGLPropertyEnum
{
public:
	LVGLPropertyImgBtnState()
		: LVGLPropertyEnum(QStringList() << "Released" << "Pressed" << "Toggled released" << "Toggled pressed" << "Inactive")
		, m_values({"LV_BTN_STATE_REL", "LV_BTN_STATE_PR", "LV_BTN_STATE_TGL_REL", "LV_BTN_STATE_TGL_PR", "LV_BTN_STATE_INA"})
	{}

	QString name() const { return "State"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_imgbtn_set_state(%1, %2);").arg(obj->codeName()).arg(m_values.at(get(obj)));
	}

protected:
	int get(LVGLObject *obj) const { return lv_imgbtn_get_state(obj->obj()); }
	void set(LVGLObject *obj, int index) { lv_imgbtn_set_state(obj->obj(), index & 0xff); }

	QStringList m_values;
};

#endif // LVGLPROPERTYIMGBTNSTATE_H
