#ifndef LVGLPROPERTYNAME_H
#define LVGLPROPERTYNAME_H

/**
 * @file LVGLPropertyName.h
 * @brief LVGLProperty subclass for the widget's identifier name.
 */

#include "LVGLProperty.h"
#include "LVGLObject.h"

class LVGLPropertyName : public LVGLPropertyString
{
public:
	inline QString name() const override { return "Name"; }

protected:
	inline QString get(LVGLObject *obj) const override { return obj->name(); }
	inline void set(LVGLObject *obj, QString string) override { obj->setName(string); }
};

#endif // LVGLPROPERTYNAME_H
