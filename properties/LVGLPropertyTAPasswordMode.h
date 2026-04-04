#ifndef LVGLPROPERTYTAPASSWORDMODE_H
#define LVGLPROPERTYTAPASSWORDMODE_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyTAPasswordMode : public LVGLPropertyBool
{
public:
	QString name() const { return "Password mode"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_ta_set_pwd_mode(%1, %2);").arg(obj->codeName()).arg(QVariant(get(obj)).toString());
	}

protected:
	bool get(LVGLObject *obj) const { return lv_ta_get_pwd_mode(obj->obj()); }
	void set(LVGLObject *obj, bool statue) { lv_ta_set_pwd_mode(obj->obj(), statue); }
};

#endif // LVGLPROPERTYTAPASSWORDMODE_H
