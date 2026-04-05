/**
 * @file MainWindow.h
 * @brief Main application window coordinating all UI panels and project operations.
 */

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
class AssetManager;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief Top-level window that assembles the builder UI and wires components together.
 *
 * Owns and coordinates:
 * - Widget palette (left panel, driven by LVGLWidgetModel)
 * - Canvas simulator (center, LVGLSimulator)
 * - Property inspector (right panel, driven by LVGLPropertyModel)
 * - Object tree (right panel, driven by LVGLObjectModel)
 * - Style editor (right panel, driven by LVGLStyleModel)
 * - Image and font asset lists (right panel tabs)
 *
 * Also manages project-level operations (new, load, save, export) via
 * ProjectManager and tracks recent files via RecentFilesManager.
 */
class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  /** @brief Returns the canvas simulator widget. */
  LVGLSimulator *simulator() const;

 private slots:
  /** @brief Updates the property inspector when a property value changes. */
  void updateProperty();
  /** @brief Handles object selection — updates inspector, style editor, and object tree. */
  void setCurrentObject(LVGLObject *obj);
  /** @brief Refreshes the style editor when style values change. */
  void styleChanged();
  /** @brief Creates a new empty project with a resolution dialog. */
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
  /**
   * @brief Loads a project from a JSON file.
   * @param fileName Path to the .json project file.
   */
  void loadProject(const QString &fileName);
  /** @brief Enables or disables the builder UI panels (disabled during run mode). */
  void setEnableBuilder(bool enable);

  Ui::MainWindow *m_ui;
  QSlider *m_zoom_slider;
  ProjectManager *m_projectManager;
  AssetManager *m_assetManager;

  class LVGLPropertyModel *m_propertyModel;
  class LVGLStyleModel *m_styleModel;
  class LVGLObjectModel *m_objectModel;

  RecentFilesManager *m_recentFilesManager;
  bool m_firstrun;
  LVGLSimulator *m_simulator;
  int m_lastindex;
};
#endif  // MAINWINDOW_H
