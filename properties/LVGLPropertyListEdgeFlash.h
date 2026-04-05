#ifndef LVGLPROPERTYLISTEDGEFLASH_H
#define LVGLPROPERTYLISTEDGEFLASH_H

/**
 * @file LVGLPropertyListEdgeFlash.h
 * @brief LVGLProperty subclass for lv_list edge-flash animation flag.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyListEdgeFlash : public LVGLPropertyBool
{
public:
	QString name() const { return "Edge flash"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_list_set_edge_flash(%1, %2);").arg(obj->codeName()).arg(QVariant(get(obj)).toString());
	}

protected:
	bool get(LVGLObject *obj) const { return lv_list_get_edge_flash(obj->obj()); }
	void set(LVGLObject *obj, bool boolean) { lv_list_set_edge_flash(obj->obj(), boolean); }
};

#endif // LVGLPROPERTYLISTEDGEFLASH_H
