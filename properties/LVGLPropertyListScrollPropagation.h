#ifndef LVGLPROPERTYLISTSCROLLPROPAGATION_H
#define LVGLPROPERTYLISTSCROLLPROPAGATION_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyListScrollPropagation : public LVGLPropertyBool
{
public:
	QString name() const { return "Scroll propagation"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_list_set_scroll_propagation(%1, %2);").arg(obj->codeName()).arg(QVariant(get(obj)).toString());
	}

protected:
	bool get(LVGLObject *obj) const { return lv_list_get_scroll_propagation(obj->obj()); }
	void set(LVGLObject *obj, bool boolean) { lv_list_set_scroll_propagation(obj->obj(), boolean); }
};

#endif // LVGLPROPERTYLISTSCROLLPROPAGATION_H
