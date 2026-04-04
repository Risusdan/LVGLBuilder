#ifndef LVGLPROPERTYTAEDGEFLASH_H
#define LVGLPROPERTYTAEDGEFLASH_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyTAEdgeFlash : public LVGLPropertyBool
{
public:
	QString name() const { return "Edge flash"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_ta_set_edge_flash(%1, %2);").arg(obj->codeName()).arg(QVariant(get(obj)).toString());
	}

protected:
	bool get(LVGLObject *obj) const { return lv_ta_get_edge_flash(obj->obj()); }
	void set(LVGLObject *obj, bool boolean) { lv_ta_set_edge_flash(obj->obj(), boolean); }
};

#endif // LVGLPROPERTYTAEDGEFLASH_H
