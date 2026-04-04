#include "MainWindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QSortFilterProxyModel>

#include "AssetManager.h"
#include "LVGLDialog.h"
#include "LVGLFontData.h"
#include "LVGLFontDialog.h"
#include "LVGLItem.h"
#include "LVGLNewDialog.h"
#include "LVGLObjectModel.h"
#include "LVGLProject.h"
#include "LVGLPropertyModel.h"
#include "ProjectManager.h"
#include "RecentFilesManager.h"
#include "LVGLSimulator.h"
#include "LVGLStyleModel.h"
#include "LVGLTabWidget.h"
#include "LVGLWidgetModel.h"
#include "ui_MainWindow.h"
#include "widgets/LVGLWidgets.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_ui(new Ui::MainWindow),
      m_zoom_slider(new QSlider(Qt::Horizontal)),
      m_projectManager(new ProjectManager(this)),
      m_firstrun(true),
      m_simulator(new LVGLSimulator(this)),
      m_lastindex(-1) {
  m_ui->setupUi(this);

  lvgl.init(320, 480);

  m_zoom_slider->setRange(-2, 2);
  connect(m_zoom_slider, &QSlider::valueChanged, m_simulator,
          &LVGLSimulator::setZoomLevel);
  m_ui->statusbar->addPermanentWidget(m_zoom_slider);

  m_ui->button_remove_image->setEnabled(false);
  m_ui->button_remove_font->setEnabled(false);

  m_propertyModel = new LVGLPropertyModel;
  connect(m_simulator, &LVGLSimulator::objectSelected, this,
          &MainWindow::setCurrentObject);
  connect(m_simulator, &LVGLSimulator::objectSelected, m_ui->property_tree,
          &QTreeView::expandAll);
  connect(m_simulator->item(), &LVGLItem::geometryChanged, this,
          &MainWindow::updateProperty);
  connect(m_ui->action_new, &QAction::triggered, this,
          &MainWindow::openNewProject);
  connect(m_simulator, &LVGLSimulator::objectAdded, m_ui->object_tree,
          &QTreeView::expandAll);

  m_ui->property_tree->setModel(m_propertyModel);
  m_ui->property_tree->setItemDelegate(new LVGLPropertyDelegate(this));

  m_objectModel = new LVGLObjectModel(this);
  connect(m_simulator, &LVGLSimulator::objectSelected, m_objectModel,
          &LVGLObjectModel::setCurrentObject);
  connect(m_ui->object_tree, &QTreeView::doubleClicked, this,
          [this](const QModelIndex &index) {
            m_simulator->setSelectedObject(m_objectModel->object(index));
          });
  m_ui->object_tree->setModel(m_objectModel);
  m_simulator->setObjectModel(m_objectModel);
  m_simulator->setPropertyModel(m_propertyModel);

  LVGLWidgetModel *widgetModel = new LVGLWidgetModel(this);
  QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
  proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
  connect(m_ui->edit_filter, &QLineEdit::textChanged, proxyModel,
          &QSortFilterProxyModel::setFilterWildcard);

  proxyModel->setSourceModel(widgetModel);
  proxyModel->sort(0);

  m_ui->list_widgets->setModel(proxyModel);

  m_styleModel = new LVGLStyleModel;
  connect(m_styleModel, &LVGLStyleModel::styleChanged, this,
          &MainWindow::styleChanged);
  m_ui->style_tree->setModel(m_styleModel);
  m_ui->style_tree->setItemDelegate(
      new LVGLStyleDelegate(m_styleModel->styleBase(), this));
  m_ui->style_tree->expandAll();

  // recent configurations
  m_recentFilesManager = new RecentFilesManager(5, this);
  m_recentFilesManager->setupMenu(m_ui->menu_resent_filess);
  connect(m_recentFilesManager, &RecentFilesManager::fileSelected,
          this, [this](const QString &f) { loadProject(f); });

  // asset manager
  m_assetManager = new AssetManager(this);
  connect(m_assetManager, &AssetManager::imageListChanged, this,
          [this](const QList<LVGLImageData *> &images) {
            m_ui->list_images->clear();
            for (LVGLImageData *i : images) {
              if (i->fileName().isEmpty()) continue;
              QString name =
                  QFileInfo(i->fileName()).baseName() +
                  QString(" [%1x%2]").arg(i->width()).arg(i->height());
              LVGLImageDataCast cast;
              cast.ptr = i;
              QListWidgetItem *item = new QListWidgetItem(i->icon(), name);
              item->setData(Qt::UserRole + 3, cast.i);
              m_ui->list_images->addItem(item);
            }
          });
  connect(m_assetManager, &AssetManager::fontListChanged, this,
          [this](const QList<const LVGLFontData *> &fonts) {
            m_ui->list_fonts->clear();
            for (const LVGLFontData *f : fonts) {
              LVGLFontDataCast cast;
              cast.ptr = const_cast<LVGLFontData *>(f);
              QListWidgetItem *item = new QListWidgetItem(f->name());
              item->setData(Qt::UserRole + 3, cast.i);
              m_ui->list_fonts->addItem(item);
            }
          });

  // add style editor dock to property dock and show the property dock
  tabifyDockWidget(m_ui->PropertyEditor, m_ui->StyleEditor);
  m_ui->PropertyEditor->raise();

  // add font editor dock to image dock and show the image dock
  tabifyDockWidget(m_ui->ImageEditor, m_ui->FontEditor);
  m_ui->ImageEditor->raise();
  connect(m_ui->tabWidget, &QTabWidget::currentChanged, this,
          &MainWindow::tabchanged);
}

