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
      m_imageManager(new LVGLImageManager(this)),
      m_fontManager(new LVGLFontManager(this)),
      m_objectManager(new LVGLObjectManager(this)),
      m_widgetRegistry(new LVGLWidgetRegistry(this)) {
}

LVGLCore::~LVGLCore() {
}

void LVGLCore::init(int width, int height) {
  lv_init();

  const uint32_t n = static_cast<uint32_t>(width * height);
  m_dispFrameBuf.resize(n);
  m_buf1.resize(n);
  m_buf2.resize(n);

  lv_disp_buf_init(&m_dispBuf, m_buf1.data(), m_buf2.data(), n);

  lv_disp_drv_init(&m_dispDrv);
  m_dispDrv.hor_res = static_cast<lv_coord_t>(width);
  m_dispDrv.ver_res = static_cast<lv_coord_t>(height);
  m_dispDrv.user_data = this;
  m_dispDrv.flush_cb = flushCb;
  m_dispDrv.buffer = &m_dispBuf;
  lv_disp_drv_register(&m_dispDrv);

  // to be sure that there is no button press at the start
  m_inputData.state = LV_INDEV_STATE_REL;

  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = inputCb;
  indev_drv.user_data = this;
  lv_indev_drv_register(&indev_drv);

  QImage pix(":/images/littlevgl_logo.png");
  m_imageManager->addImage(pix, "default");

  lv_style_copy(&m_screenStyle, &lv_style_scr);

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

  setScreenColor(Qt::white);
  changeResolution({width, height});
  // lv_log_register_print_cb(logCb);
}

bool LVGLCore::changeResolution(QSize size) {
  const uint32_t n = static_cast<uint32_t>(size.width() * size.height());
  if (n != m_dispBuf.size) {
    m_dispFrameBuf.resize(n);
    m_buf1.resize(n);
    m_buf2.resize(n);
    lv_disp_buf_init(&m_dispBuf, m_buf1.data(), m_buf2.data(), n);
  }

  m_dispDrv.hor_res = static_cast<lv_coord_t>(size.width());
  m_dispDrv.ver_res = static_cast<lv_coord_t>(size.height());
  lv_disp_drv_update(lv_disp_get_default(), &m_dispDrv);

  return false;
}

QPixmap LVGLCore::framebuffer() const {
  auto disp = lv_disp_get_default();
  auto width = lv_disp_get_hor_res(disp);
  auto height = lv_disp_get_ver_res(disp);

  QImage img(width, height, QImage::Format_ARGB32);
  memcpy(img.bits(), m_dispFrameBuf.data(),
         static_cast<size_t>(width * height) * 4);
  return QPixmap::fromImage(img);
}

QPixmap LVGLCore::grab(const QRect &region) const {
  const auto stride = lv_disp_get_hor_res(lv_disp_get_default());

  QImage img(region.width(), region.height(), QImage::Format_ARGB32);
  for (auto y = 0; y < region.height(); ++y)
    memcpy(img.scanLine(y + region.y()),
           &m_dispFrameBuf[static_cast<size_t>(y * stride + region.x())],
           static_cast<size_t>(stride) * 4);
  QPixmap pix;
  try {
    pix = QPixmap::fromImage(img);
  } catch (std::exception const &ex) {
    qDebug() << ex.what();
  }
  return pix;
}

LVGLObject *LVGLCore::findByName(const QString &name) {
  return m_objectManager->findByName(name);
}

int LVGLCore::width() const {
  return lv_disp_get_hor_res(lv_disp_get_default());
}

int LVGLCore::height() const {
  return lv_disp_get_ver_res(lv_disp_get_default());
}

