#ifndef LVGLPROPERTYMBOXBUTTONS_H
#define LVGLPROPERTYMBOXBUTTONS_H

/**
 * @file LVGLPropertyMBoxButtons.h
 * @brief LVGLProperty subclass for lv_mbox action button labels.
 */

#include "LVGLPropertyTextList.h"
#include "LVGLObject.h"

class LVGLPropertyMBoxButtons : public LVGLPropertyTextList
{
public:
	inline QString name() const override { return "Buttons"; }
	inline ~LVGLPropertyMBoxButtons()
	{
		for (auto &entry : m_garbageCollector) {
			for (int i = 0; i <= entry.second; ++i)
				delete[] entry.first[i];
			delete[] entry.first;
		}
	}

protected:
	inline QStringList get(LVGLObject *obj) const override {
		QStringList ret;
		lv_obj_t *btnm = reinterpret_cast<lv_mbox_ext_t*>(lv_obj_get_ext_attr(obj->obj()))->btnm;
		if (btnm == nullptr)
			return {};

		lv_btnm_ext_t *ext = reinterpret_cast<lv_btnm_ext_t*>(lv_obj_get_ext_attr(btnm));
		for (uint16_t i = 0; i < ext->btn_cnt; ++i)
			ret << QString(ext->map_p[i]);
		return ret;
	}

	inline void set(LVGLObject *obj, QStringList list) override {
		char **map = new char*[list.size() + 1];
		for (int i = 0; i < list.size(); ++i) {
			map[i] = new char[list[i].size() + 1];
			memcpy(map[i], qPrintable(list[i]), list[i].size());
			map[i][list[i].size()] = '\0';
		}
		map[list.size()] = new char[1];
		map[list.size()][0] = '\0';
		m_garbageCollector << qMakePair(map, list.size());
		lv_mbox_add_btns(obj->obj(), const_cast<const char**>(map));
	}

private:
	QList<QPair<char**, int>> m_garbageCollector;
};

#endif // LVGLPROPERTYMBOXBUTTONS_H
