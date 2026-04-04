#ifndef LVGLPROPERTYTAMAXLEN_H
#define LVGLPROPERTYTAMAXLEN_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyTAMaxLen : public LVGLPropertyInt
{
public:
	LVGLPropertyTAMaxLen() : LVGLPropertyInt(0, LV_TA_CURSOR_LAST-1) {}

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_ta_set_max_length(%1, %2);").arg(obj->codeName()).arg(get(obj));
	}

	QString name() const { return "Max length"; }

protected:
	int get(LVGLObject *obj) const { return lv_ta_get_max_length(obj->obj()); }
	void set(LVGLObject *obj, int index) { lv_ta_set_max_length(obj->obj(), index & 0xff); }
};

#endif // LVGLPROPERTYTAMAXLEN_H