QSize LVGLCore::size() const {
  auto disp = lv_disp_get_default();
  return QSize(lv_disp_get_hor_res(disp), lv_disp_get_ver_res(disp));
}

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
  return QStringList() << "LV_SYMBOL_AUDIO"
                       << "LV_SYMBOL_VIDEO"
                       << "LV_SYMBOL_LIST"
                       << "LV_SYMBOL_OK"
                       << "LV_SYMBOL_CLOSE"
                       << "LV_SYMBOL_POWER"
                       << "LV_SYMBOL_SETTINGS"
                       << "LV_SYMBOL_HOME"
                       << "LV_SYMBOL_DOWNLOAD"
                       << "LV_SYMBOL_DRIVE"
                       << "LV_SYMBOL_REFRESH"
                       << "LV_SYMBOL_MUTE"
                       << "LV_SYMBOL_VOLUME_MID"
                       << "LV_SYMBOL_VOLUME_MAX"
                       << "LV_SYMBOL_IMAGE"
                       << "LV_SYMBOL_EDIT"
                       << "LV_SYMBOL_PREV"
                       << "LV_SYMBOL_PLAY"
                       << "LV_SYMBOL_PAUSE"
                       << "LV_SYMBOL_STOP"
                       << "LV_SYMBOL_NEXT"
                       << "LV_SYMBOL_EJECT"
                       << "LV_SYMBOL_LEFT"
                       << "LV_SYMBOL_RIGHT"
                       << "LV_SYMBOL_PLUS"
                       << "LV_SYMBOL_MINUS"
                       << "LV_SYMBOL_EYE_OPEN"
                       << "LV_SYMBOL_EYE_CLOSE"
                       << "LV_SYMBOL_WARNING"
                       << "LV_SYMBOL_SHUFFLE"
                       << "LV_SYMBOL_UP"
                       << "LV_SYMBOL_DOWN"
                       << "LV_SYMBOL_LOOP"
                       << "LV_SYMBOL_DIRECTORY"
                       << "LV_SYMBOL_UPLOAD"
                       << "LV_SYMBOL_CALL"
                       << "LV_SYMBOL_CUT"
                       << "LV_SYMBOL_COPY"
                       << "LV_SYMBOL_SAVE"
                       << "LV_SYMBOL_CHARGE"
                       << "LV_SYMBOL_PASTE"
                       << "LV_SYMBOL_BELL"
                       << "LV_SYMBOL_KEYBOARD"
                       << "LV_SYMBOL_GPS"
                       << "LV_SYMBOL_FILE"
                       << "LV_SYMBOL_WIFI"
                       << "LV_SYMBOL_BATTERY_FULL"
                       << "LV_SYMBOL_BATTERY_3"
                       << "LV_SYMBOL_BATTERY_2"
                       << "LV_SYMBOL_BATTERY_1"
                       << "LV_SYMBOL_BATTERY_EMPTY"
                       << "LV_SYMBOL_USB"
                       << "LV_SYMBOL_BLUETOOTH"
                       << "LV_SYMBOL_TRASH"
                       << "LV_SYMBOL_BACKSPACE"
                       << "LV_SYMBOL_SD_CARD"
                       << "LV_SYMBOL_NEW_LINE";
}

