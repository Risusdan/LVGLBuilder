#ifndef LVGLPROPERTYTASCROLLPROPAGATION_H
#define LVGLPROPERTYTASCROLLPROPAGATION_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyTAScrollPropagation : public LVGLPropertyBool
{
public:
	QString name() const { return "Scroll propagation"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_ta_set_scroll_propagation(%1, %2);").arg(obj->codeName()).arg(QVariant(get(obj)).toString());
	}

protected:
	bool get(LVGLObject *obj) const { return lv_ta_get_scroll_propagation(obj->obj()); }
	void set(LVGLObject *obj, bool boolean) { lv_ta_set_scroll_propagation(obj->obj(), boolean); }
};

#endif // LVGLPROPERTYTASCROLLPROPAGATION_H
