#include "LVGLCore.h"

#include <QDebug>
#include <QJsonObject>
#include <QPainter>

#include "LVGLFontData.h"
#include "LVGLObject.h"
#include "widgets/LVGLWidgets.h"

LVGLCore lvgl(nullptr);

const char *LVGLCore::DEFAULT_DAYS[7] = {"Su", "Mo", "Tu", "We",
                                         "Th", "Fr", "Sa"};
const char *LVGLCore::DEFAULT_MONTHS[12] = {
    "January", "February", "March",     "April",   "May",      "June",
    "July",    "August",   "September", "October", "November", "December"};

LVGLCore::LVGLCore(QObject *parent)
    : QObject(parent),
      m_display(new LVGLDisplay(this)),
      m_imageManager(new LVGLImageManager(this)),
      m_fontManager(new LVGLFontManager(this)),
      m_objectManager(new LVGLObjectManager(this)),
      m_widgetRegistry(new LVGLWidgetRegistry(this)) {
}

LVGLCore::~LVGLCore() {
}

void LVGLCore::init(int width, int height) {
  m_display->init(width, height);

  QImage pix(":/images/littlevgl_logo.png");
  m_imageManager->addImage(pix, "default");

  m_fontManager->initBuiltinFonts();

  registerWidget(new LVGLArc);
  registerWidget(new LVGLBar);
  registerWidget(new LVGLButton);
  registerWidget(new LVGLButtonMatrix);
  registerWidget(new LVGLCalendar);
  registerWidget(new LVGLCanvas);
  registerWidget(new LVGLChart);
  registerWidget(new LVGLCheckBox);
  registerWidget(new LVGLColorPicker);
  registerWidget(new LVGLContainer);
  registerWidget(new LVGLDropDownList);
  registerWidget(new LVGLGauge);
  registerWidget(new LVGLImage);
  registerWidget(new LVGLImageButton);
  registerWidget(new LVGLImageSlider);
  registerWidget(new LVGLKeyboard);
  registerWidget(new LVGLLabel);
  registerWidget(new LVGLLED);
  registerWidget(new LVGLLine);
  registerWidget(new LVGLLineMeter);
  registerWidget(new LVGLMessageBox);
  registerWidget(new LVGLList);
  registerWidget(new LVGLPage);
  registerWidget(new LVGLPreloader);
  registerWidget(new LVGLSlider);
  registerWidget(new LVGLSwitch);
  registerWidget(new LVGLTabview);
  registerWidget(new LVGLTextArea);

  m_display->setScreenColor(Qt::white);
  m_display->changeResolution({width, height});
}

bool LVGLCore::changeResolution(QSize size) {
  return m_display->changeResolution(size);
}

QPixmap LVGLCore::framebuffer() const { return m_display->framebuffer(); }

QPixmap LVGLCore::grab(const QRect &region) const {
  return m_display->grab(region);
}

LVGLObject *LVGLCore::findByName(const QString &name) {
  return m_objectManager->findByName(name);
}

int LVGLCore::width() const { return m_display->width(); }

int LVGLCore::height() const { return m_display->height(); }

QSize LVGLCore::size() const { return m_display->size(); }

LVGLImageData *LVGLCore::addImage(QImage image, QString name) {
  return m_imageManager->addImage(image, name);
}

LVGLImageData *LVGLCore::addImage(QString fileName, QString name) {
  return m_imageManager->addImage(fileName, name);
}

void LVGLCore::addImage(LVGLImageData *image) {
  m_imageManager->addImage(image);
}

QStringList LVGLCore::imageNames() const {
  return m_imageManager->imageNames();
}

QList<LVGLImageData *> LVGLCore::images() const {
  return m_imageManager->images();
}

lv_img_dsc_t *LVGLCore::image(QString name) {
  return m_imageManager->image(name);
}

lv_img_dsc_t *LVGLCore::defaultImage() const {
  return m_imageManager->defaultImage();
}

QString LVGLCore::nameByImage(const lv_img_dsc_t *img_dsc) const {
  return m_imageManager->nameByImage(img_dsc);
}

LVGLImageData *LVGLCore::imageByDesc(const lv_img_dsc_t *img_dsc) const {
  return m_imageManager->imageByDesc(img_dsc);
}

bool LVGLCore::removeImage(LVGLImageData *img) {
  return m_imageManager->removeImage(img);
}

void LVGLCore::removeAllImages() { m_imageManager->removeAllImages(); }

QStringList LVGLCore::symbolNames() const {
  return m_display->symbolNames();
}

const char *LVGLCore::symbol(const QString &name) const {
  return m_display->symbol(name);
}

void LVGLCore::poll() { m_display->poll(); }

void LVGLCore::objsclear() { m_objectManager->clear(); }

void LVGLCore::sendMouseEvent(int x, int y, bool pressed) {
  m_display->sendMouseEvent(x, y, pressed);
}

