#ifndef LVGLPROPERTYTABBTNHIDE_H
#define LVGLPROPERTYTABBTNHIDE_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyTabBtnHide : public LVGLPropertyBool
{
public:
	QString name() const { return "Hide buttons"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_tabview_set_btns_hidden(%1, %2);").arg(obj->codeName()).arg(QVariant(get(obj)).toString());
	}

protected:
	bool get(LVGLObject *obj) const { return lv_tabview_get_btns_hidden(obj->obj()); }
	void set(LVGLObject *obj, bool boolean) { lv_tabview_set_btns_hidden(obj->obj(), boolean); }
};

#endif // LVGLPROPERTYTABBTNHIDE_H
