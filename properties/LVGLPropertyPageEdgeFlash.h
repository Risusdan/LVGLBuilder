#ifndef LVGLPROPERTYPAGEEDGEFLASH_H
#define LVGLPROPERTYPAGEEDGEFLASH_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyPageEdgeFlash : public LVGLPropertyBool
{
public:
	QString name() const { return "Edge flash"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_page_set_edge_flash(%1, %2);").arg(obj->codeName()).arg(QVariant(get(obj)).toString());
	}

protected:
	bool get(LVGLObject *obj) const { return lv_page_get_edge_flash(obj->obj()); }
	void set(LVGLObject *obj, bool boolean) { lv_page_set_edge_flash(obj->obj(), boolean); }
};

#endif // LVGLPROPERTYPAGEEDGEFLASH_H
