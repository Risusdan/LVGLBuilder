#include "LVGLTabview.h"

#include <QIcon>
#include <QMap>

#include "LVGLObject.h"
#include "properties/LVGLPropertyTextList.h"
#include "LVGLCore.h"

class LVGLPropertyTabBtnPos : public LVGLPropertyEnum
{
public:
	LVGLPropertyTabBtnPos()
		: LVGLPropertyEnum({"Top", "Bottom", "Left", "Right"})
		, m_values({"LV_TABVIEW_BTNS_POS_TOP", "LV_TABVIEW_BTNS_POS_BOTTOM", "LV_TABVIEW_BTNS_POS_LEFT", "LV_TABVIEW_BTNS_POS_RIGHT"})
	{}

	QString name() const { return "Button position"; }

	QStringList function(LVGLObject *obj) const {
		if (get(obj) != LV_TABVIEW_BTNS_POS_TOP)
			return QStringList() << QString("lv_tabview_set_btns_pos(%1, %2);").arg(obj->codeName()).arg(m_values.at(get(obj)));
		return QStringList();
	}

protected:
	int get(LVGLObject *obj) const { return lv_tabview_get_btns_pos(obj->obj()); }
	void set(LVGLObject *obj, int index) { lv_tabview_set_btns_pos(obj->obj(), index & 0xff); }

	QStringList m_values;
};

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
		// tab k's allocated name lives at array index k for TOP/BOTTOM,
		// and k*2 for LEFT/RIGHT (separated by "\n" literals).
		const bool sideLayout =
			(ext->btns_pos == LV_TABVIEW_BTNS_POS_LEFT ||
			 ext->btns_pos == LV_TABVIEW_BTNS_POS_RIGHT);
		const uint16_t stride = sideLayout ? 2 : 1;
		for (uint16_t i = 0; i < ext->tab_cnt; ++i)
			ret << QString(ext->tab_name_ptr[i * stride]);
		return ret;
	}

	inline void set(LVGLObject *obj, QStringList list) {
		lv_tabview_ext_t * ext = reinterpret_cast<lv_tabview_ext_t*>(lv_obj_get_ext_attr(obj->obj()));

		// ================================================================
		// PHASE 1: Name-based removal — detect which tabs were deleted
		// ================================================================
		//
		// Strategy: use a name→count map to match old tabs against new tabs.
		//
		// Example: old=[A, B, C], new=[A, C]
		//   countMap from new: {A:1, C:1}
		//   Walk old: A → found (A:0), B → not found → REMOVE, C → found (C:0)
		//   toRemove = [1]  (index of "B")
		//
		// Why a count map instead of a simple set?
		//   Handles duplicate tab names. If old=[A, A, B] and new=[A, B],
		//   the first A survives (count 1→0), the second A is removed (count=0).

		// Build name→count map from the new list
		QMap<QString, int> countMap;
		for (const QString &name : list)
			countMap[name]++;

		// Walk old tabs, consume counts to decide survive vs. remove
		QList<int> toRemove;
		for (uint16_t i = 0; i < ext->tab_cnt; ++i) {
			QString oldName(ext->tab_name_ptr[i]);
			if (countMap.value(oldName, 0) > 0) {
				countMap[oldName]--;   // this old tab survives
			} else {
				toRemove.append(i);   // this old tab was deleted by user
			}
		}

		// Process removals from HIGHEST index to LOWEST.
		// Why reverse order? When we remove index 2, indices 0 and 1 are
		// unaffected. If we removed index 0 first, all higher indices would
		// shift down and our stored indices would be wrong.
		for (int r = toRemove.size() - 1; r >= 0; --r) {
			int i = toRemove[r];

			// --- Clean up the builder-side wrapper tree ---
			// There are TWO parallel object trees:
			//   LVGL tree:    lv_obj_t (tabview) → lv_obj_t (page) → lv_obj_t (children)
			//   Builder tree: LVGLObject (tabview) → LVGLObject (page) → LVGLObject (children)
			//
			// lv_tabview_remove_tab() deletes the LVGL page + all LVGL children.
			// We must detach all wrappers first so their destructors don't
			// also call lv_obj_del() on already-freed objects (double-free crash).
			LVGLObject *page = obj->findChildByIndex(i);
			if (page) {
				page->detachLvObjRecursive();  // null out m_obj on page + descendants
				lvgl.removeObject(page);       // remove from builder tree + delete wrappers
			}

			// Now let LVGL do the actual object deletion + tabview bookkeeping
			lv_tabview_remove_tab(obj->obj(), static_cast<uint16_t>(i));
		}

		// After removals, re-index surviving page wrappers.
		// Example: removing index 1 from [A(0), B(1), C(2)] leaves
		// C with m_index=2, but it's now at LVGL position 1.
		// Without this fix, findChildByIndex() fails for subsequent
		// operations (widget drop, save/load, further tab edits).
		{
			int idx = 0;
			for (LVGLObject *child : obj->childs()) {
				child->setIndex(idx++);
			}
		}

		// ================================================================
		// PHASE 2: Positional rename — handle in-place text edits
		// ================================================================
		//
		// After removals, the surviving LVGL tabs are at indices 0..tab_cnt-1.
		// Compare each with list[i]. If they differ, the user edited the
		// text in the dialog → rename in place.
		//
		// Example: old=[A, B] → no removals → LVGL has [A, B]
		//          new=[A, X] → A=A ok, B≠X → rename B to X.

		// Re-read ext since tab_cnt may have changed during removals
		ext = reinterpret_cast<lv_tabview_ext_t*>(lv_obj_get_ext_attr(obj->obj()));
		// tab k's name lives at array index k for TOP/BOTTOM, k*2 for LEFT/RIGHT
		const bool sideLayout =
			(ext->btns_pos == LV_TABVIEW_BTNS_POS_LEFT ||
			 ext->btns_pos == LV_TABVIEW_BTNS_POS_RIGHT);
		const uint16_t stride = sideLayout ? 2 : 1;
		for (uint16_t i = 0; i < qMin(ext->tab_cnt, static_cast<uint16_t>(list.size())); ++i) {
			const uint16_t arrIdx = i * stride;
			QByteArray name = list.at(i).toLatin1();
			if (strcmp(ext->tab_name_ptr[arrIdx], name.data()) == 0)
				continue;  // name unchanged, skip

			// Allocate new LVGL name string
			char * name_dm = reinterpret_cast<char*>(lv_mem_alloc(static_cast<uint32_t>(name.size() + 1)));
			if (name_dm == nullptr)
				continue;
			memcpy(name_dm, name.constData(), static_cast<size_t>(name.size() + 1));

			// FIX (C4): free the old name BEFORE overwriting its slot.
			// Old name was allocated by lv_tabview_add_tab() or a prior
			// rename — both use lv_mem_alloc. Without this free, every
			// rename leaks bytes in LVGL's heap.
			lv_mem_free((void *)ext->tab_name_ptr[arrIdx]);
			ext->tab_name_ptr[arrIdx] = name_dm;

			lv_btnm_set_map(ext->btns, ext->tab_name_ptr);
			lv_btnm_set_btn_ctrl(ext->btns, ext->tab_cur, LV_BTNM_CTRL_NO_REPEAT);
		}

		// ================================================================
		// PHASE 3: Append new tabs — handle additions
		// ================================================================
		//
		// Any list entries beyond the current tab_cnt are brand-new tabs.
		// LVGL only supports appending (no insert-at-position).
		//
		// Example: old=[A, C] (after removal), new=[A, C, D]
		//          → tab_cnt=2, list.size()=3 → add "D" at the end.

		for (int i = ext->tab_cnt; i < list.size(); ++i) {
			lv_obj_t *page_obj = lv_tabview_add_tab(obj->obj(), qPrintable(list.at(i)));
			LVGLObject *page = new LVGLObject(page_obj, lvgl.widget("lv_page"), obj, false, i);
			lvgl.addObject(page);
		}
	}

};