MainWindow::~MainWindow() {
  lvgl.removeAllObjects();
  // m_simulator->setSelectedObject(nullptr);
  delete m_propertyModel;
  delete m_objectModel;
  delete m_styleModel;
  int curindex = m_ui->tabWidget->currentIndex();
  for (int i = 0; i < m_ui->tabWidget->count(); ++i) {
    if (i != curindex) {
      auto tabw = static_cast<LVGLTabWidget *>(m_ui->tabWidget->widget(i));
      tabw->removeAllObjects();
    }
  }
  delete m_ui;
}

LVGLSimulator *MainWindow::simulator() const { return m_simulator; }

void MainWindow::updateProperty() {
  LVGLObject *o = m_simulator->selectedObject();
  if (o == nullptr) return;
  LVGLProperty *p = o->widgetClass()->property("Geometry");
  if (p == nullptr) return;

  for (int i = 0; i < p->count(); ++i) {
    auto index = m_propertyModel->propIndex(p->child(i), o->widgetClass(), 1);
    emit m_propertyModel->dataChanged(index, index);
  }
}

void MainWindow::setCurrentObject(LVGLObject *obj) {
  m_ui->combo_style->clear();
  m_propertyModel->setObject(obj);
  if (obj) {
    m_ui->combo_style->addItems(obj->widgetClass()->styles());
    m_styleModel->setStyle(obj->style(0),
                           obj->widgetClass()->editableStyles(0));

  } else {
    m_styleModel->setStyle(nullptr);
  }
}

void MainWindow::styleChanged() {
  LVGLObject *obj = m_simulator->selectedObject();
  if (obj) {
    int index = m_ui->combo_style->currentIndex();
    obj->widgetClass()->setStyle(obj->obj(), index, obj->style(index));
    // refresh_children_style(obj->obj());
    // lv_obj_refresh_style(obj->obj());
  }
}

void MainWindow::openNewProject() {
  LVGLNewDialog dialog(this);
  if (dialog.exec() == QDialog::Accepted) {
    if (m_firstrun) {
      m_firstrun = false;
      m_projectManager->newProject(dialog.selectedName(),
                                   dialog.selectedResolution());
      const auto res = dialog.selectedResolution();
      lvgl.changeResolution(res);
      m_simulator->changeResolution(res);
    }
    setEnableBuilder(true);
    LVGLTabWidget *tabW = new LVGLTabWidget(this);
    tabW->setname(dialog.selectedName());
    m_simulator->setobjParent(tabW->getparent());
    m_ui->tabWidget->addTab(tabW, dialog.selectedName());
    m_ui->tabWidget->setCurrentIndex(m_ui->tabWidget->count() - 1);
  } else if (!m_projectManager->hasProject()) {
    setEnableBuilder(false);
    setWindowTitle("LVGL Builder");
  }
}

void MainWindow::loadProject(const QString &fileName) {
  m_simulator->clear();
  if (!m_projectManager->loadProject(fileName)) {
    QMessageBox::critical(this, "Error", "Could not load lvgl file!");
    setWindowTitle("LVGL Builder");
    setEnableBuilder(false);
  } else {
    m_recentFilesManager->addFile(fileName);
    setWindowTitle("LVGL Builder - [" + m_projectManager->projectName() + "]");
    const auto res = m_projectManager->project()->resolution();
    lvgl.changeResolution(res);
    m_simulator->changeResolution(res);
    setEnableBuilder(true);
  }
  m_assetManager->refreshImageList();
  m_assetManager->refreshFontList();
}

