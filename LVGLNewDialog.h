#ifndef LVGLNEWDIALOG_H
#define LVGLNEWDIALOG_H

/**
 * @file LVGLNewDialog.h
 * @brief New project dialog for setting project name and display resolution.
 */

#include <QDialog>

#include "LVGLCore.h"

namespace Ui {
class LVGLNewDialog;
}

/**
 * @class LVGLNewDialog
 * @brief Dialog for creating a new project — collects project name and target resolution.
 */
class LVGLNewDialog : public QDialog
{
	Q_OBJECT

public:
	explicit LVGLNewDialog(QWidget *parent = nullptr);
	virtual ~LVGLNewDialog() override;

	QString selectedName() const;
	QSize selectedResolution() const;

public slots:
	virtual void accept() override;

private slots:
	void resolutionChanged(int index);

private:
	Ui::LVGLNewDialog *m_ui;
	QVector<QPair<lv_coord_t,lv_coord_t>> m_resolutions;

};

#endif // LVGLNEWDIALOG_H
