#ifndef LVGLOBJECT_H
#define LVGLOBJECT_H

/**
 * @file LVGLObject.h
 * @brief Wrapper around lv_obj_t that adds Qt integration, hierarchy, and serialization.
 *
 * Every widget placed on the canvas is represented by an LVGLObject. It bridges
 * the gap between LVGL's C-based object system and Qt's object model, providing
 * name management, parent-child tracking, property access, JSON serialization,
 * and C code generation.
 */

#include <QObject>
#include <QJsonObject>

#include <lvgl/lvgl.h>

#include "widgets/LVGLWidget.h"

/**
 * @class LVGLObject
 * @brief Represents a single widget instance on the builder canvas.
 *
 * Wraps an lv_obj_t (the LVGL C widget handle) and layers Qt functionality
 * on top: named identity, hierarchical parent-child relationships, style
 * management, and the ability to serialize to JSON or generate C source code.
 *
 * LVGLObject does not subclass lv_obj_t — LVGL is a C library, so this class
 * acts as a companion object that owns and manages the lv_obj_t lifetime.
 * The implicit conversion operators allow passing LVGLObject directly to
 * LVGL C functions that expect lv_obj_t*.
 *
 * Four constructors exist to handle different creation contexts:
 * - From widget palette (new object with LVGLObject parent)
 * - From widget palette (new object with raw lv_obj_t parent, for root objects)
 * - Internal creation with explicit LVGL parent (currently unused)
 * - Wrapping a pre-existing lv_obj_t (used by TabView for page sub-objects)
 */
class LVGLObject : public QObject
{
	Q_OBJECT
public:
	/**
	 * @brief Creates a new widget as a child of an existing LVGLObject.
	 * @param widgetClass The widget type definition (e.g., LVGLButton).
	 * @param name Display name; auto-generated if empty.
	 * @param parent The parent LVGLObject in the widget tree.
	 * This is the primary constructor for user-created widgets via drag-and-drop.
	 */
	LVGLObject(const LVGLWidget *widgetClass, QString name, LVGLObject *parent);

	/**
	 * @brief Creates a new root-level widget with a raw LVGL parent.
	 * @param widgetClass The widget type definition.
	 * @param name Display name; auto-generated if empty.
	 * @param parent The raw lv_obj_t parent (typically the LVGL screen object).
	 * Used for top-level widgets that have no LVGLObject parent.
	 */
	LVGLObject(const LVGLWidget *widgetClass, QString name, lv_obj_t *parent);

	/**
	 * @brief Creates a sub-object with an explicit LVGL parent object.
	 * @param widgetClass The widget type definition.
	 * @param parent The logical parent LVGLObject.
	 * @param parentObj The raw lv_obj_t to use as LVGL parent (may differ from parent->obj()).
	 * Currently unused in the codebase. Intended for cases where the LVGL parent
	 * container differs from the logical LVGLObject parent.
	 */
	LVGLObject(const LVGLWidget *widgetClass, LVGLObject *parent, lv_obj_t *parentObj);

	/**
	 * @brief Wraps a pre-existing lv_obj_t as an LVGLObject.
	 * @param obj The pre-existing LVGL object to wrap (not created by this constructor).
	 * @param widgetClass The widget type definition.
	 * @param parent The parent LVGLObject, or nullptr for root.
	 * @param movable Whether the object can be dragged on the canvas.
	 * @param index Sub-object index (used by TabView pages); -1 if not applicable.
	 * Used by LVGLPropertyTabs to wrap TabView page objects that are created by
	 * lv_tabview_add_tab() rather than the normal newObject() path.
	 */
	LVGLObject(lv_obj_t *obj, const LVGLWidget *widgetClass, LVGLObject *parent, bool movable = true, int index = -1);

	~LVGLObject();

	/** @brief Returns the underlying LVGL object handle. */
	lv_obj_t *obj() const;

	/** @brief Returns the user-assigned display name. */
	QString name() const;
	/** @brief Sets the display name. */
	void setName(const QString &name);
	/**
	 * @brief Returns the C-safe identifier for code generation.
	 * Lowercases the display name, replaces non-alphanumeric characters with
	 * underscores, and prepends '_' if the result starts with a digit.
	 */
	QString codeName() const;

	/** @brief Returns true if the object is locked (cannot be selected or moved). */
	bool isLocked() const;
	/** @brief Sets the locked state. */
	void setLocked(bool locked);

	/** @brief Returns true if the object can be dragged on the canvas. */
	bool isMovable() const;

