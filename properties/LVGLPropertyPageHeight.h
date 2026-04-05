#ifndef LVGLPROPERTYPAGEHEIGHT_H
#define LVGLPROPERTYPAGEHEIGHT_H

/**
 * @file LVGLPropertyPageHeight.h
 * @brief LVGLProperty subclass for lv_page scrollable-area height.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyPageHeight : public LVGLPropertyCoord
{
public:
	inline LVGLPropertyPageHeight(LVGLProperty *p = nullptr) : LVGLPropertyCoord(Qt::Vertical, p) {}
	inline QString name() const override { return "Scroll height"; }

	inline lv_coord_t get(LVGLObject *obj) const override { return lv_page_get_scrl_height(obj->obj()); }
	inline void set(LVGLObject *obj, lv_coord_t value) override { lv_page_set_scrl_height(obj->obj(), value); }

};

#endif // LVGLPROPERTYPAGEHEIGHT_H
