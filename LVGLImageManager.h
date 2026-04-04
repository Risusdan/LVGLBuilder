#ifndef LVGLIMAGEMANAGER_H
#define LVGLIMAGEMANAGER_H

#include <QHash>
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>

#include "LVGLImageData.h"

class LVGLImageManager : public QObject {
  Q_OBJECT
 public:
  explicit LVGLImageManager(QObject *parent = nullptr);
  ~LVGLImageManager();

  LVGLImageData *addImage(QImage image, QString name);
  LVGLImageData *addImage(QString fileName, QString name = QString());
  void addImage(LVGLImageData *image);

  QStringList imageNames() const;
  QList<LVGLImageData *> images() const;
  lv_img_dsc_t *image(QString name);
  lv_img_dsc_t *defaultImage() const;
  QString nameByImage(const lv_img_dsc_t *img_dsc) const;
  LVGLImageData *imageByDesc(const lv_img_dsc_t *img_dsc) const;
  bool removeImage(LVGLImageData *img);
  void removeAllImages();

 private:
  QHash<QString, LVGLImageData *> m_images;
  LVGLImageData *m_default;
};

#endif  // LVGLIMAGEMANAGER_H