const char *LVGLCore::symbol(const QString &name) const {
  if (name == "LV_SYMBOL_AUDIO")
    return LV_SYMBOL_AUDIO;
  else if (name == "LV_SYMBOL_VIDEO")
    return LV_SYMBOL_VIDEO;
  else if (name == "LV_SYMBOL_LIST")
    return LV_SYMBOL_LIST;
  else if (name == "LV_SYMBOL_OK")
    return LV_SYMBOL_OK;
  else if (name == "LV_SYMBOL_CLOSE")
    return LV_SYMBOL_CLOSE;
  else if (name == "LV_SYMBOL_POWER")
    return LV_SYMBOL_POWER;
  else if (name == "LV_SYMBOL_SETTINGS")
    return LV_SYMBOL_SETTINGS;
  else if (name == "LV_SYMBOL_HOME")
    return LV_SYMBOL_HOME;
  else if (name == "LV_SYMBOL_DOWNLOAD")
    return LV_SYMBOL_DOWNLOAD;
  else if (name == "LV_SYMBOL_DRIVE")
    return LV_SYMBOL_DRIVE;
  else if (name == "LV_SYMBOL_REFRESH")
    return LV_SYMBOL_REFRESH;
  else if (name == "LV_SYMBOL_MUTE")
    return LV_SYMBOL_MUTE;
  else if (name == "LV_SYMBOL_VOLUME_MID")
    return LV_SYMBOL_VOLUME_MID;
  else if (name == "LV_SYMBOL_VOLUME_MAX")
    return LV_SYMBOL_VOLUME_MAX;
  else if (name == "LV_SYMBOL_IMAGE")
    return LV_SYMBOL_IMAGE;
  else if (name == "LV_SYMBOL_EDIT")
    return LV_SYMBOL_EDIT;
  else if (name == "LV_SYMBOL_PREV")
    return LV_SYMBOL_PREV;
  else if (name == "LV_SYMBOL_PLAY")
    return LV_SYMBOL_PLAY;
  else if (name == "LV_SYMBOL_PAUSE")
    return LV_SYMBOL_PAUSE;
  else if (name == "LV_SYMBOL_STOP")
    return LV_SYMBOL_STOP;
  else if (name == "LV_SYMBOL_NEXT")
    return LV_SYMBOL_NEXT;
  else if (name == "LV_SYMBOL_EJECT")
    return LV_SYMBOL_EJECT;
  else if (name == "LV_SYMBOL_LEFT")
    return LV_SYMBOL_LEFT;
  else if (name == "LV_SYMBOL_RIGHT")
    return LV_SYMBOL_RIGHT;
  else if (name == "LV_SYMBOL_PLUS")
    return LV_SYMBOL_PLUS;
  else if (name == "LV_SYMBOL_MINUS")
    return LV_SYMBOL_MINUS;
  else if (name == "LV_SYMBOL_EYE_OPEN")
    return LV_SYMBOL_EYE_OPEN;
  else if (name == "LV_SYMBOL_EYE_CLOSE")
    return LV_SYMBOL_EYE_CLOSE;
  else if (name == "LV_SYMBOL_WARNING")
    return LV_SYMBOL_WARNING;
  else if (name == "LV_SYMBOL_SHUFFLE")
    return LV_SYMBOL_SHUFFLE;
  else if (name == "LV_SYMBOL_UP")
    return LV_SYMBOL_UP;
  else if (name == "LV_SYMBOL_DOWN")
    return LV_SYMBOL_DOWN;
  else if (name == "LV_SYMBOL_LOOP")
    return LV_SYMBOL_LOOP;
  else if (name == "LV_SYMBOL_DIRECTORY")
    return LV_SYMBOL_DIRECTORY;
  else if (name == "LV_SYMBOL_UPLOAD")
    return LV_SYMBOL_UPLOAD;
  else if (name == "LV_SYMBOL_CALL")
    return LV_SYMBOL_CALL;
  else if (name == "LV_SYMBOL_CUT")
    return LV_SYMBOL_CUT;
  else if (name == "LV_SYMBOL_COPY")
    return LV_SYMBOL_COPY;
  else if (name == "LV_SYMBOL_SAVE")
    return LV_SYMBOL_SAVE;
  else if (name == "LV_SYMBOL_CHARGE")
    return LV_SYMBOL_CHARGE;
  else if (name == "LV_SYMBOL_PASTE")
    return LV_SYMBOL_PASTE;
  else if (name == "LV_SYMBOL_BELL")
    return LV_SYMBOL_BELL;
  else if (name == "LV_SYMBOL_KEYBOARD")
    return LV_SYMBOL_KEYBOARD;
  else if (name == "LV_SYMBOL_GPS")
    return LV_SYMBOL_GPS;
  else if (name == "LV_SYMBOL_FILE")
    return LV_SYMBOL_FILE;
  else if (name == "LV_SYMBOL_WIFI")
    return LV_SYMBOL_WIFI;
  else if (name == "LV_SYMBOL_BATTERY_FULL")
    return LV_SYMBOL_BATTERY_FULL;
  else if (name == "LV_SYMBOL_BATTERY_3")
    return LV_SYMBOL_BATTERY_3;
  else if (name == "LV_SYMBOL_BATTERY_2")
    return LV_SYMBOL_BATTERY_2;
  else if (name == "LV_SYMBOL_BATTERY_1")
    return LV_SYMBOL_BATTERY_1;
  else if (name == "LV_SYMBOL_BATTERY_EMPTY")
    return LV_SYMBOL_BATTERY_EMPTY;
  else if (name == "LV_SYMBOL_USB")
    return LV_SYMBOL_USB;
  else if (name == "LV_SYMBOL_BLUETOOTH")
    return LV_SYMBOL_BLUETOOTH;
  else if (name == "LV_SYMBOL_TRASH")
    return LV_SYMBOL_TRASH;
  else if (name == "LV_SYMBOL_BACKSPACE")
    return LV_SYMBOL_BACKSPACE;
  else if (name == "LV_SYMBOL_SD_CARD")
    return LV_SYMBOL_SD_CARD;
  else if (name == "LV_SYMBOL_NEW_LINE")
    return LV_SYMBOL_NEW_LINE;
  else
    return nullptr;
}

