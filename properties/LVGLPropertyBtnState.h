#ifndef LVGLPROPERTYBTNSTATE_H
#define LVGLPROPERTYBTNSTATE_H

/**
 * @file LVGLPropertyBtnState.h
 * @brief LVGLProperty subclass for generic button state (released, pressed, etc.).
 */

#include "LVGLProperty.h"

class QComboBox;

class LVGLPropertyBtnState : public LVGLProperty
{
public:
	LVGLPropertyBtnState();

	QString name() const override;
	QVariant value(lv_obj_t *obj) const override;

	bool hasEditor() const override;
	QWidget *editor(QWidget *parent) override;
	void updateEditor(lv_obj_t *obj) override;
	void updateWidget(lv_obj_t *obj) override;

protected:
	QComboBox *m_widget;
	QStringList m_states;

};

#endif // LVGLPROPERTYBTNSTATE_H
