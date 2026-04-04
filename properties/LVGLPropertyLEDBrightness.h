#ifndef LVGLPROPERTYLEDBRIGHTNESS_H
#define LVGLPROPERTYLEDBRIGHTNESS_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyLEDBrightness : public LVGLPropertyInt
{
public:
	LVGLPropertyLEDBrightness() : LVGLPropertyInt(0, 255) {}

	QString name() const { return "Brightness"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_led_set_bright(%1, %2);").arg(obj->codeName()).arg(get(obj));
	}

protected:
	int get(LVGLObject *obj) const { return lv_led_get_bright(obj->obj()); }
	void set(LVGLObject *obj, int value) { lv_led_set_bright(obj->obj(), static_cast<uint8_t>(value)); }
};

#endif // LVGLPROPERTYLEDBRIGHTNESS_H
