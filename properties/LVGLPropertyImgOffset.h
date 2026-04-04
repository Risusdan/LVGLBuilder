#ifndef LVGLPROPERTYIMGOFFSET_H
#define LVGLPROPERTYIMGOFFSET_H

#include <QJsonObject>
#include "LVGLProperty.h"
#include "LVGLObject.h"
#include "LVGLPropertyImgOffsetX.h"
#include "LVGLPropertyImgOffsetY.h"

class LVGLPropertyImgOffset : public LVGLProperty
{
public:
	inline LVGLPropertyImgOffset(LVGLProperty *parent = nullptr)
		: LVGLProperty(parent)
		, m_x(new LVGLPropertyImgOffsetX(this))
		, m_y(new LVGLPropertyImgOffsetY(this))
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
		if (m_x->get(obj) > 0)
			ret << QString("lv_img_set_offset_x(%1, %2);").arg(obj->codeName()).arg(m_x->get(obj));
		if (m_y->get(obj) > 0)
			ret << QString("lv_img_set_offset_y(%1, %2);").arg(obj->codeName()).arg(m_y->get(obj));
		return ret;
	}

private:
	LVGLPropertyImgOffsetX *m_x;
	LVGLPropertyImgOffsetY *m_y;

};

#endif // LVGLPROPERTYIMGOFFSET_H