void LVGLCore::poll() {
  lv_task_handler();
  lv_tick_inc(static_cast<uint32_t>(m_time.elapsed()));
  m_time.restart();
}

void LVGLCore::objsclear() { m_objectManager->clear(); }

void LVGLCore::sendMouseEvent(int x, int y, bool pressed) {
  m_inputData.point.x = static_cast<lv_coord_t>(x);
  m_inputData.point.y = static_cast<lv_coord_t>(y);
  m_inputData.state = pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}

QPoint LVGLCore::get_absolute_position(const lv_obj_t *obj) const {
  if (obj == lv_scr_act()) return QPoint(0, 0);
  int x = lv_obj_get_x(obj);
  int y = lv_obj_get_y(obj);
  lv_obj_t *parent = lv_obj_get_parent(obj);
  while (parent) {
    if (parent == lv_scr_act()) break;
    x += lv_obj_get_x(parent);
    y += lv_obj_get_y(parent);
    parent = lv_obj_get_parent(parent);
  }
  return QPoint(x, y);
}

QSize LVGLCore::get_object_size(const lv_obj_t *lv_obj) const {
  return QSize(lv_obj_get_width(lv_obj), lv_obj_get_height(lv_obj));
}

QRect LVGLCore::get_object_rect(const lv_obj_t *lv_obj) const {
  return QRect(get_absolute_position(lv_obj), get_object_size(lv_obj));
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
#if LV_COLOR_DEPTH == 24
  return QColor(c.ch.red, c.ch.green, c.ch.blue);
#elif LV_COLOR_DEPTH == 32
  return QColor(c.ch.red, c.ch.green, c.ch.blue, c.ch.alpha);
#endif
}

lv_color_t LVGLCore::fromColor(QColor c) const {
#if LV_COLOR_DEPTH == 24
  lv_color_t ret;
  ret.ch.red = c.red() & 0xff;
  ret.ch.green = c.green() & 0xff;
  ret.ch.blue = c.blue() & 0xff;
  return ret;
#elif LV_COLOR_DEPTH == 32
  lv_color_t ret;
  ret.ch.red = c.red() & 0xff;
  ret.ch.green = c.green() & 0xff;
  ret.ch.blue = c.blue() & 0xff;
  ret.ch.alpha = c.alpha() & 0xff;
  return ret;
#endif
}

lv_color_t LVGLCore::fromColor(QVariant v) const {
  if (v.typeId() == QMetaType::QVariantMap) {
    QVariantMap map = v.toMap();
#if LV_COLOR_DEPTH == 32
    lv_color_t c;
    c.ch.red = map["red"].toInt() & 0xff;
    c.ch.green = map["green"].toInt() & 0xff;
    c.ch.blue = map["blue"].toInt() & 0xff;
    c.ch.alpha = map["alpha"].toInt() & 0xff;
    return c;
#endif
  }
  return fromColor(v.value<QColor>());
}

QJsonObject LVGLCore::colorToJson(lv_color_t c) const {
  QJsonObject color(
      {{"red", c.ch.red}, {"green", c.ch.green}, {"blue", c.ch.blue}});
#if LV_COLOR_DEPTH == 32
  color.insert("alpha", c.ch.alpha);
#endif
  return color;
}

