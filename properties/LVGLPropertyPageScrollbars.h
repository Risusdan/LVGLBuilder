#ifndef LVGLPROPERTYPAGESCROLLBARS_H
#define LVGLPROPERTYPAGESCROLLBARS_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyPageScrollbars : public LVGLPropertyEnum
{
public:
	LVGLPropertyPageScrollbars()
		: LVGLPropertyEnum(QStringList() << "Off" << "On" << "Drag" << "Auto")
		, m_values({"LV_SB_MODE_OFF", "LV_SB_MODE_ON", "LV_SB_MODE_DRAG", "LV_SB_MODE_AUTO"})
	{}

	QString name() const { return "Scrollbars"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_page_set_sb_mode(%1, %2);").arg(obj->codeName()).arg(m_values.at(get(obj)));
	}

protected:
	int get(LVGLObject *obj) const { return lv_page_get_sb_mode(obj->obj()) & 0x03; }
	void set(LVGLObject *obj, int index) { lv_page_set_sb_mode(obj->obj(), index & 0xff); }

	QStringList m_values;
};

#endif // LVGLPROPERTYPAGESCROLLBARS_H
