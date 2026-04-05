#ifndef LVGLPROPERTYTAONELINEMODE_H
#define LVGLPROPERTYTAONELINEMODE_H

/**
 * @file LVGLPropertyTAOneLineMode.h
 * @brief LVGLProperty subclass for lv_ta single-line mode flag.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyTAOneLineMode : public LVGLPropertyBool
{
public:
	QString name() const { return "One line mode"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_ta_set_one_line(%1, %2);").arg(obj->codeName()).arg(QVariant(get(obj)).toString());
	}

protected:
	bool get(LVGLObject *obj) const { return lv_ta_get_one_line(obj->obj()); }
	void set(LVGLObject *obj, bool statue) { lv_ta_set_one_line(obj->obj(), statue); }
};

#endif // LVGLPROPERTYTAONELINEMODE_H
