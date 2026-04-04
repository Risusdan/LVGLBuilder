#ifndef LVGLPROPERTYTABS_H
#define LVGLPROPERTYTABS_H

#include "LVGLPropertyTextList.h"
#include "LVGLObject.h"
#include "LVGLCore.h"

class LVGLPropertyTabs : public LVGLPropertyTextList
{
public:
	inline LVGLPropertyTabs(LVGLProperty *parent = nullptr)
		: LVGLPropertyTextList(true, parent) {}

	inline QString name() const { return "Tabs"; }

protected:
	inline QStringList get(LVGLObject *obj) const {
		QStringList ret;
		lv_tabview_ext_t * ext = reinterpret_cast<lv_tabview_ext_t*>(lv_obj_get_ext_attr(obj->obj()));
		for (uint16_t i = 0; i < ext->tab_cnt; ++i)
			ret << QString(ext->tab_name_ptr[i]);
		return ret;
	}

	inline void set(LVGLObject *obj, QStringList list) {
		lv_tabview_ext_t * ext = reinterpret_cast<lv_tabview_ext_t*>(lv_obj_get_ext_attr(obj->obj()));
		// rename
		for (uint16_t i = 0; i < qMin(ext->tab_cnt, static_cast<uint16_t>(list.size())); ++i) {
			QByteArray name = list.at(i).toLatin1();
			if (strcmp(ext->tab_name_ptr[i], name.data()) == 0)
				continue;

			char * name_dm = reinterpret_cast<char*>(lv_mem_alloc(name.size()));
			if (name_dm == nullptr)
				continue;

			memcpy(name_dm, name, name.size());
			name_dm[name.size()] = '\0';
			ext->tab_name_ptr[i] = name_dm;

			lv_btnm_set_map(ext->btns, ext->tab_name_ptr);
			lv_btnm_set_btn_ctrl(ext->btns, ext->tab_cur, LV_BTNM_CTRL_NO_REPEAT);
		}

		// add new
		for (uint16_t i = ext->tab_cnt; i < list.size(); ++i) {
			//lv_tabview_add_tab(obj->obj(), qPrintable(list.at(i)));
			lv_obj_t *page_obj = lv_tabview_add_tab(obj->obj(), qPrintable(list.at(i)));
			LVGLObject *page = new LVGLObject(page_obj, lvgl.widget("lv_page"), obj, false, i);
			lvgl.addObject(page);
		}
	}

};

#endif // LVGLPROPERTYTABS_H
