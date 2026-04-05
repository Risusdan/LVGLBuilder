#ifndef LVGLPROPERTYGAUGECRITICALVALUE_H
#define LVGLPROPERTYGAUGECRITICALVALUE_H

/**
 * @file LVGLPropertyGaugeCriticalValue.h
 * @brief LVGLProperty subclass for lv_gauge critical threshold value.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyGaugeCriticalValue : public LVGLPropertyInt
{
public:
	inline LVGLPropertyGaugeCriticalValue() : LVGLPropertyInt(INT16_MIN, INT16_MAX) {}

	inline QString name() const override { return "Critical value"; }

	inline QStringList function(LVGLObject *obj) const override {
		return { QString("lv_gauge_set_critical_value(%1, %2);").arg(obj->codeName()).arg(get(obj)) };
	}

protected:
	inline int get(LVGLObject *obj) const override { return lv_gauge_get_critical_value(obj->obj()); }
	inline void set(LVGLObject *obj, int value) override { lv_gauge_set_critical_value(obj->obj(), static_cast<int16_t>(value)); }
};

#endif // LVGLPROPERTYGAUGECRITICALVALUE_H
