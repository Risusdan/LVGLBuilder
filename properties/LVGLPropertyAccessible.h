#ifndef LVGLPROPERTYACCESSIBLE_H
#define LVGLPROPERTYACCESSIBLE_H

/**
 * @file LVGLPropertyAccessible.h
 * @brief LVGLProperty subclass controlling per-object accessibility flag.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyAccessible : public LVGLPropertyBool
{
public:
	inline LVGLPropertyAccessible() : LVGLPropertyBool("Accessible") {}

protected:
	inline bool get(LVGLObject *obj) const override { return obj->isAccessible(); }
	inline void set(LVGLObject *obj, bool statue) override { obj->setAccessible(statue); }
};

#endif // LVGLPROPERTYACCESSIBLE_H
