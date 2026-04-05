#ifndef LVGLPROPERTYTABANIMTIME_H
#define LVGLPROPERTYTABANIMTIME_H

/**
 * @file LVGLPropertyTabAnimTime.h
 * @brief LVGLProperty subclass for lv_tabview tab-switch animation duration.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyTabAnimTime : public LVGLPropertyInt
{
public:
	LVGLPropertyTabAnimTime() : LVGLPropertyInt(0, UINT16_MAX) {}

	QString name() const { return "Animation time"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_preload_set_spin_time(%1, %2);").arg(obj->codeName()).arg(get(obj));
	}

protected:
	int get(LVGLObject *obj) const { return lv_preload_get_spin_time(obj->obj()); }
	void set(LVGLObject *obj, int value) { lv_preload_set_spin_time(obj->obj(), static_cast<uint16_t>(value)); }
};

#endif // LVGLPROPERTYTABANIMTIME_H
