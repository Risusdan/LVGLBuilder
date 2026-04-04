#include "LVGLImageManager.h"

LVGLImageManager::LVGLImageManager(QObject *parent)
    : QObject(parent), m_default(nullptr) {}

LVGLImageManager::~LVGLImageManager() { qDeleteAll(m_images); }

LVGLImageData *LVGLImageManager::addImage(QImage image, QString name) {
  if (image.isNull()) return nullptr;
  LVGLImageData *img = new LVGLImageData(image, "", name);
  m_images.insert(name, img);
  if (m_default == nullptr) m_default = img;
  return img;
}

LVGLImageData *LVGLImageManager::addImage(QString fileName, QString name) {
  QImage image(fileName);
  if (image.isNull()) return nullptr;

  if (name.isEmpty()) {
    // create sorted list by type
    QList<int> numbers;
    for (LVGLImageData *o : m_images) {
      if (o->fileName().isEmpty()) continue;
      const int index = o->name().lastIndexOf('_');
      const int num = o->name().mid(index + 1).toInt();
      auto ind = std::lower_bound(numbers.begin(), numbers.end(), num);
      numbers.insert(ind, num);
    }

    // find next free id
    int id = 1;
    for (int num : numbers) {
      if (num == id)
        id++;
      else
        break;
    }
    name = QString("img_%1").arg(id);
  }

  LVGLImageData *img = new LVGLImageData(image, fileName, name);
  m_images.insert(name, img);
  if (m_default == nullptr) m_default = img;
  return img;
}

void LVGLImageManager::addImage(LVGLImageData *image) {
  m_images.insert(image->name(), image);
  if (m_default == nullptr) m_default = image;
}

QStringList LVGLImageManager::imageNames() const { return m_images.keys(); }

QList<LVGLImageData *> LVGLImageManager::images() const {
  return m_images.values();
}

lv_img_dsc_t *LVGLImageManager::image(QString name) {
  LVGLImageData *img = m_images.value(name, nullptr);
  if (img) return img->img_des();
  return nullptr;
}

lv_img_dsc_t *LVGLImageManager::defaultImage() const {
  if (m_default == nullptr) return nullptr;
  return m_default->img_des();
}

QString LVGLImageManager::nameByImage(const lv_img_dsc_t *img_dsc) const {
  for (LVGLImageData *img : m_images) {
    if (img->img_des() == img_dsc) return img->name();
  }
  return "";
}

LVGLImageData *LVGLImageManager::imageByDesc(
    const lv_img_dsc_t *img_dsc) const {
  for (LVGLImageData *img : m_images) {
    if (img->img_des() == img_dsc) return img;
  }
  return nullptr;
}

bool LVGLImageManager::removeImage(LVGLImageData *img) {
  for (auto it = m_images.begin(); it != m_images.end(); ++it) {
    if (it.value() == img) {
      m_images.remove(it.key());
      if (m_default == img)
        m_default = nullptr;
      delete img;
      return true;
    }
  }
  return false;
}

void LVGLImageManager::removeAllImages() {
  qDeleteAll(m_images);
  m_images.clear();
  m_default = nullptr;
}
