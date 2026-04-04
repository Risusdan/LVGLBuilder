#ifndef LVGLPROPERTYLABELALIGN_H
#define LVGLPROPERTYLABELALIGN_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyLabelAlign : public LVGLPropertyEnum
{
public:
	LVGLPropertyLabelAlign()
		: LVGLPropertyEnum(QStringList() << "Left" << "Center" << "Right")
		, m_values({"LV_LABEL_ALIGN_LEFT", "LV_LABEL_ALIGN_CENTER", "LV_LABEL_ALIGN_RIGHT"})
	{}

	QString name() const { return "Align"; }

	QStringList function(LVGLObject *obj) const {
		if (get(obj) == LV_LABEL_ALIGN_LEFT) return QStringList();
		return QStringList() << QString("lv_label_set_align(%1, %2);").arg(obj->codeName()).arg(m_values.at(get(obj)));
	}

protected:
	int get(LVGLObject *obj) const { return lv_label_get_align(obj->obj()); }
	void set(LVGLObject *obj, int index) { lv_label_set_align(obj->obj(), index & 0xff); }

	QStringList m_values;
};

#endif // LVGLPROPERTYLABELALIGN_H
