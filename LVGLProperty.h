/**
 * @file LVGLProperty.h
 * @brief Abstract property system for widget configuration in the builder.
 *
 * Defines the LVGLProperty base class and a family of typed subclasses that
 * handle getting/setting widget properties, providing editor widgets for the
 * property inspector, JSON serialization, and C code generation.
 *
 * The property system uses a composite pattern: an LVGLProperty can have
 * child properties (e.g., a "Geometry" property contains X, Y, Width, Height
 * sub-properties). This maps directly to the tree structure shown in the
 * property inspector panel.
 *
 * Template subclasses (LVGLPropertyType<T>, LVGLPropertyValT<T>) provide
 * type-safe get/set so that property implementations work with concrete C++
 * types (bool, int, QString, lv_coord_t) rather than QVariant. The
 * value()/setValue() bridge in the template handles QVariant conversion.
 */

#ifndef LVGLPROPERTY_H
#define LVGLPROPERTY_H

#include <QVariant>
#include <QJsonValue>
#include <lvgl/lvgl.h>
#include <functional>

class LVGLObject;

class QComboBox;
class QLineEdit;
class QSpinBox;

/**
 * @class LVGLProperty
 * @brief Abstract base class for all editable widget properties.
 *
 * Provides the interface for:
 * - Reading/writing property values on an LVGLObject
 * - Creating editor widgets for the property inspector
 * - Serializing to/from JSON for project save/load
 * - Generating C code for export
 *
 * Properties form a tree structure (parent/children) that mirrors the
 * display in the property inspector. Leaf properties do actual get/set;
 * branch properties group related leaves (e.g., "Geometry" groups X/Y/W/H).
 */
class LVGLProperty {
public:
	LVGLProperty(LVGLProperty *parent = nullptr);
	virtual ~LVGLProperty();

	/** @brief Returns the property display name shown in the inspector. */
	virtual QString name() const = 0;

	/** @brief Returns true if this property provides an editor widget. */
	virtual bool hasEditor() const;
	/**
	 * @brief Creates the editor widget for the property inspector.
	 * @param parent The parent QWidget for the editor.
	 * @return The editor widget (combo box, spin box, line edit, etc.).
	 */
	virtual QWidget *editor(QWidget *parent);
	/** @brief Updates the editor widget to reflect the current object's value. */
	virtual void updateEditor(LVGLObject *obj);
	/** @brief Reads the editor widget and applies the value to the object. */
	virtual void updateWidget(LVGLObject *obj);

	/** @brief Returns the default value for this property. */
	virtual QVariant defaultValue() const;
	/** @brief Reads the current property value from an object. */
	virtual QVariant value(LVGLObject *obj) const;
	/** @brief Writes a property value to an object. */
	virtual void setValue(LVGLObject *obj, QVariant value);

	/** @brief Serializes the property value to JSON. */
	virtual QJsonValue toJson(LVGLObject *obj) const;

	/** @brief Parent property in the tree, or nullptr for top-level. */
	const LVGLProperty *parent() const;
	/** @brief Number of child properties. */
	int count() const;
	/** @brief Child property at the given index. */
	const LVGLProperty *child(int index) const;
	/** @brief Row index within the parent's child list. */
	int row() const;
	/** @brief Index of a child property. */
	int indexOf(const LVGLProperty *item) const;

	/**
	 * @brief Generates C code lines for setting this property.
	 * @param obj The widget instance.
	 * @return List of C function call strings (e.g., "lv_btn_set_toggle(btn1, true);").
	 *
	 * Base implementation returns an empty list. Subclasses return an empty
	 * list when no function name was provided at construction time.
	 */
	virtual QStringList function(LVGLObject *obj) const;

protected:
	LVGLProperty *m_parent;
	QList<const LVGLProperty*> m_childs;

};

/**
 * @class LVGLPropertyType
 * @brief Template base that provides type-safe get/set for properties with a known C++ type.
 * @tparam T The native value type (bool, int, lv_coord_t, QString, etc.).
 *
 * Subclasses implement get() and set() with the concrete type T instead of
 * QVariant. The value()/setValue() bridge handles QVariant conversion.
 */
