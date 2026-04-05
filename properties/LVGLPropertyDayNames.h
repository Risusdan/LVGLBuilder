#ifndef LVGLPROPERTYDAYNAMES_H
#define LVGLPROPERTYDAYNAMES_H

/**
 * @file LVGLPropertyDayNames.h
 * @brief LVGLProperty subclass for lv_calendar custom day-name strings.
 */

#include "LVGLPropertyTextList.h"
#include "LVGLObject.h"
#include "LVGLCore.h"

class LVGLPropertyDayNames : public LVGLPropertyTextList
{
public:
	inline LVGLPropertyDayNames() : LVGLPropertyTextList(false) {}
	inline ~LVGLPropertyDayNames() {
		for (const char **d:m_garbageCollector) {
			for (uint8_t i = 0; i < N; ++i)
				delete [] d[i];
			delete [] d;
		}
	}

	inline QString name() const override { return "Day names"; }

	inline QStringList function(LVGLObject *obj) const override
	{
		const QStringList list = get(obj);
		if (!isDifferent(list))
			return {};
		QStringList days;
		for (uint8_t i = 0; i < N; ++i)
			days << "\"" + list[i] + "\"";
		QStringList ret;
		const QString varName = QString("days_%1").arg(obj->codeName());
		ret << QString("static const char *%1[7] = {%2};").arg(varName).arg(days.join(", "));
		ret << QString("lv_calendar_set_day_names(%1, %2);").arg(obj->codeName()).arg(varName);
		return ret;
	}

protected:
	QList<const char **> m_garbageCollector;
	static constexpr uint8_t N = 7;

	inline bool isDifferent(QStringList list) const {
		for (uint8_t i = 0; i < N; ++i) {
			if (list[i] != LVGLCore::DEFAULT_DAYS[i])
				return true;
		}
		return false;
	}

	inline QStringList get(LVGLObject *obj) const override {
		const char **names = lv_calendar_get_day_names(obj->obj());
		if (names == nullptr)
			return {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
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
		lv_calendar_set_day_names(obj->obj(), data);
	}

};

#endif // LVGLPROPERTYDAYNAMES_H
