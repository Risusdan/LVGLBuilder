#ifndef LVGLOBJECT_H
#define LVGLOBJECT_H

#include <QObject>
#include <QJsonObject>

#include <lvgl/lvgl.h>

#include "widgets/LVGLWidget.h"

class LVGLObject : public QObject
{
	Q_OBJECT
public:
	LVGLObject(const LVGLWidget *widgetClass, QString name, LVGLObject *parent);
	LVGLObject(const LVGLWidget *widgetClass, QString name, lv_obj_t *parent);
	LVGLObject(const LVGLWidget *widgetClass, LVGLObject *parent, lv_obj_t *parentObj);
	LVGLObject(lv_obj_t *obj, const LVGLWidget *widgetClass, LVGLObject *parent, bool movable = true, int index = -1);
	~LVGLObject();

	lv_obj_t *obj() const;

	QString name() const;
	void setName(const QString &name);
	QString codeName() const;

	bool isLocked() const;
	void setLocked(bool locked);

	bool isMovable() const;

	const LVGLWidget *widgetClass() const;
	LVGLWidget::Type widgetType() const;

	QPoint absolutePosition() const;
	QPoint position() const;
	int x() const;
	int y() const;
	void setPosition(const QPoint &pos);
	void setX(int x);
	void setY(int y);

	QSize size() const;
	QRect geometry() const;
	void setGeometry(QRect geometry);

	int width() const;
	int height() const;
	void setWidth(int width);
	void setHeight(int height);

	QJsonObject toJson();
	QJsonArray jsonStyles() const;
	QStringList codeStyle(QString styleVar, int type) const;
	void parseStyles(const QJsonArray &styles);

	LVGLObject *parent() const;

	operator lv_obj_t *() noexcept;
	operator const lv_obj_t *() const noexcept;

	bool hasCustomStyle(int type) const;
	lv_style_t *style(int type);
	QString styleCodeName(int type) const;

	QList<LVGLObject *> childs() const;

	void removeChild(LVGLObject *child);

	bool isAccessible() const;
	void setAccessible(bool accessible);

	bool doesNameExists() const;
	static bool doesNameExists(const QString &name, LVGLObject *except = nullptr);
	void generateName();

	static LVGLObject *parse(QJsonObject object, LVGLObject *parent);

	int index() const;
	void setIndex(int index);

	LVGLObject *findChildByIndex(int index) const;

	/**
	 * @brief Detach the underlying lv_obj_t so the destructor won't delete it.
	 *
	 * Call this when an external owner (e.g. lv_tabview_remove_tab) will
	 * delete the lv_obj_t. Sets m_obj to nullptr so ~LVGLObject() is a no-op.
	 */
	void detachLvObj();

	/**
	 * @brief Recursively detach this object and all descendant LVGLObjects.
	 *
	 * Use when removing a container (e.g. a tab page) that has child widgets.
	 * lv_obj_del() on the container recursively frees all LVGL children,
	 * so every wrapper in the subtree must be detached to prevent double-free.
	 */
	void detachLvObjRecursive();

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
