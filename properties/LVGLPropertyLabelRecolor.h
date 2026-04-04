#ifndef LVGLPROPERTYLABELRECOLOR_H
#define LVGLPROPERTYLABELRECOLOR_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyLabelRecolor : public LVGLPropertyBool
{
public:
	QString name() const { return "Recolor"; }

	QStringList function(LVGLObject *obj) const {
		if (!get(obj)) return QStringList();
		return QStringList() << QString("lv_label_set_recolor(%1, %2);").arg(obj->codeName()).arg(QVariant(get(obj)).toString());
	}

protected:
	bool get(LVGLObject *obj) const { return lv_label_get_recolor(obj->obj()); }
	void set(LVGLObject *obj, bool boolean) { lv_label_set_recolor(obj->obj(), boolean); }
};

#endif // LVGLPROPERTYLABELRECOLOR_H
