#ifndef LVGLPROPERTYTACURSORBLINKTIME_H
#define LVGLPROPERTYTACURSORBLINKTIME_H

/**
 * @file LVGLPropertyTACursorBlinkTime.h
 * @brief LVGLProperty subclass for lv_ta cursor blink interval (ms).
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyTACursorBlinkTime : public LVGLPropertyInt
{
public:
	LVGLPropertyTACursorBlinkTime() : LVGLPropertyInt(0, UINT16_MAX) {}

	QString name() const { return "Cursor blink time"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_ta_set_cursor_blink_time(%1, %2);").arg(obj->codeName()).arg(get(obj));
	}

protected:
	int get(LVGLObject *obj) const { return lv_ta_get_cursor_blink_time(obj->obj()); }
	void set(LVGLObject *obj, int index) { lv_ta_set_cursor_blink_time(obj->obj(), index & 0xff); }
};

#endif // LVGLPROPERTYTACURSORBLINKTIME_H
