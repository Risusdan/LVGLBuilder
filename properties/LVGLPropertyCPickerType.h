#ifndef LVGLPROPERTYCPICKERTYPE_H
#define LVGLPROPERTYCPICKERTYPE_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyCPickerType : public LVGLPropertyEnum
{
public:
	LVGLPropertyCPickerType()
		: LVGLPropertyEnum(QStringList() << "Rectange" << "Disc")
	{}

	QString name() const { return "Type"; }

protected:
	int get(LVGLObject *obj) const {
		lv_cpicker_ext_t *ext = reinterpret_cast<lv_cpicker_ext_t *>(lv_obj_get_ext_attr(obj->obj()));
		return ext->type;
	}
	void set(LVGLObject *obj, int index) { lv_cpicker_set_type(obj->obj(), index & 0xff); }
};

#endif // LVGLPROPERTYCPICKERTYPE_H
