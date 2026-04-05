/**
 * @file LVGLItem.h
 * @brief QGraphicsItem that displays an LVGLObject on the canvas with resize handles.
 *
 * LVGLItem is the visual representation of a selected widget in the
 * QGraphicsScene. It draws selection handles around the widget bounds and
 * translates mouse interactions into position/size changes on the underlying
 * LVGLObject.
 */

#ifndef LVGLITEM_H
#define LVGLITEM_H

#include <QGraphicsItem>

#include "LVGLObject.h"

/**
 * @class LVGLItem
 * @brief Graphics item providing selection visualization and interactive resizing.
 *
 * Inherits from both QObject (for signals/slots) and QGraphicsItem (for
 * scene rendering). Tracks which edges/corners the user is dragging via
 * the ResizeDirections struct to support 8-directional resizing.
 *
 * The item does not render the widget itself — the LVGL framebuffer handles
 * that. LVGLItem only draws the selection rectangle and resize handles on top.
 */
class LVGLItem : public QObject, public QGraphicsItem
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)
public:
	LVGLItem(QGraphicsItem *parent = nullptr);

	/** @brief Returns the bounding rect, matching the associated LVGLObject's geometry. */
	QRectF boundingRect() const override;

	/**
	 * @brief Paints the selection handles and border around the widget.
	 *
	 * Only draws when the item's object is selected. The widget content
	 * itself comes from the LVGL framebuffer painted by LVGLScene.
	 */
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

	/** @brief Returns the associated LVGLObject. */
	LVGLObject *object() const;
	/** @brief Sets the LVGLObject this item represents. */
	void setObject(LVGLObject *obj);

	/** @brief Returns true if the user is currently resizing via a handle. */
	bool isManipolating() const;

signals:
	/** @brief Emitted when the object's geometry changes due to user interaction. */
	void geometryChanged();

public slots:
	/** @brief Synchronizes the item's position and size with the LVGLObject. */
	void updateGeometry();

protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
	void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
	/**
	 * @brief Tracks which edges the user is dragging during a resize operation.
	 *
	 * Supports independent horizontal (Left/Right/None) and vertical
	 * (Top/Bottom/None) directions, enabling 8-directional resize from
	 * any edge or corner.
	 */
	struct ResizeDirections
	{
		enum { HorzNone, Left, Right } horizontal;
		enum { VertNone, Top, Bottom } vertical;
		bool any() const { return horizontal || vertical; }
		void clear() { horizontal = HorzNone; vertical = VertNone; }
	};

	LVGLObject *m_object;
	QRectF m_start;
	ResizeDirections m_direction;
	lv_coord_t m_minimumWidth;
	lv_coord_t m_minimumHeight;

};

#endif // LVGLITEM_H
