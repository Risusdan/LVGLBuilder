#ifndef LVGLPROPERTYLISTSINGLEMODE_H
#define LVGLPROPERTYLISTSINGLEMODE_H

/**
 * @file LVGLPropertyListSingleMode.h
 * @brief LVGLProperty subclass for lv_list single-selection mode flag.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyListSingleMode : public LVGLPropertyBool
{
public:
	QString name() const { return "Single mode"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_list_set_single_mode(%1, %2);").arg(obj->codeName()).arg(QVariant(get(obj)).toString());
	}

protected:
	bool get(LVGLObject *obj) const { return lv_list_get_single_mode(obj->obj()); }
	void set(LVGLObject *obj, bool boolean) { lv_list_set_single_mode(obj->obj(), boolean); }
};

#endif // LVGLPROPERTYLISTSINGLEMODE_H