template <class T>
class LVGLPropertyType : public LVGLProperty
{
public:
	inline LVGLPropertyType(LVGLProperty *parent = nullptr) : LVGLProperty(parent) {}

	inline QVariant value(LVGLObject *obj) const override { return get(obj); }
	inline void setValue(LVGLObject *obj, QVariant value) override { return set(obj, value.value<T>()); }
	inline bool hasEditor() const override { return true; }

protected:
	virtual T get(LVGLObject *obj) const = 0;
	virtual void set(LVGLObject *obj, T value) = 0;
};

/**
 * @class LVGLPropertyEnum
 * @brief Property backed by a list of string options, displayed as a combo box.
 *
 * Used for properties like alignment, layout mode, and other fixed-choice
 * settings. The enum text list maps 1:1 to integer values.
 */
class LVGLPropertyEnum : public LVGLProperty
{
public:
	LVGLPropertyEnum(QStringList enumText, LVGLProperty *parent = nullptr);

	QVariant value(LVGLObject *obj) const override;
	void setValue(LVGLObject *obj, QVariant value) override;

	bool hasEditor() const override;

	QWidget *editor(QWidget *parent) override;
	void updateEditor(LVGLObject *obj) override;
	void updateWidget(LVGLObject *obj) override;

protected:
	QStringList m_enum;
	QComboBox *m_widget;

	virtual int get(LVGLObject *obj) const = 0;
	virtual void set(LVGLObject *obj, int value) = 0;
};

/**
 * @class LVGLPropertyCoord
 * @brief Coordinate property with a spin box editor bounded by display dimensions.
 *
 * Can be oriented horizontally (bounded by display width) or vertically
 * (bounded by display height).
 */
class LVGLPropertyCoord : public LVGLPropertyType<lv_coord_t>
{
public:
	LVGLPropertyCoord(LVGLProperty *parent = nullptr);
	LVGLPropertyCoord(Qt::Orientation orientation, LVGLProperty *parent = nullptr);

	QWidget *editor(QWidget *parent) override;
	void updateEditor(LVGLObject *obj) override;
	void updateWidget(LVGLObject *obj) override;

protected:
	QSpinBox *m_widget;
	int m_max;

};

/**
 * @class LVGLPropertyInt
 * @brief Integer property with configurable min/max range and optional suffix.
 */
class LVGLPropertyInt : public LVGLPropertyType<int>
{
public:
	LVGLPropertyInt(int min, int max, LVGLProperty *parent = nullptr);
	LVGLPropertyInt(int min, int max, QString surfix, LVGLProperty *parent = nullptr);

	QWidget *editor(QWidget *parent) override;
	void updateEditor(LVGLObject *obj) override;
	void updateWidget(LVGLObject *obj) override;

protected:
	QSpinBox *m_widget;
	int m_min;
	int m_max;
	QString m_surfix;

};

/**
 * @class LVGLPropertyFont
 * @brief Font selection property displayed as a combo box of registered fonts.
 */
class LVGLPropertyFont : public LVGLProperty
{
public:
	LVGLPropertyFont(LVGLProperty *parent = nullptr);

	QVariant value(LVGLObject *obj) const override;
	void setValue(LVGLObject *obj, QVariant value) override;

	bool hasEditor() const override;

	QWidget *editor(QWidget *parent) override;
	void updateEditor(LVGLObject *obj) override;
	void updateWidget(LVGLObject *obj) override;

protected:
	QComboBox *m_widget;

	virtual const lv_font_t *get(LVGLObject *obj) const = 0;
	virtual void set(LVGLObject *obj, const lv_font_t *value) = 0;
};

/**
 * @class LVGLPropertyValT
 * @brief Template property that wraps an LVGL getter/setter function pair.
 * @tparam T The value type (int16_t, uint16_t).
 *
 * Constructed with lambdas for the getter and setter, plus a function name
 * for code generation. This avoids creating a dedicated subclass for every
 * simple LVGL property — the lambda captures handle the specifics.
 */
template<class T>
class LVGLPropertyValT : public LVGLPropertyType<T>
{
public:
	LVGLPropertyValT(T min, T max, QString title,
						  QString functionName, std::function<void(lv_obj_t*, T)> setter,
						  std::function<T(lv_obj_t*)> getter,
						  LVGLProperty *parent = nullptr);

