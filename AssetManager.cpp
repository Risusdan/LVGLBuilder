#include "AssetManager.h"

#include <QFileInfo>

#include "LVGLCore.h"
#include "LVGLFontData.h"
#include "LVGLImageData.h"

AssetManager::AssetManager(QObject *parent) : QObject(parent) {}

LVGLImageData *AssetManager::addImageFile(const QString &fileName,
                                          const QString &name) {
  LVGLImageData *img = lvgl.addImage(fileName, name);
  if (img) {
    QString displayName =
        QFileInfo(fileName).baseName() +
        QString(" [%1x%2]").arg(img->width()).arg(img->height());
    emit imageAdded(img, displayName);
    emit imageListChanged(lvgl.images());
  }
  return img;
}

bool AssetManager::removeImage(LVGLImageData *img) {
  bool ok = lvgl.removeImage(img);
  if (ok) {
    emit imageRemoved();
    emit imageListChanged(lvgl.images());
  }
  return ok;
}

QList<LVGLImageData *> AssetManager::images() const { return lvgl.images(); }

QStringList AssetManager::imageNames() const {
  QStringList names;
  for (LVGLImageData *i : lvgl.images()) {
    if (i->fileName().isEmpty()) continue;
    names << QFileInfo(i->fileName()).baseName() +
                 QString(" [%1x%2]").arg(i->width()).arg(i->height());
  }
  return names;
}

LVGLFontData *AssetManager::addFont(const QString &fileName, uint8_t size) {
  LVGLFontData *f = lvgl.addFont(fileName, size);
  if (f) {
    emit fontAdded(f, f->name());
    emit fontListChanged(lvgl.customFonts());
  }
  return f;
}

bool AssetManager::removeFont(LVGLFontData *font) {
  bool ok = lvgl.removeFont(font);
  if (ok) {
    emit fontRemoved();
    emit fontListChanged(lvgl.customFonts());
  }
  return ok;
}

QList<const LVGLFontData *> AssetManager::customFonts() const {
  return lvgl.customFonts();
}

QStringList AssetManager::fontNames() const {
  QStringList names;
  for (const LVGLFontData *f : lvgl.customFonts()) names << f->name();
  return names;
}

void AssetManager::refreshImageList() { emit imageListChanged(lvgl.images()); }

void AssetManager::refreshFontList() {
  emit fontListChanged(lvgl.customFonts());
}
