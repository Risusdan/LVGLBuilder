#ifndef LVGLDIALOG_H
#define LVGLDIALOG_H

/**
 * @file LVGLDialog.h
 * @brief Fixed-size dialog window that matches the LVGL display resolution.
 */

#include <QDialog>

/**
 * @class LVGLDialog
 * @brief QDialog sized to match the LVGL display resolution for previewing layouts.
 */
class LVGLDialog : public QDialog
{
	Q_OBJECT
public:
	explicit LVGLDialog(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // LVGLDIALOG_H