	QWidget *editor(QWidget *parent) override;
	void updateEditor(LVGLObject *obj) override;
	void updateWidget(LVGLObject *obj) override;

	QString name() const override;

	QStringList function(LVGLObject *obj) const override;

protected:
	virtual T get(LVGLObject *obj) const override;
	virtual void set(LVGLObject *obj, T value) override;

	QSpinBox *m_widget;
	T m_min;
	T m_max;
	QString m_title;
	QString m_functionName;
	std::function<void(lv_obj_t*, T)> m_setter;
	std::function<T(lv_obj_t*)> m_getter;

};

/**
 * @class LVGLPropertyValInt16
 * @brief Convenience subclass of LVGLPropertyValT<int16_t> with an explicit constructor.
 */
class LVGLPropertyValInt16 : public LVGLPropertyValT<int16_t>
{
public:
	LVGLPropertyValInt16(int16_t min, int16_t max, QString title,
								QString functionName, std::function<void(lv_obj_t*, int16_t)> setter,
								std::function<int16_t(lv_obj_t*)> getter,
								LVGLProperty *parent = nullptr);
};

/**
 * @class LVGLPropertyValUInt16
 * @brief Convenience subclass of LVGLPropertyValT<uint16_t> with an explicit constructor.
 */
class LVGLPropertyValUInt16 : public LVGLPropertyValT<uint16_t>
{
public:
	LVGLPropertyValUInt16(uint16_t min, uint16_t max, QString title,
								 QString functionName, std::function<void(lv_obj_t*, uint16_t)> setter,
								 std::function<uint16_t(lv_obj_t*)> getter,
								 LVGLProperty *parent = nullptr);
};

/**
 * @class LVGLPropertyBool
 * @brief Boolean property displayed as a True/False combo box.
 *
 * Supports both direct subclass override (via virtual get/set) and
 * lambda-based construction for simple LVGL flag properties.
 */
class LVGLPropertyBool : public LVGLPropertyType<bool>
{
public:
	LVGLPropertyBool(QString title = "", QString functionName = "", LVGLProperty *parent = nullptr);
	LVGLPropertyBool(QString title, QString functionName,
						  std::function<void(lv_obj_t*, bool)> setter,
						  std::function<bool(lv_obj_t*)> getter,
						  LVGLProperty *parent = nullptr);

	QWidget *editor(QWidget *parent) override;
	void updateEditor(LVGLObject *obj) override;
	void updateWidget(LVGLObject *obj) override;

	QString name() const override;

	QStringList function(LVGLObject *obj) const override;

protected:
	bool get(LVGLObject *obj) const override;
	void set(LVGLObject *obj, bool boolean) override;

	QComboBox *m_widget;
	QString m_title;
	QString m_functionName;
	std::function<void(lv_obj_t*, bool)> m_setter;
	std::function<bool(lv_obj_t*)> m_getter;

};

/**
 * @class LVGLPropertyString
 * @brief String property displayed as a line edit.
 *
 * Like LVGLPropertyBool, supports both subclass override and lambda
 * construction. Handles conversion between QString and const char*
 * for the LVGL C API.
 */
class LVGLPropertyString : public LVGLPropertyType<QString>
{
public:
	LVGLPropertyString(QString title = "", QString functionName = "", LVGLProperty *parent = nullptr);
	LVGLPropertyString(QString title, QString functionName,
							 std::function<void(lv_obj_t*, const char*)> setter,
							 std::function<const char*(lv_obj_t*)> getter,
							 LVGLProperty *parent = nullptr);

	QString name() const override;

	QWidget *editor(QWidget *parent) override;
	void updateEditor(LVGLObject *obj) override;
	void updateWidget(LVGLObject *obj) override;

	QStringList function(LVGLObject *obj) const override;

protected:
	QString get(LVGLObject *obj) const override;
	void set(LVGLObject *obj, QString string) override;

	QLineEdit *m_widget;
	QString m_title;
	QString m_functionName;
	std::function<void(lv_obj_t*, const char*)> m_setter;
	std::function<const char*(lv_obj_t*)> m_getter;

};

#endif // LVGLPROPERTY_H
