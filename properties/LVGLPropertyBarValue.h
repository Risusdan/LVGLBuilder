#ifndef LVGLPROPERTYBARVALUE_H
#define LVGLPROPERTYBARVALUE_H

#include "LVGLProperty.h"
#include "LVGLObject.h"
#include "lvgl/lvgl.h"

class LVGLPropertyBarValue : public LVGLPropertyInt
{
public:
	LVGLPropertyBarValue() : LVGLPropertyInt(INT16_MIN, INT16_MAX) {}

	QString name() const { return "Value"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_bar_set_value(%1, %2, LV_ANIM_ON);").arg(obj->codeName()).arg(get(obj));
	}

protected:
	int get(LVGLObject *obj) const { return lv_bar_get_value(obj->obj()); }
	void set(LVGLObject *obj, int value) { lv_bar_set_value(obj->obj(), static_cast<int16_t>(value), LV_ANIM_OFF); }
};

#endif // LVGLPROPERTYBARVALUE_H