class LVGLPropertyTabCurrent : public LVGLPropertyInt
{
public:
	LVGLPropertyTabCurrent() : LVGLPropertyInt(0, UINT16_MAX) {}

	QString name() const { return "Current tab"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_tabview_set_tab_act(%1, %2, LV_ANIM_OFF);").arg(obj->codeName()).arg(get(obj));
	}

protected:
	int get(LVGLObject *obj) const { return lv_tabview_get_tab_act(obj->obj()); }
	void set(LVGLObject *obj, int value) { lv_tabview_set_tab_act(obj->obj(), static_cast<uint16_t>(value), LV_ANIM_OFF); }
};

class LVGLPropertyTabBtnHide : public LVGLPropertyBool
{
public:
	QString name() const { return "Hide buttons"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_tabview_set_btns_hidden(%1, %2);").arg(obj->codeName()).arg(QVariant(get(obj)).toString());
	}

protected:
	bool get(LVGLObject *obj) const { return lv_tabview_get_btns_hidden(obj->obj()); }
	void set(LVGLObject *obj, bool boolean) { lv_tabview_set_btns_hidden(obj->obj(), boolean); }
};

class LVGLPropertyTabAnimTime : public LVGLPropertyInt
{
public:
	LVGLPropertyTabAnimTime() : LVGLPropertyInt(0, UINT16_MAX) {}

	QString name() const { return "Animation time"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_preload_set_spin_time(%1, %2);").arg(obj->codeName()).arg(get(obj));
	}

protected:
	int get(LVGLObject *obj) const { return lv_preload_get_spin_time(obj->obj()); }
	void set(LVGLObject *obj, int value) { lv_preload_set_spin_time(obj->obj(), static_cast<uint16_t>(value)); }
};

class LVGLPropertyTabSliding : public LVGLPropertyBool
{
public:
	QString name() const { return "Sliding"; }

