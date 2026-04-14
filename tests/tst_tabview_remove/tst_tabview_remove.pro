QT += core gui widgets testlib
CONFIG += c++11 testcase
CONFIG += sdk_no_version_check

TARGET = tst_tabview_remove

include(../../lvgl/lvgl.pri)
include(../../freetype/freetype.pri)

# Use system zlib to avoid Byte typedef conflict between vendored
# FreeType's ancient zlib and newer macOS SDK TargetConditionals.h
macx {
    DEFINES += FT_CONFIG_OPTION_SYSTEM_ZLIB
    LIBS += -lz
}

INCLUDEPATH += ../../

RESOURCES += ../../resources/res.qrc

SOURCES += \
    tst_tabview_remove.cpp \
    ../../LVGLCore.cpp \
    ../../LVGLFontData.cpp \
    ../../LVGLObject.cpp \
    ../../LVGLProject.cpp \
    ../../LVGLProperty.cpp \
    ../../LVGLStyle.cpp \
    ../../LVGLImageData.cpp \
    ../../properties/LVGLPropertyAssignTextArea.cpp \
    ../../properties/LVGLPropertyColor.cpp \
    ../../properties/LVGLPropertyDate.cpp \
    ../../properties/LVGLPropertyDateList.cpp \
    ../../properties/LVGLPropertyFlags.cpp \
    ../../properties/LVGLPropertyGeometry.cpp \
    ../../properties/LVGLPropertyImage.cpp \
    ../../properties/LVGLPropertyList.cpp \
    ../../properties/LVGLPropertyPoints.cpp \
    ../../properties/LVGLPropertyRange.cpp \
    ../../properties/LVGLPropertyScale.cpp \
    ../../properties/LVGLPropertySeries.cpp \
    ../../properties/LVGLPropertyTextList.cpp \
    ../../properties/LVGLPropertyVal2.cpp \
    ../../widgets/LVGLArc.cpp \
    ../../widgets/LVGLBar.cpp \
    ../../widgets/LVGLButton.cpp \
    ../../widgets/LVGLButtonMatrix.cpp \
    ../../widgets/LVGLCalendar.cpp \
    ../../widgets/LVGLCanvas.cpp \
    ../../widgets/LVGLChart.cpp \
    ../../widgets/LVGLCheckBox.cpp \
    ../../widgets/LVGLColorPicker.cpp \
    ../../widgets/LVGLContainer.cpp \
    ../../widgets/LVGLDropDownList.cpp \
    ../../widgets/LVGLGauge.cpp \
    ../../widgets/LVGLImage.cpp \
    ../../widgets/LVGLImageButton.cpp \
    ../../widgets/LVGLImageSlider.cpp \
    ../../widgets/LVGLKeyboard.cpp \
    ../../widgets/LVGLLabel.cpp \
    ../../widgets/LVGLLED.cpp \
    ../../widgets/LVGLLine.cpp \
    ../../widgets/LVGLLineMeter.cpp \
    ../../widgets/LVGLList.cpp \
    ../../widgets/LVGLMessageBox.cpp \
    ../../widgets/LVGLPage.cpp \
    ../../widgets/LVGLPreloader.cpp \
    ../../widgets/LVGLSlider.cpp \
    ../../widgets/LVGLSwitch.cpp \
    ../../widgets/LVGLTabview.cpp \
    ../../widgets/LVGLTextArea.cpp \
    ../../widgets/LVGLWidget.cpp

HEADERS += \
    ../../LVGLCore.h \
    ../../LVGLFontData.h \
    ../../LVGLObject.h \
    ../../LVGLProject.h \
    ../../LVGLProperty.h \
    ../../LVGLStyle.h \
    ../../LVGLImageData.h \
    ../../properties/LVGLPropertyTextList.h \
    ../../widgets/LVGLWidgets.h \
    ../../widgets/LVGLWidget.h
