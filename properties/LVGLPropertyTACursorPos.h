#ifndef LVGLPROPERTYTACURSORPOS_H
#define LVGLPROPERTYTACURSORPOS_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyTACursorPos : public LVGLPropertyInt
{
public:
	LVGLPropertyTACursorPos() : LVGLPropertyInt(0, LV_TA_CURSOR_LAST) {}

	QString name() const { return "Cursor position"; }

protected:
	int get(LVGLObject *obj) const { return lv_ta_get_cursor_pos(obj->obj()); }
	void set(LVGLObject *obj, int value) { lv_ta_set_cursor_pos(obj->obj(), static_cast<int16_t>(value)); }
};

#endif // LVGLPROPERTYTACURSORPOS_H