QPoint LVGLCore::get_absolute_position(const lv_obj_t *lv_obj) const {
  return m_display->get_absolute_position(lv_obj);
}

QSize LVGLCore::get_object_size(const lv_obj_t *lv_obj) const {
  return m_display->get_object_size(lv_obj);
}

QRect LVGLCore::get_object_rect(const lv_obj_t *lv_obj) const {
  return m_display->get_object_rect(lv_obj);
}

void LVGLCore::addObject(LVGLObject *object) {
  m_objectManager->addObject(object);
}

void LVGLCore::removeObject(LVGLObject *object) {
  m_objectManager->removeObject(object);
}

void LVGLCore::removeAllObjects() { m_objectManager->removeAllObjects(); }

void LVGLCore::setAllObjects(QList<LVGLObject *> objs) {
  m_objectManager->setAllObjects(objs);
}

QList<LVGLObject *> LVGLCore::allObjects() const {
  return m_objectManager->allObjects();
}

QList<LVGLObject *> LVGLCore::topLevelObjects() const {
  return m_objectManager->topLevelObjects();
}

QList<LVGLObject *> LVGLCore::objectsByType(QString className) const {
  return m_objectManager->objectsByType(className);
}

LVGLObject *LVGLCore::object(QString name) const {
  return m_objectManager->object(name);
}

LVGLObject *LVGLCore::object(lv_obj_t *obj) const {
  return m_objectManager->object(obj);
}

QColor LVGLCore::toColor(lv_color_t c) const {
  return m_display->toColor(c);
}

lv_color_t LVGLCore::fromColor(QColor c) const {
  return m_display->fromColor(c);
}

lv_color_t LVGLCore::fromColor(QVariant v) const {
  return m_display->fromColor(v);
}

QJsonObject LVGLCore::colorToJson(lv_color_t c) const {
  return m_display->colorToJson(c);
}

lv_color_t LVGLCore::colorFromJson(QJsonObject obj) const {
  return m_display->colorFromJson(obj);
}

LVGLFontData *LVGLCore::addFont(const QString &fileName, uint8_t size) {
  return m_fontManager->addFont(fileName, size);
}

void LVGLCore::addFont(LVGLFontData *font) { m_fontManager->addFont(font); }

bool LVGLCore::removeFont(LVGLFontData *font) {
  return m_fontManager->removeFont(font);
}

QStringList LVGLCore::fontNames() const { return m_fontManager->fontNames(); }

QStringList LVGLCore::fontCodeNames() const {
  return m_fontManager->fontCodeNames();
}

const lv_font_t *LVGLCore::font(int index) const {
  return m_fontManager->font(index);
}

const lv_font_t *LVGLCore::font(const QString &name,
                                Qt::CaseSensitivity cs) const {
  return m_fontManager->font(name, cs);
}

int LVGLCore::indexOfFont(const lv_font_t *font) const {
  return m_fontManager->indexOfFont(font);
}

QString LVGLCore::fontName(const lv_font_t *font) const {
  return m_fontManager->fontName(font);
}

QString LVGLCore::fontCodeName(const lv_font_t *font) const {
  return m_fontManager->fontCodeName(font);
}

QList<const LVGLFontData *> LVGLCore::customFonts() const {
  return m_fontManager->customFonts();
}

void LVGLCore::removeCustomFonts() { m_fontManager->removeCustomFonts(); }

QString LVGLCore::baseStyleName(const lv_style_t *style) const {
  return m_display->baseStyleName(style);
}

void LVGLCore::setScreenColor(QColor color) {
  m_display->setScreenColor(color);
}

QColor LVGLCore::screenColor() const { return m_display->screenColor(); }

bool LVGLCore::screenColorChanged() const {
  return m_display->screenColorChanged();
}

QList<const LVGLWidget *> LVGLCore::widgets() const {
  return m_widgetRegistry->widgets();
}

const LVGLWidget *LVGLCore::widget(const QString &name) const {
  return m_widgetRegistry->widget(name);
}

void LVGLCore::registerWidget(LVGLWidget *w) {
  auto size = w->minimumSize();
  if (size.width() > width() || size.height() > height())
    changeResolution(
        {std::max(size.width(), width()), std::max(size.height(), height())});

  setScreenColor(Qt::transparent);
  lv_obj_t *o = w->newObject(lv_scr_act());
  lv_obj_set_pos(o, 0, 0);
  lv_obj_set_size(o, w->minimumSize().width(), w->minimumSize().height());

  lv_scr_load(lv_scr_act());
  lv_tick_inc(LV_DISP_DEF_REFR_PERIOD);
  lv_task_handler();

  w->setPreview(
      /*grab(QRect(QPoint(0, 0), size))*/ framebuffer().copy({{0, 0}, size}));
  w->preview().save(w->name() + ".png");
  lv_obj_del(o);

  lv_scr_load(lv_scr_act());
  lv_tick_inc(LV_DISP_DEF_REFR_PERIOD);
  lv_task_handler();

  m_widgetRegistry->addWidget(w);
}
