#ifndef LVGLPROPERTYTATEXT_H
#define LVGLPROPERTYTATEXT_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyTAText : public LVGLPropertyString
{
public:
	QString name() const { return "Text"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_ta_set_text(%1, \"%2\");").arg(obj->codeName()).arg(get(obj));
	}

protected:
	QString get(LVGLObject *obj) const { return lv_ta_get_text(obj->obj()); }
	void set(LVGLObject *obj, QString string) { lv_ta_set_text(obj->obj(), qPrintable(string)); }
};

#endif // LVGLPROPERTYTATEXT_H
