#ifndef LVGLPROPERTYLOCKED_H
#define LVGLPROPERTYLOCKED_H

/**
 * @file LVGLPropertyLocked.h
 * @brief LVGLProperty subclass controlling per-object lock flag (prevents drag/resize).
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyLocked : public LVGLPropertyBool
{
public:
	inline LVGLPropertyLocked() : LVGLPropertyBool("Locked") {}

protected:
	inline bool get(LVGLObject *obj) const override { return obj->isLocked(); }
	inline void set(LVGLObject *obj, bool statue) override { obj->setLocked(statue); }
};

#endif // LVGLPROPERTYLOCKED_H