	QStringList function(LVGLObject *obj) const {
		return QStringList() << QString("lv_tabview_set_sliding(%1, %2);").arg(obj->codeName()).arg(QVariant(get(obj)).toString());
	}

protected:
	bool get(LVGLObject *obj) const { return lv_tabview_get_sliding(obj->obj()); }
	void set(LVGLObject *obj, bool boolean) { lv_tabview_set_sliding(obj->obj(), boolean); }
};

class LVGLPropertyTabScrollbars : public LVGLPropertyEnum
{
public:
	LVGLPropertyTabScrollbars()
		: LVGLPropertyEnum(QStringList() << "Off" << "On" << "Drag" << "Auto")
		, m_values({"LV_SB_MODE_OFF", "LV_SB_MODE_ON", "LV_SB_MODE_DRAG", "LV_SB_MODE_AUTO"})
	{}

	QString name() const { return "Scrollbars"; }

protected:
	int get(LVGLObject *obj) const {
		lv_sb_mode_t mode = LV_SB_MODE_AUTO;
		for (uint16_t i = 0; i < lv_tabview_get_tab_count(obj->obj()); ++i) {
			lv_obj_t *page = lv_tabview_get_tab(obj->obj(), i);
			mode = lv_page_get_sb_mode(page);
		}
		return mode;
	}
	void set(LVGLObject *obj, int index) {
		for (uint16_t i = 0; i < lv_tabview_get_tab_count(obj->obj()); ++i) {
			lv_obj_t *page = lv_tabview_get_tab(obj->obj(), i);
			lv_page_set_sb_mode(page, index & 0xff);
		}
	}

	QStringList m_values;
};

LVGLTabview::LVGLTabview()
{
	m_properties << new LVGLPropertyTabs;
	m_properties << new LVGLPropertyTabBtnPos;
	m_properties << new LVGLPropertyTabCurrent;
	m_properties << new LVGLPropertyTabBtnHide;
	m_properties << new LVGLPropertyTabSliding;
	//m_properties << new LVGLPropertyTabScrollbars;

	m_editableStyles << LVGL::Body; // LV_TABVIEW_STYLE_BG
	m_editableStyles << LVGL::Body; // LV_TABVIEW_STYLE_INDIC
	m_editableStyles << LVGL::Body; // LV_TABVIEW_STYLE_BTN_BG
	m_editableStyles << LVGL::Body; // LV_TABVIEW_STYLE_BTN_REL
	m_editableStyles << LVGL::Body; // LV_TABVIEW_STYLE_BTN_PR
	m_editableStyles << LVGL::Body; // LV_TABVIEW_STYLE_BTN_TGL_REL
	m_editableStyles << LVGL::Body; // LV_TABVIEW_STYLE_BTN_TGL_PR
}

QString LVGLTabview::name() const
{
	return "Tabview";
}

QString LVGLTabview::className() const
{
	return "lv_tabview";
}

LVGLWidget::Type LVGLTabview::type() const
{
	return TabView;
}

QIcon LVGLTabview::icon() const
{
	return QIcon();
}

lv_obj_t *LVGLTabview::newObject(lv_obj_t *parent) const
{
	lv_obj_t *obj = lv_tabview_create(parent, nullptr);
	return obj;
}

QSize LVGLTabview::minimumSize() const
{
	return QSize(100, 150);
}

QStringList LVGLTabview::styles() const
{
	return QStringList() << "LV_TABVIEW_STYLE_BG"
								<< "LV_TABVIEW_STYLE_INDIC"
								<< "LV_TABVIEW_STYLE_BTN_BG"
								<< "LV_TABVIEW_STYLE_BTN_REL"
								<< "LV_TABVIEW_STYLE_BTN_PR"
								<< "LV_TABVIEW_STYLE_BTN_TGL_REL"
								<< "LV_TABVIEW_STYLE_BTN_TGL_PR";
}

lv_style_t *LVGLTabview::style(lv_obj_t *obj, int type) const
{
	return const_cast<lv_style_t*>(lv_tabview_get_style(obj, type & 0xff));
}

void LVGLTabview::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const
{
	lv_tabview_set_style(obj, static_cast<lv_tabview_style_t>(type), style);
}

lv_style_t *LVGLTabview::defaultStyle(int type) const
{
	if (type == LV_TABVIEW_STYLE_BG)
		return &lv_style_plain;
	else if (type == LV_TABVIEW_STYLE_INDIC)
		return &lv_style_plain_color;
	else if (type == LV_TABVIEW_STYLE_BTN_BG)
		return &lv_style_transp;
	else if (type == LV_TABVIEW_STYLE_BTN_REL)
		return &lv_style_btn_rel;
	else if (type == LV_TABVIEW_STYLE_BTN_PR)
		return &lv_style_btn_pr;
	else if (type == LV_TABVIEW_STYLE_BTN_TGL_REL)
		return &lv_style_btn_tgl_rel;
	else if (type == LV_TABVIEW_STYLE_BTN_TGL_PR)
		return &lv_style_btn_tgl_pr;
	return nullptr;
}
