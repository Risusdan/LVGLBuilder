#ifndef LVGLOBJECTMODEL_H
#define LVGLOBJECTMODEL_H

/**
 * @file LVGLObjectModel.h
 * @brief Qt item model for the object tree panel showing the widget hierarchy.
 */

#include <QAbstractItemModel>
#include "LVGLCore.h"
#include "LVGLObject.h"

/**
 * @class LVGLObjectModel
 * @brief QAbstractItemModel that presents all LVGLObjects as a parent-child tree.
 *
 * Supports insert/remove notifications so the tree view stays synchronized
 * as widgets are added or deleted on the canvas.
 */
class LVGLObjectModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	explicit LVGLObjectModel(QObject *parent = nullptr);

	// Header:
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	// Basic functionality:
	QModelIndex index(int row, int column,
							const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	// Add object
	void beginInsertObject(LVGLObject *obj);
	void endInsertObject();

	// Remove object
	void beginRemoveObject(LVGLObject *obj);
	void endRemoveObject();

	LVGLObject *object(const QModelIndex &index) const;

public slots:
	void setCurrentObject(LVGLObject *obj);

private:
	QModelIndex objIndex(LVGLObject *obj, int col) const;

	LVGLObject *m_current;

};

#endif // LVGLOBJECTMODEL_H
