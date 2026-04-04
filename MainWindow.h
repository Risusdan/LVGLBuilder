#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidgetItem>
#include <QMainWindow>
#include <QSlider>

class LVGLSimulator;
class LVGLObject;
class LVGLImageData;
class LVGLFontData;
class ProjectManager;
class RecentFilesManager;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  LVGLSimulator *simulator() const;

 private slots:
  void updateProperty();
  void setCurrentObject(LVGLObject *obj);
  void styleChanged();
  void openNewProject();

  void on_action_load_triggered();
  void on_action_save_triggered();
  void on_combo_style_currentIndexChanged(int index);
  void on_action_export_c_triggered();
  void on_button_add_image_clicked();
  void on_button_remove_image_clicked();
  void on_list_images_customContextMenuRequested(const QPoint &pos);
  void on_list_images_currentItemChanged(QListWidgetItem *current,
                                         QListWidgetItem *previous);
  void on_button_add_font_clicked();
  void on_button_remove_font_clicked();
  void on_list_fonts_customContextMenuRequested(const QPoint &pos);
  void on_list_fonts_currentItemChanged(QListWidgetItem *current,
                                        QListWidgetItem *previous);
  void on_action_run_toggled(bool run);

  void tabchanged(int index);

 protected:
  void showEvent(QShowEvent *event);

 private:
  void addImage(LVGLImageData *img, QString name);
  void updateImages();

  void addFont(LVGLFontData *font, QString name);
  void updateFonts();

  void loadProject(const QString &fileName);
  void setEnableBuilder(bool enable);

  Ui::MainWindow *m_ui;
  QSlider *m_zoom_slider;
  ProjectManager *m_projectManager;

  class LVGLPropertyModel *m_propertyModel;
  class LVGLStyleModel *m_styleModel;
  class LVGLObjectModel *m_objectModel;

  RecentFilesManager *m_recentFilesManager;
  bool m_firstrun;
  LVGLSimulator *m_simulator;
  int m_lastindex;
};
#endif  // MAINWINDOW_H
