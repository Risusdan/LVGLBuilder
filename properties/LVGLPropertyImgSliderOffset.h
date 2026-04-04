#ifndef LVGLPROPERTYIMGSLIDEROFFSET_H
#define LVGLPROPERTYIMGSLIDEROFFSET_H

#include <QJsonObject>
#include "LVGLProperty.h"
#include "LVGLObject.h"
#include "LVGLPropertyImgSliderOffsetX.h"
#include "LVGLPropertyImgSliderOffsetY.h"

class LVGLPropertyImgSliderOffset : public LVGLProperty
{
public:
	inline LVGLPropertyImgSliderOffset(LVGLProperty *parent = nullptr)
		: LVGLProperty(parent)
		, m_x(new LVGLPropertyImgSliderOffsetX(this))
		, m_y(new LVGLPropertyImgSliderOffsetY(this))
	{
		m_childs << m_x << m_y;
	}

	inline QString name() const override { return "Offset"; }
	inline QVariant value(LVGLObject *obj) const override {
		return QString("(%1,%2)").arg(child(0)->value(obj).toInt())
										 .arg(child(1)->value(obj).toInt());
	}

	inline void setValue(LVGLObject *obj, QVariant value) override {
		if (value.typeId() == QMetaType::QVariantMap) {
			QVariantMap map = value.toMap();
			m_x->set(obj, static_cast<lv_coord_t>(map["X"].toInt()));
			m_y->set(obj, static_cast<lv_coord_t>(map["Y"].toInt()));
		}
	}

	inline QJsonValue toJson(LVGLObject *obj) const override {
		QJsonObject object({{"X", m_x->get(obj)},
								  {"Y", m_y->get(obj)}
								 });
		return object;
	}

	QStringList function(LVGLObject *obj) const override {
		QStringList ret;
		if (m_x->get(obj) != 0)
			ret << QString("lv_imgslider_set_offset_x(%1, %2);").arg(obj->codeName()).arg(m_x->get(obj));
		if (m_y->get(obj) != 0)
			ret << QString("lv_imgslider_set_offset_y(%1, %2);").arg(obj->codeName()).arg(m_y->get(obj));
		return ret;
	}

private:
	LVGLPropertyImgSliderOffsetX *m_x;
	LVGLPropertyImgSliderOffsetY *m_y;

};

#endif // LVGLPROPERTYIMGSLIDEROFFSET_H