void MainWindow::setEnableBuilder(bool enable) {
  m_ui->action_save->setEnabled(enable);
  m_ui->action_export_c->setEnabled(enable);
  m_ui->action_run->setEnabled(enable);

  m_ui->WidgeBox->setEnabled(enable);
  m_ui->ImageEditor->setEnabled(enable);
  m_ui->FontEditor->setEnabled(enable);
}

void MainWindow::on_action_load_triggered() {
  QString path;
  if (m_projectManager->hasProject()) path = m_projectManager->fileName();
  QString fileName =
      QFileDialog::getOpenFileName(this, "Load lvgl", path, "LVGL (*.lvgl)");
  if (fileName.isEmpty()) return;
  loadProject(fileName);
}

void MainWindow::on_action_save_triggered() {
  QString path;
  if (m_projectManager->hasProject()) path = m_projectManager->fileName();
  QString fileName =
      QFileDialog::getSaveFileName(this, "Save lvgl", path, "LVGL (*.lvgl)");
  if (fileName.isEmpty()) return;
  if (!m_projectManager->saveProject(fileName)) {
    QMessageBox::critical(this, "Error", "Could not save lvgl file!");
  } else {
    m_recentFilesManager->addFile(fileName);
  }
}

void MainWindow::on_combo_style_currentIndexChanged(int index) {
  LVGLObject *obj = m_simulator->selectedObject();
  if (obj && (index >= 0) && (index < obj->widgetClass()->styles().size()))
    m_styleModel->setStyle(obj->style(index),
                           obj->widgetClass()->editableStyles(index));
}

void MainWindow::on_action_export_c_triggered() {
  QString dir;
  if (m_projectManager->hasProject()) {
    QFileInfo fi(m_projectManager->fileName());
    dir = fi.absoluteFilePath();
  }
  QString path = QFileDialog::getExistingDirectory(this, "Export C files", dir);
  if (path.isEmpty()) return;
  if (m_projectManager->exportCode(path))
    QMessageBox::information(this, "Export", "C project exported!");
}

void MainWindow::on_button_add_image_clicked() {
  QString dir;
  if (m_projectManager->hasProject()) {
    QFileInfo fi(m_projectManager->fileName());
    dir = fi.absoluteFilePath();
  }
  QStringList fileNames = QFileDialog::getOpenFileNames(
      this, "Import image", dir, "Image (*.png *.jpg *.bmp *.jpeg)");
  for (const QString &fileName : fileNames) {
    QImage image(fileName);
    if (image.isNull()) continue;
    if (image.width() >= 2048 || image.height() >= 2048) {
      QMessageBox::critical(
          this, "Error Image Size",
          tr("Image size must be under 2048! (Src: '%1')").arg(fileName));
      continue;
    }

    QString name = QFileInfo(fileName).baseName();
    m_assetManager->addImageFile(fileName, name);
  }
}

void MainWindow::on_button_remove_image_clicked() {
  QListWidgetItem *item = m_ui->list_images->currentItem();
  if (item == nullptr) return;

  LVGLImageDataCast cast;
  cast.i = item->data(Qt::UserRole + 3).toLongLong();

  m_assetManager->removeImage(cast.ptr);
}

void MainWindow::on_list_images_customContextMenuRequested(const QPoint &pos) {
  QPoint item = m_ui->list_images->mapToGlobal(pos);
  QListWidgetItem *listItem = m_ui->list_images->itemAt(pos);
  if (listItem == nullptr) return;

  QMenu menu;
  QAction *save = menu.addAction("Save as ...");
  QAction *color = menu.addAction("Set output color ...");
  QAction *sel = menu.exec(item);
  if (sel == save) {
    LVGLImageDataCast cast;
    cast.i = listItem->data(Qt::UserRole + 3).toLongLong();

    QStringList options({"C Code (*.c)", "Binary (*.bin)"});
    QString selected;
    QString fileName = QFileDialog::getSaveFileName(
        this, "Save image as c file", cast.ptr->codeName(), options.join(";;"),
        &selected);
    if (fileName.isEmpty()) return;
    bool ok = false;
    if (selected == options.at(0))
      ok = cast.ptr->saveAsCode(fileName);
    else if (selected == options.at(1))
      ok = cast.ptr->saveAsBin(fileName);
    if (!ok) {
      QMessageBox::critical(this, "Error",
                            tr("Could not save image '%1'").arg(fileName));
    }
  } else if (sel == color) {
    LVGLImageDataCast cast;
    cast.i = listItem->data(Qt::UserRole + 3).toLongLong();
    int index = static_cast<int>(cast.ptr->colorFormat());
    QString ret =
        QInputDialog::getItem(this, "Output color", "Select output color",
                              LVGLImageData::colorFormats(), index, false);
    index = LVGLImageData::colorFormats().indexOf(ret);
    if (index >= 0)
      cast.ptr->setColorFormat(static_cast<LVGLImageData::ColorFormat>(index));
  }
}

