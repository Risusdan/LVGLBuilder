#ifndef LVGLPROPERTYPAGESCROLLPROPAGATION_H
#define LVGLPROPERTYPAGESCROLLPROPAGATION_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyPageScrollPropagation : public LVGLPropertyBool
{
public:
	QString name() const { return "Scroll propagation"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_page_set_scroll_propagation(%1, %2);").arg(obj->codeName()).arg(QVariant(get(obj)).toString());
	}

protected:
	bool get(LVGLObject *obj) const { return lv_page_get_scroll_propagation(obj->obj()); }
	void set(LVGLObject *obj, bool boolean) { lv_page_set_scroll_propagation(obj->obj(), boolean); }
};

#endif // LVGLPROPERTYPAGESCROLLPROPAGATION_H
