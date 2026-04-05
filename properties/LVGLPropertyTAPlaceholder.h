#ifndef LVGLPROPERTYTAPLACEHOLDER_H
#define LVGLPROPERTYTAPLACEHOLDER_H

/**
 * @file LVGLPropertyTAPlaceholder.h
 * @brief LVGLProperty subclass for lv_ta placeholder text.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyTAPlaceholder : public LVGLPropertyString
{
public:
	QString name() const { return "Placeholder"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_ta_set_placeholder_text(%1, \"%2\");").arg(obj->codeName()).arg(get(obj));
	}

protected:
	QString get(LVGLObject *obj) const { return lv_ta_get_placeholder_text(obj->obj()); }
	void set(LVGLObject *obj, QString string) { lv_ta_set_placeholder_text(obj->obj(), qPrintable(string)); }
};

#endif // LVGLPROPERTYTAPLACEHOLDER_H
