#ifndef LVGLWIDGETLISTVIEW_H
#define LVGLWIDGETLISTVIEW_H

/**
 * @file LVGLWidgetListView.h
 * @brief List view for the widget palette with drag-and-drop initiation support.
 */

#include <QListView>

/**
 * @class LVGLWidgetListView
 * @brief QListView subclass that initiates drags from the widget palette.
 */
class LVGLWidgetListView : public QListView
{
	Q_OBJECT
public:
	explicit LVGLWidgetListView(QWidget *parent = nullptr);

signals:

public slots:

protected:
	virtual void startDrag(Qt::DropActions supportedActions);

};

#endif // LVGLWIDGETLISTVIEW_H
