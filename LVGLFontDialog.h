#ifndef LVGLFONTDIALOG_H
#define LVGLFONTDIALOG_H

/**
 * @file LVGLFontDialog.h
 * @brief Dialog for loading custom fonts with size and BPP (bits per pixel) selection.
 */

#include <QDialog>

namespace Ui {
class LVGLFontDialog;
}

/**
 * @class LVGLFontDialog
 * @brief Dialog that lets the user browse for a TTF file and choose font size and BPP.
 */
class LVGLFontDialog : public QDialog
{
	Q_OBJECT

public:
	explicit LVGLFontDialog(QWidget *parent = nullptr);
	~LVGLFontDialog();

	QString selectedFontPath() const;
	uint8_t selectedFontSize() const;
	uint8_t selectedBPP() const;

private slots:
	void on_button_select_clicked();
	void on_list_font_currentTextChanged(const QString &currentText);

private:
	Ui::LVGLFontDialog *m_ui;
	QHash<QString, int> m_index;
	QHash<QString, QString> m_name;

};

#endif // LVGLFONTDIALOG_H
