#include "LVGLPropertyRange.h"

#include <QJsonObject>
#include <algorithm>

class LVGLPropertyMin : public LVGLPropertyInt
{
public:
	inline LVGLPropertyMin(LVGLProperty *p) : LVGLPropertyInt(INT16_MIN, INT16_MAX, p) {}
	inline QString name() const override { return "Min"; }

	inline int get(LVGLObject *obj) const override {
		const LVGLPropertyRange *r = reinterpret_cast<const LVGLPropertyRange *>(m_parent);
		return r->getMin(obj);
	}
	inline void set(LVGLObject *obj, int value) override {
		LVGLPropertyRange *r = reinterpret_cast<LVGLPropertyRange *>(const_cast<LVGLProperty*>(m_parent));
		int max = r->getMax(obj);
		if (value > max) value = max;
		r->set(obj, value, max);
	}

};

class LVGLPropertyMax : public LVGLPropertyInt
{
public:
	inline LVGLPropertyMax(LVGLProperty *p) : LVGLPropertyInt(INT16_MIN, INT16_MAX, p) {}
	inline QString name() const override { return "Max"; }

	inline int get(LVGLObject *obj) const override {
		const LVGLPropertyRange *r = reinterpret_cast<const LVGLPropertyRange *>(m_parent);
		return r->getMax(obj);
	}
	inline void set(LVGLObject *obj, int value) override {
		LVGLPropertyRange *r = reinterpret_cast<LVGLPropertyRange *>(const_cast<LVGLProperty*>(m_parent));
		int min = r->getMin(obj);
		if (value < min) value = min;
		r->set(obj, min, value);
	}

};

LVGLPropertyRange::LVGLPropertyRange(LVGLProperty *parent)
	: LVGLProperty(parent)
	, m_min(new LVGLPropertyMin(this))
	, m_max(new LVGLPropertyMax(this))
{
	m_childs << m_min << m_max;
}

QString LVGLPropertyRange::name() const
{
	return "Range";
}

QVariant LVGLPropertyRange::value(LVGLObject *obj) const
{
	return QString("[%1,%2]").arg(getMin(obj)).arg(getMax(obj));
}

void LVGLPropertyRange::setValue(LVGLObject *obj, QVariant value)
{
	if (value.typeId() == QMetaType::QVariantMap) {
		QVariantMap map = value.toMap();
		int min = map["min"].toInt();
		int max = map["max"].toInt();
		if (min > max) std::swap(min, max);
		set(obj, min, max);
	}
}

QJsonValue LVGLPropertyRange::toJson(LVGLObject *obj) const
{
	return QJsonObject({{"min", getMin(obj)},
							  {"max", getMax(obj)}});
}
