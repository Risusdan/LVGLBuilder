#ifndef LVGLPROPERTYMONTHNAMES_H
#define LVGLPROPERTYMONTHNAMES_H

#include "LVGLPropertyTextList.h"
#include "LVGLObject.h"
#include "LVGLCore.h"

class LVGLPropertyMonthNames : public LVGLPropertyTextList
{
public:
	inline LVGLPropertyMonthNames() : LVGLPropertyTextList(false) {}
	inline ~LVGLPropertyMonthNames() {
		for (const char **d:m_garbageCollector) {
			for (uint8_t i = 0; i < N; ++i)
				delete [] d[i];
			delete [] d;
		}
	}

	inline QString name() const override { return "Month names"; }

	inline QStringList function(LVGLObject *obj) const override
	{
		const QStringList list = get(obj);
		if (!isDifferent(list))
			return {};
		QStringList months;
		for (uint8_t i = 0; i < N; ++i)
			months << "\"" + list[i] + "\"";
		QStringList ret;
		const QString varName = QString("months_%1").arg(obj->codeName());
		ret << QString("static const char *%1[12] = {%2};").arg(varName).arg(months.join(", "));
		ret << QString("lv_calendar_set_month_names(%1, %2);").arg(obj->codeName()).arg(varName);
		return ret;
	}

protected:
	QList<const char **> m_garbageCollector;
	static constexpr uint8_t N = 12;
	inline bool isDifferent(QStringList list) const {
		for (uint8_t i = 0; i < N; ++i) {
			if (list[i] != LVGLCore::DEFAULT_MONTHS[i])
				return true;
		}
		return false;
	}

	inline QStringList get(LVGLObject *obj) const override {
		const char **names = lv_calendar_get_month_names(obj->obj());
		if (names == nullptr)
			names = LVGLCore::DEFAULT_MONTHS;
		QStringList ret;
		for (uint8_t i = 0; i < N; ++i)
			ret << QString(names[i]);
		return ret;
	}

	inline void set(LVGLObject *obj, QStringList list) override {
		if (!isDifferent(list))
			return;
		const char **data = new const char *[N];
		for (uint8_t i = 0; i < N; ++i) {
			const QString &s = list[i];
			char *string = new char[s.size()+1];
			memcpy(string, qPrintable(s), s.size());
			string[s.size()] = '\0';
			data[i] = string;
		}
		m_garbageCollector << data;
		lv_calendar_set_month_names(obj->obj(), data);
	}

};

#endif // LVGLPROPERTYMONTHNAMES_H
