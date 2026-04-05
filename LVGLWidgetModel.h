#ifndef LVGLWIDGETMODEL_H
#define LVGLWIDGETMODEL_H

/**
 * @file LVGLWidgetModel.h
 * @brief Qt list model for the widget palette panel.
 */

#include <QAbstractListModel>

class LVGLWidget;

/**
 * @class LVGLWidgetModel
 * @brief QAbstractListModel that lists available widget types for drag-and-drop creation.
 *
 * Provides mime data encoding so that dragging a widget type from the palette
 * and dropping it on the canvas triggers widget creation in LVGLSimulator.
 */
class LVGLWidgetModel : public QAbstractListModel
{
	Q_OBJECT

public:
	explicit LVGLWidgetModel(QObject *parent = nullptr);

	// Basic functionality:
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	Qt::ItemFlags flags(const QModelIndex &index) const override;

	virtual QStringList mimeTypes() const override;
	virtual QMimeData *mimeData(const QModelIndexList &indexes) const override;

};

#endif // LVGLWIDGETMODEL_H