	/** @brief Returns the widget type definition for this instance. */
	const LVGLWidget *widgetClass() const;
	/** @brief Returns the widget type enum value. */
	LVGLWidget::Type widgetType() const;

	/** @brief Returns the absolute position on the display (accumulated from parents). */
	QPoint absolutePosition() const;
	/** @brief Returns the position relative to the parent widget. */
	QPoint position() const;
	/** @brief X coordinate relative to parent. */
	int x() const;
	/** @brief Y coordinate relative to parent. */
	int y() const;
	/** @brief Moves the widget to a new position relative to its parent. */
	void setPosition(const QPoint &pos);
	/** @brief Sets X coordinate relative to parent. */
	void setX(int x);
	/** @brief Sets Y coordinate relative to parent. */
	void setY(int y);

	/** @brief Returns the widget dimensions. */
	QSize size() const;
	/** @brief Returns position + size as a QRect. */
	QRect geometry() const;
	/** @brief Sets both position and size. */
	void setGeometry(QRect geometry);
	/** @brief Widget width in pixels. */
	int width() const;
	/** @brief Widget height in pixels. */
	int height() const;
	/** @brief Sets widget width. */
	void setWidth(int width);
	/** @brief Sets widget height. */
	void setHeight(int height);

	/**
	 * @brief Serializes the widget and its children to a JSON object.
	 * Includes widget type, name, geometry, properties, styles, and recursively
	 * serializes all child objects.
	 */
	QJsonObject toJson();

	/** @brief Returns the custom styles as a JSON array. */
	QJsonArray jsonStyles() const;

	/**
	 * @brief Generates C code lines for setting a custom style on this widget.
	 * @param styleVar The C variable name for the style struct.
	 * @param type The style type index.
	 * @return List of C code lines (e.g., "style_btn_main.body.main_color = ...").
	 */
	QStringList codeStyle(QString styleVar, int type) const;

	/** @brief Restores custom styles from a JSON array (during project load). */
	void parseStyles(const QJsonArray &styles);

	/** @brief Returns the parent LVGLObject, or nullptr for root objects. */
	LVGLObject *parent() const;

	/** @brief Implicit conversion to lv_obj_t* for use with LVGL C API. */
	operator lv_obj_t *() noexcept;
	/** @brief Implicit const conversion to lv_obj_t* for use with LVGL C API. */
	operator const lv_obj_t *() const noexcept;

	/** @brief Returns true if this widget has a custom (non-default) style for the given type. */
	bool hasCustomStyle(int type) const;
	/**
	 * @brief Returns or creates a custom style for the given type index.
	 * @param type The style type index (widget-specific, e.g., 0 = main style).
	 * @return Pointer to the style struct, created on first access.
	 */
	lv_style_t *style(int type);
	/** @brief Returns the C variable name for this widget's style. */
	QString styleCodeName(int type) const;

	/** @brief Returns the list of child LVGLObjects. */
	QList<LVGLObject *> childs() const;

	/** @brief Removes a child from this object's child list. */
	void removeChild(LVGLObject *child);

	/** @brief Returns true if this object is exported as a global variable in generated C code. */
	bool isAccessible() const;
	/** @brief Sets whether this object is exported as a global variable. */
	void setAccessible(bool accessible);

	/** @brief Returns true if another object already uses this name. */
	bool doesNameExists() const;
	/** @brief Static version: checks if any object (except @p except) uses the given name. */
	static bool doesNameExists(const QString &name, LVGLObject *except = nullptr);
	/** @brief Auto-generates a unique name based on the widget type. */
	void generateName();

	/**
	 * @brief Deserializes a widget tree from JSON.
	 * @param object The JSON object representing a single widget.
	 * @param parent The parent LVGLObject, or nullptr for root widgets.
	 * @return The reconstructed LVGLObject with all children.
	 */
	static LVGLObject *parse(QJsonObject object, LVGLObject *parent);

	/** @brief Returns the sub-object index (used by TabView pages); -1 if not a sub-object. */
	int index() const;

	/** @brief Detach the underlying lv_obj_t so the destructor won't delete it. */
	void detachLvObj() { m_obj = nullptr; }

	/** @brief Finds a child by its sub-object index. */
	LVGLObject *findChildByIndex(int index) const;

signals:
	void positionChanged();

private:
	lv_obj_t *m_obj;
	const LVGLWidget *m_widgetClass;
	QString m_name;
	bool m_locked;
	bool m_accessible;
	bool m_movable;
	int m_index;
	LVGLObject *m_parent;
	QList<LVGLObject*> m_childs;
	QHash<int,lv_style_t> m_styles;

};

#endif // LVGLOBJECT_H
