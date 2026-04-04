#ifndef LVGLPROPERTYTABSLIDING_H
#define LVGLPROPERTYTABSLIDING_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyTabSliding : public LVGLPropertyBool
{
public:
	QString name() const { return "Sliding"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_tabview_set_sliding(%1, %2);").arg(obj->codeName()).arg(QVariant(get(obj)).toString());
	}

protected:
	bool get(LVGLObject *obj) const { return lv_tabview_get_sliding(obj->obj()); }
	void set(LVGLObject *obj, bool boolean) { lv_tabview_set_sliding(obj->obj(), boolean); }
};

#endif // LVGLPROPERTYTABSLIDING_H
