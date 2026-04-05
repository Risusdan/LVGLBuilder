#ifndef LVGLPROPERTYGAUGEVAL_H
#define LVGLPROPERTYGAUGEVAL_H

/**
 * @file LVGLPropertyGaugeVal.h
 * @brief LVGLProperty subclass for lv_gauge needle value.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyGaugeVal : public LVGLPropertyInt
{
public:
	inline LVGLPropertyGaugeVal() : LVGLPropertyInt(INT16_MIN, INT16_MAX) {}

	inline QString name() const override { return "Value"; }

	inline QStringList function(LVGLObject *obj) const override {
		return { QString("lv_gauge_set_value(%1, 0, %2);").arg(obj->codeName()).arg(get(obj)) };
	}

protected:
	inline int get(LVGLObject *obj) const override { return lv_gauge_get_value(obj->obj(), 0); }
	inline void set(LVGLObject *obj, int value) override { lv_gauge_set_value(obj->obj(), 0, static_cast<int16_t>(value)); }
};

#endif // LVGLPROPERTYGAUGEVAL_H
