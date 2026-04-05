#ifndef LVGLPROPERTYLABELBODYDRAW_H
#define LVGLPROPERTYLABELBODYDRAW_H

/**
 * @file LVGLPropertyLabelBodyDraw.h
 * @brief LVGLProperty subclass for lv_label body-draw (background fill) flag.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyLabelBodyDraw : public LVGLPropertyBool
{
public:
	QString name() const { return "Body draw"; }

	QStringList function(LVGLObject *obj) const {
		if (!get(obj)) return QStringList();
		return QStringList() << QString("lv_label_set_body_draw(%1, %2);").arg(obj->codeName()).arg(QVariant(get(obj)).toString());
	}

protected:
	bool get(LVGLObject *obj) const { return lv_label_get_body_draw(obj->obj()); }
	void set(LVGLObject *obj, bool boolean) { lv_label_set_body_draw(obj->obj(), boolean); }
};

#endif // LVGLPROPERTYLABELBODYDRAW_H
