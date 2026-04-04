cmake_minimum_required(VERSION 3.16)
project(LVGLBuilder LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# ---------------------------------------------------------------------------
# Vendored LVGL static library
# ---------------------------------------------------------------------------
file(GLOB LVGL_CORE_SRC    lvgl/lvgl/src/lv_core/*.c)
file(GLOB LVGL_DRAW_SRC    lvgl/lvgl/src/lv_draw/*.c)
file(GLOB LVGL_FONT_SRC    lvgl/lvgl/src/lv_font/*.c)
file(GLOB LVGL_HAL_SRC     lvgl/lvgl/src/lv_hal/*.c)
file(GLOB LVGL_MISC_SRC    lvgl/lvgl/src/lv_misc/*.c)
file(GLOB LVGL_OBJX_SRC    lvgl/lvgl/src/lv_objx/*.c)
file(GLOB LVGL_THEMES_SRC  lvgl/lvgl/src/lv_themes/*.c)

add_library(lvgl STATIC
    ${LVGL_CORE_SRC}
    ${LVGL_DRAW_SRC}
    ${LVGL_FONT_SRC}
    ${LVGL_HAL_SRC}
    ${LVGL_MISC_SRC}
    ${LVGL_OBJX_SRC}
    ${LVGL_THEMES_SRC}
)
target_include_directories(lvgl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lvgl)

# ---------------------------------------------------------------------------
# Vendored FreeType static library
# ---------------------------------------------------------------------------
set(FREETYPE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/freetype)

set(FREETYPE_SRC
    # Amalgamation (single-object) builds for each module
    ${FREETYPE_DIR}/src/autofit/autofit.c
    ${FREETYPE_DIR}/src/base/ftbase.c
    ${FREETYPE_DIR}/src/base/ftbitmap.c
    ${FREETYPE_DIR}/src/base/ftbbox.c
    ${FREETYPE_DIR}/src/base/ftdebug.c
    ${FREETYPE_DIR}/src/base/ftglyph.c
    ${FREETYPE_DIR}/src/base/ftfntfmt.c
    ${FREETYPE_DIR}/src/base/ftinit.c
    ${FREETYPE_DIR}/src/base/ftlcdfil.c
    ${FREETYPE_DIR}/src/base/ftmm.c
    ${FREETYPE_DIR}/src/base/ftsynth.c
    ${FREETYPE_DIR}/src/base/fttype1.c
    ${FREETYPE_DIR}/src/bdf/bdf.c
    ${FREETYPE_DIR}/src/cache/ftcache.c
    ${FREETYPE_DIR}/src/cff/cff.c
    ${FREETYPE_DIR}/src/cid/type1cid.c
    ${FREETYPE_DIR}/src/gzip/ftgzip.c
    ${FREETYPE_DIR}/src/lzw/ftlzw.c
    ${FREETYPE_DIR}/src/otvalid/otvalid.c
    ${FREETYPE_DIR}/src/pcf/pcf.c
    ${FREETYPE_DIR}/src/pfr/pfr.c
    ${FREETYPE_DIR}/src/psaux/psaux.c
    ${FREETYPE_DIR}/src/pshinter/pshinter.c
    ${FREETYPE_DIR}/src/psnames/psmodule.c
    ${FREETYPE_DIR}/src/raster/raster.c
    ${FREETYPE_DIR}/src/sfnt/sfnt.c
    ${FREETYPE_DIR}/src/smooth/smooth.c
    ${FREETYPE_DIR}/src/truetype/truetype.c
    ${FREETYPE_DIR}/src/type1/type1.c
    ${FREETYPE_DIR}/src/type42/type42.c
    ${FREETYPE_DIR}/src/winfonts/winfnt.c
)

if(WIN32)
    list(APPEND FREETYPE_SRC ${FREETYPE_DIR}/src/base/ftsystem.c)
else()
    list(APPEND FREETYPE_SRC ${FREETYPE_DIR}/builds/unix/ftsystem.c)
endif()

add_library(freetype STATIC ${FREETYPE_SRC})
target_include_directories(freetype PUBLIC ${FREETYPE_DIR}/include)
if(NOT WIN32)
    target_include_directories(freetype PRIVATE ${FREETYPE_DIR}/builds/unix)
endif()
target_compile_definitions(freetype PRIVATE
    FT_CONFIG_OPTION_ERROR_STRINGS
    FT2_BUILD_LIBRARY
)

# ---------------------------------------------------------------------------
# Application sources
# ---------------------------------------------------------------------------
set(APP_SOURCES
    AssetManager.cpp
    LVGLCore.cpp
    LVGLDisplay.cpp
    LVGLFontManager.cpp
    LVGLImageManager.cpp
    LVGLObjectManager.cpp
    LVGLWidgetRegistry.cpp
    LVGLDialog.cpp
    LVGLFontData.cpp
    LVGLFontDialog.cpp
    LVGLImageData.cpp
    LVGLItem.cpp
    LVGLNewDialog.cpp
    LVGLObject.cpp
    LVGLObjectModel.cpp
    LVGLProject.cpp
    LVGLProperty.cpp
    LVGLPropertyModel.cpp
    LVGLSimulator.cpp
    LVGLStyle.cpp
    LVGLStyleModel.cpp
    LVGLTabWidget.cpp
    LVGLWidgetListView.cpp
    LVGLWidgetModel.cpp
    LVGLWorker.cpp
    main.cpp
    MainWindow.cpp
    ProjectManager.cpp
    RecentFilesManager.cpp
    properties/LVGLPropertyAssignTextArea.cpp
    properties/LVGLPropertyColor.cpp
    properties/LVGLPropertyDate.cpp
    properties/LVGLPropertyDateList.cpp
    properties/LVGLPropertyFlags.cpp
    properties/LVGLPropertyGeometry.cpp
    properties/LVGLPropertyImage.cpp
    properties/LVGLPropertyList.cpp
    properties/LVGLPropertyPoints.cpp
    properties/LVGLPropertyRange.cpp
    properties/LVGLPropertyScale.cpp
    properties/LVGLPropertySeries.cpp
    properties/LVGLPropertyTextList.cpp
    properties/LVGLPropertyVal2.cpp
    widgets/LVGLArc.cpp
    widgets/LVGLBar.cpp
    widgets/LVGLButton.cpp
    widgets/LVGLButtonMatrix.cpp
    widgets/LVGLCalendar.cpp
    widgets/LVGLCanvas.cpp
    widgets/LVGLChart.cpp
    widgets/LVGLCheckBox.cpp
    widgets/LVGLColorPicker.cpp
    widgets/LVGLContainer.cpp
    widgets/LVGLDropDownList.cpp
    widgets/LVGLGauge.cpp
    widgets/LVGLImage.cpp
    widgets/LVGLImageButton.cpp
    widgets/LVGLImageSlider.cpp
    widgets/LVGLKeyboard.cpp
    widgets/LVGLLabel.cpp
    widgets/LVGLLED.cpp
    widgets/LVGLLine.cpp
    widgets/LVGLLineMeter.cpp
    widgets/LVGLList.cpp
    widgets/LVGLMessageBox.cpp
    widgets/LVGLPage.cpp
    widgets/LVGLPreloader.cpp
    widgets/LVGLSlider.cpp
    widgets/LVGLSwitch.cpp
    widgets/LVGLTabview.cpp
    widgets/LVGLTextArea.cpp
    widgets/LVGLWidget.cpp
)

set(APP_UI_FILES
    LVGLFontDialog.ui
    LVGLNewDialog.ui
    MainWindow.ui
)

set(APP_RESOURCES
    resources/res.qrc
)

# ---------------------------------------------------------------------------
# Executable target
# ---------------------------------------------------------------------------
qt_add_executable(LVGLBuilder
    ${APP_SOURCES}
    ${APP_UI_FILES}
    ${APP_RESOURCES}
)

target_include_directories(LVGLBuilder PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(LVGLBuilder PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    lvgl
    freetype
)

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------
enable_testing()
add_subdirectory(tests)
