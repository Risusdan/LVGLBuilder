#ifndef LVGLPROPERTYTATEXTALIGN_H
#define LVGLPROPERTYTATEXTALIGN_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyTATextAlign : public LVGLPropertyEnum
{
public:
	LVGLPropertyTATextAlign()
		: LVGLPropertyEnum(QStringList() << "Left" << "Center" << "Right") {}

	QString name() const { return "Text align"; }

protected:
	int get(LVGLObject *obj) const { return lv_label_get_align(lv_ta_get_label(obj->obj()));  }
	void set(LVGLObject *obj, int index) { lv_ta_set_text_align(obj->obj(), index & 0xff); }
};

#endif // LVGLPROPERTYTATEXTALIGN_H
