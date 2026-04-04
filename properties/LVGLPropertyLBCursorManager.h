#ifndef LVGLPROPERTYLBCURSORMANAGER_H
#define LVGLPROPERTYLBCURSORMANAGER_H

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyLBCursorManager : public LVGLPropertyBool
{
public:
	QString name() const { return "Cursor Manager"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_kb_set_cursor_manage(%1, %2);").arg(obj->codeName()).arg(QVariant(get(obj)).toString());
	}

protected:
	bool get(LVGLObject *obj) const { return lv_kb_get_cursor_manage(obj->obj()); }
	void set(LVGLObject *obj, bool statue) { lv_kb_set_cursor_manage(obj->obj(), statue); }
};

#endif // LVGLPROPERTYLBCURSORMANAGER_H