void MainWindow::on_list_images_currentItemChanged(QListWidgetItem *current,
                                                   QListWidgetItem *previous) {
  Q_UNUSED(previous)
  m_ui->button_remove_image->setEnabled(current != nullptr);
}

void MainWindow::on_button_add_font_clicked() {
  LVGLFontDialog dialog(this);
  if (dialog.exec() != QDialog::Accepted) return;
  LVGLFontData *f =
      m_assetManager->addFont(dialog.selectedFontPath(), dialog.selectedFontSize());
  if (!f)
    QMessageBox::critical(this, "Error", "Could not load font!");
}

void MainWindow::on_button_remove_font_clicked() {
  QListWidgetItem *item = m_ui->list_fonts->currentItem();
  if (item == nullptr) return;

  LVGLFontDataCast cast;
  cast.i = item->data(Qt::UserRole + 3).toLongLong();

  m_assetManager->removeFont(cast.ptr);
}

void MainWindow::on_list_fonts_customContextMenuRequested(const QPoint &pos) {
  QPoint item = m_ui->list_fonts->mapToGlobal(pos);
  QListWidgetItem *listItem = m_ui->list_fonts->itemAt(pos);
  if (listItem == nullptr) return;

  QMenu menu;
  QAction *save = menu.addAction("Save as ...");
  QAction *sel = menu.exec(item);
  if (sel == save) {
    LVGLFontDataCast cast;
    cast.i = listItem->data(Qt::UserRole + 3).toLongLong();

    QStringList options({"C Code (*.c)", "Binary (*.bin)"});
    QString selected;
    QString fileName = QFileDialog::getSaveFileName(
        this, "Save font as c file", cast.ptr->codeName(), options.join(";;"),
        &selected);
    if (fileName.isEmpty()) return;
    bool ok = false;
    if (selected == options.at(0)) ok = cast.ptr->saveAsCode(fileName);
    if (!ok) {
      QMessageBox::critical(this, "Error",
                            tr("Could not save font '%1'").arg(fileName));
    }
  }
}

void MainWindow::on_list_fonts_currentItemChanged(QListWidgetItem *current,
                                                  QListWidgetItem *previous) {
  Q_UNUSED(previous)
  m_ui->button_remove_font->setEnabled(current != nullptr);
}

void MainWindow::on_action_run_toggled(bool run) {
  m_simulator->setMouseEnable(run);
  m_simulator->setSelectedObject(nullptr);
}

void MainWindow::tabchanged(int index) {
  if (m_lastindex != -1) {
    auto oldtabw =
        static_cast<LVGLTabWidget *>(m_ui->tabWidget->widget(m_lastindex));
    auto objs = lvgl.allObjects();
    oldtabw->setAllObjects(lvgl.allObjects());
    for (int i = 0; i < objs.count(); ++i) {
      m_objectModel->beginRemoveObject(objs[i]);
      m_objectModel->endRemoveObject();
    }
    lvgl.objsclear();
  }
  m_lastindex = index;
  auto tabw = static_cast<LVGLTabWidget *>(m_ui->tabWidget->widget(index));
  m_simulator->setobjParent(tabw->getparent());
  tabw->setSimulator(m_simulator);
  auto objs = tabw->allObject();
  lvgl.setAllObjects(objs);
  if (m_projectManager->hasProject())
    m_projectManager->project()->setName(tabw->getname());
  for (int i = 0; i < objs.count(); ++i) {
    m_objectModel->beginInsertObject(objs[i]);
    m_objectModel->endInsertObject();
  }
  m_ui->object_tree->update();
  if (!objs.isEmpty()) m_simulator->setSelectedObject(lvgl.allObjects().at(0));
  m_simulator->setSelectedObject(nullptr);  // need it
}

void MainWindow::showEvent(QShowEvent *event) {
  QMainWindow::showEvent(event);
  if (!m_projectManager->hasProject())
    QTimer::singleShot(50, this, &MainWindow::openNewProject);
}
