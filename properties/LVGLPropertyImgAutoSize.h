#ifndef LVGLPROPERTYIMGAUTOSIZE_H
#define LVGLPROPERTYIMGAUTOSIZE_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyImgAutoSize : public LVGLPropertyBool
{
public:
	inline QString name() const { return "Auto Size"; }

	QStringList function(LVGLObject *obj) const {
		if (get(obj)) return QStringList();
		return QStringList() << QString("lv_img_set_auto_size(%1, %2);").arg(obj->codeName()).arg(QVariant(get(obj)).toString());
	}

protected:
	inline bool get(LVGLObject *obj) const { return lv_img_get_auto_size(obj->obj()); }
	inline void set(LVGLObject *obj, bool statue) { lv_img_set_auto_size(obj->obj(), statue); }
};

#endif // LVGLPROPERTYIMGAUTOSIZE_H
