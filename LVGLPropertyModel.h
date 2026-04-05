#ifndef LVGLPROPERTYMODEL_H
#define LVGLPROPERTYMODEL_H

/**
 * @file LVGLPropertyModel.h
 * @brief Qt item model and delegate for the property inspector panel.
 */

#include <QAbstractItemModel>
#include <QStyledItemDelegate>

#include "LVGLObject.h"
#include "LVGLProperty.h"

class LVGLWidget;

/**
 * @class LVGLPropertyModel
 * @brief QAbstractItemModel that exposes an LVGLObject's properties as a tree.
 *
 * Maps the LVGLProperty composite tree (parent/children) to Qt's model/view
 * framework. The property inspector (QTreeView) displays two columns: property
 * name and current value.
 */
class LVGLPropertyModel : public QAbstractItemModel {
  Q_OBJECT

 public:
  explicit LVGLPropertyModel(QObject *parent = nullptr);

  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

  // Basic functionality:
  QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &index) const override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

  LVGLObject *object() const;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  QModelIndex propIndex(const LVGLProperty *prop, const LVGLWidget *widget,
                        int column);

 public slots:
  void setObject(LVGLObject *obj);

 private:
  LVGLObject *m_obj;
};

/**
 * @class LVGLPropertyDelegate
 * @brief Custom delegate that creates property-specific editors in the inspector.
 *
 * Delegates editor creation to each LVGLProperty's editor() method, allowing
 * spin boxes for integers, combo boxes for enums, color pickers, etc.
 */
class LVGLPropertyDelegate : public QStyledItemDelegate {
 public:
  LVGLPropertyDelegate(QObject *parent = nullptr);

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const override;
  void setEditorData(QWidget *editor, const QModelIndex &index) const override;
  void setModelData(QWidget *editor, QAbstractItemModel *model,
                    const QModelIndex &index) const override;
};

#endif  // LVGLPROPERTYMODEL_H