lv_color_t LVGLCore::colorFromJson(QJsonObject obj) const {
#if LV_COLOR_DEPTH == 32
  lv_color_t c;
  c.ch.red = obj["red"].toInt() & 0xff;
  c.ch.green = obj["green"].toInt() & 0xff;
  c.ch.blue = obj["blue"].toInt() & 0xff;
  c.ch.alpha = obj["alpha"].toInt() & 0xff;
  return c;
#else
  return lv_color_hex(0x000000);
#endif
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
  if (style == &lv_style_scr)
    return "lv_style_scr";
  else if (style == &lv_style_transp)
    return "lv_style_transp";
  else if (style == &lv_style_transp_fit)
    return "lv_style_transp_fit";
  else if (style == &lv_style_transp_tight)
    return "lv_style_transp_tight";
  else if (style == &lv_style_plain)
    return "lv_style_plain";
  else if (style == &lv_style_plain_color)
    return "lv_style_plain_color";
  else if (style == &lv_style_pretty)
    return "lv_style_pretty";
  else if (style == &lv_style_pretty_color)
    return "lv_style_pretty_color";
  else if (style == &lv_style_btn_rel)
    return "lv_style_btn_rel";
  else if (style == &lv_style_btn_pr)
    return "lv_style_btn_pr";
  else if (style == &lv_style_btn_tgl_rel)
    return "lv_style_btn_tgl_rel";
  else if (style == &lv_style_btn_tgl_pr)
    return "lv_style_btn_tgl_pr";
  else if (style == &lv_style_btn_ina)
    return "lv_style_btn_ina";
  return "";
}

void LVGLCore::setScreenColor(QColor color) {
  m_screenStyle.body.main_color = fromColor(color);
  m_screenStyle.body.grad_color = m_screenStyle.body.main_color;
  lv_obj_set_style(lv_scr_act(), &m_screenStyle);
}

QColor LVGLCore::screenColor() const {
  return toColor(m_screenStyle.body.main_color);
}

bool LVGLCore::screenColorChanged() const {
  return (
      m_screenStyle.body.main_color.full != lv_style_scr.body.main_color.full &&
      m_screenStyle.body.grad_color.full != lv_style_scr.body.grad_color.full);
}

QList<const LVGLWidget *> LVGLCore::widgets() const {
  return m_widgetRegistry->widgets();
}

const LVGLWidget *LVGLCore::widget(const QString &name) const {
  return m_widgetRegistry->widget(name);
}

void LVGLCore::tick() {
  lv_task_handler();
  lv_tick_inc(20);
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

void LVGLCore::flushHandler(lv_disp_drv_t *disp, const lv_area_t *area,
                            lv_color_t *color_p) {
  const auto stride = disp->hor_res;
  for (auto y = area->y1; y <= area->y2; ++y) {
    for (auto x = area->x1; x <= area->x2; ++x) {
      m_dispFrameBuf[x + y * stride].full = color_p->full;
      color_p++;
    }
  }
  lv_disp_flush_ready(disp);
}

bool LVGLCore::inputHandler(lv_indev_drv_t *indev_driver,
                            lv_indev_data_t *data) {
  (void)indev_driver;

  data->state = m_inputData.state;
  data->point.x = m_inputData.point.x;
  data->point.y = m_inputData.point.y;

  return false; /*Return `false` because we are not buffering and no more data
                   to read*/
}

void LVGLCore::flushCb(lv_disp_drv_t *disp, const lv_area_t *area,
                       lv_color_t *color_p) {
  LVGLCore *self = reinterpret_cast<LVGLCore *>(disp->user_data);
  self->flushHandler(disp, area, color_p);
}

bool LVGLCore::inputCb(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  LVGLCore *self = reinterpret_cast<LVGLCore *>(indev_driver->user_data);
  return self->inputHandler(indev_driver, data);
}

void LVGLCore::logCb(lv_log_level_t level, const char *file, uint32_t line,
                     const char *dsc) {
  qDebug().nospace() << file << " (" << line << "," << level << "): " << dsc;
}
