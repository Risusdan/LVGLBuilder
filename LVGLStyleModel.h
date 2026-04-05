#ifndef LVGLSTYLEMODEL_H
#define LVGLSTYLEMODEL_H

/**
 * @file LVGLStyleModel.h
 * @brief Qt item model, delegate, and body-part dialog for the style editor panel.
 */

#include <QAbstractItemModel>
#include <QStyledItemDelegate>
#include <QDialog>
#include "LVGLStyle.h"
#include "widgets/LVGLWidget.h"

class LVGLObject;

/**
 * @class LVGLStyleModel
 * @brief QAbstractItemModel that exposes an lv_style_t as an editable tree.
 *
 * Wraps an LVGLStyle root item and maps it to Qt's model/view framework for
 * display in the style editor panel.
 */
class LVGLStyleModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	explicit LVGLStyleModel(QObject *parent = nullptr);
	~LVGLStyleModel() override;

	// Header:
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	// Basic functionality:
	QModelIndex index(int row, int column,
							const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	Qt::ItemFlags flags(const QModelIndex& index) const override;

	LVGLStyle *styleBase() const;

	void setStyle(lv_style_t *style, LVGL::StyleParts editableStyles = LVGL::None);

signals:
	void styleChanged();

public slots:

private:
	void updateValue(const LVGLStyleItem *item);
	QModelIndex indexOf(const LVGLStyleItem *item, int column);

	LVGLStyle *m_styleBase;
	LVGL::StyleParts m_editableStyles;

};

/**
 * @class LVGLBodyPartDialog
 * @brief Dialog for selecting which border parts are active on a style.
 */
class LVGLBodyPartDialog : public QDialog
{
	Q_OBJECT
public:
	LVGLBodyPartDialog(QWidget *parent = nullptr);

	void setBodyParts(lv_border_part_t parts);
	lv_border_part_t bodyParts() const;

private:
	class QListWidget *m_list;

};

/**
 * @class LVGLStyleDelegate
 * @brief Custom delegate that creates style-field-specific editors in the style panel.
 */
class LVGLStyleDelegate : public QStyledItemDelegate
{
public:
	LVGLStyleDelegate(LVGLStyle *styleBase, QObject* parent = nullptr);

	QWidget *createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

private:
	LVGLStyle *m_styleBase;

};

#endif // LVGLSTYLEMODEL_H
