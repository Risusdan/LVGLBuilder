#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <QList>
#include <QObject>
#include <QString>

class LVGLImageData;
class LVGLFontData;

class AssetManager : public QObject {
  Q_OBJECT

 public:
  explicit AssetManager(QObject *parent = nullptr);

  // Image operations
  LVGLImageData *addImageFile(const QString &fileName,
                              const QString &name = QString());
  bool removeImage(LVGLImageData *img);
  QList<LVGLImageData *> images() const;
  QStringList imageNames() const;

  // Font operations
  LVGLFontData *addFont(const QString &fileName, uint8_t size);
  bool removeFont(LVGLFontData *font);
  QList<const LVGLFontData *> customFonts() const;
  QStringList fontNames() const;

  // Bulk refresh
  void refreshImageList();
  void refreshFontList();

 signals:
  void imageListChanged(QList<LVGLImageData *> images);
  void fontListChanged(QList<const LVGLFontData *> fonts);
  void imageAdded(LVGLImageData *img, QString name);
  void imageRemoved();
  void fontAdded(LVGLFontData *font, QString name);
  void fontRemoved();
};

#endif  // ASSETMANAGER_H
