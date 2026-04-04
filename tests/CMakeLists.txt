# ---------------------------------------------------------------------------
# Test infrastructure for LVGLBuilder
# ---------------------------------------------------------------------------

find_package(Qt6 REQUIRED COMPONENTS Test)

# ---------------------------------------------------------------------------
# App sources (same as top-level APP_SOURCES, minus main.cpp)
# ---------------------------------------------------------------------------
set(APP_SOURCES_NO_MAIN
    ${CMAKE_SOURCE_DIR}/LVGLCore.cpp
    ${CMAKE_SOURCE_DIR}/LVGLDisplay.cpp
    ${CMAKE_SOURCE_DIR}/LVGLFontManager.cpp
    ${CMAKE_SOURCE_DIR}/LVGLImageManager.cpp
    ${CMAKE_SOURCE_DIR}/LVGLObjectManager.cpp
    ${CMAKE_SOURCE_DIR}/LVGLWidgetRegistry.cpp
    ${CMAKE_SOURCE_DIR}/LVGLDialog.cpp
    ${CMAKE_SOURCE_DIR}/LVGLFontData.cpp
    ${CMAKE_SOURCE_DIR}/LVGLFontDialog.cpp
    ${CMAKE_SOURCE_DIR}/LVGLImageData.cpp
    ${CMAKE_SOURCE_DIR}/LVGLItem.cpp
    ${CMAKE_SOURCE_DIR}/LVGLNewDialog.cpp
    ${CMAKE_SOURCE_DIR}/LVGLObject.cpp
    ${CMAKE_SOURCE_DIR}/LVGLObjectModel.cpp
    ${CMAKE_SOURCE_DIR}/LVGLProject.cpp
    ${CMAKE_SOURCE_DIR}/LVGLProperty.cpp
    ${CMAKE_SOURCE_DIR}/LVGLPropertyModel.cpp
    ${CMAKE_SOURCE_DIR}/LVGLSimulator.cpp
    ${CMAKE_SOURCE_DIR}/LVGLStyle.cpp
    ${CMAKE_SOURCE_DIR}/LVGLStyleModel.cpp
    ${CMAKE_SOURCE_DIR}/LVGLTabWidget.cpp
    ${CMAKE_SOURCE_DIR}/LVGLWidgetListView.cpp
    ${CMAKE_SOURCE_DIR}/LVGLWidgetModel.cpp
    ${CMAKE_SOURCE_DIR}/LVGLWorker.cpp
    ${CMAKE_SOURCE_DIR}/MainWindow.cpp
    ${CMAKE_SOURCE_DIR}/ProjectManager.cpp
    ${CMAKE_SOURCE_DIR}/properties/LVGLPropertyAssignTextArea.cpp
    ${CMAKE_SOURCE_DIR}/properties/LVGLPropertyColor.cpp
    ${CMAKE_SOURCE_DIR}/properties/LVGLPropertyDate.cpp
    ${CMAKE_SOURCE_DIR}/properties/LVGLPropertyDateList.cpp
    ${CMAKE_SOURCE_DIR}/properties/LVGLPropertyFlags.cpp
    ${CMAKE_SOURCE_DIR}/properties/LVGLPropertyGeometry.cpp
    ${CMAKE_SOURCE_DIR}/properties/LVGLPropertyImage.cpp
    ${CMAKE_SOURCE_DIR}/properties/LVGLPropertyList.cpp
    ${CMAKE_SOURCE_DIR}/properties/LVGLPropertyPoints.cpp
    ${CMAKE_SOURCE_DIR}/properties/LVGLPropertyRange.cpp
    ${CMAKE_SOURCE_DIR}/properties/LVGLPropertyScale.cpp
    ${CMAKE_SOURCE_DIR}/properties/LVGLPropertySeries.cpp
    ${CMAKE_SOURCE_DIR}/properties/LVGLPropertyTextList.cpp
    ${CMAKE_SOURCE_DIR}/properties/LVGLPropertyVal2.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLArc.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLBar.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLButton.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLButtonMatrix.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLCalendar.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLCanvas.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLChart.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLCheckBox.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLColorPicker.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLContainer.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLDropDownList.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLGauge.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLImage.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLImageButton.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLImageSlider.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLKeyboard.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLLabel.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLLED.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLLine.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLLineMeter.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLList.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLMessageBox.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLPage.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLPreloader.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLSlider.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLSwitch.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLTabview.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLTextArea.cpp
    ${CMAKE_SOURCE_DIR}/widgets/LVGLWidget.cpp
)

set(APP_UI_FILES
    ${CMAKE_SOURCE_DIR}/LVGLFontDialog.ui
    ${CMAKE_SOURCE_DIR}/LVGLNewDialog.ui
    ${CMAKE_SOURCE_DIR}/MainWindow.ui
)

set(APP_RESOURCES
    ${CMAKE_SOURCE_DIR}/resources/res.qrc
)

# ---------------------------------------------------------------------------
# Static library: compiled once, shared by all test executables
# ---------------------------------------------------------------------------
add_library(lvglbuilder_lib STATIC
    ${APP_SOURCES_NO_MAIN}
    ${APP_UI_FILES}
    ${APP_RESOURCES}
)

target_include_directories(lvglbuilder_lib PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_BINARY_DIR}
)

target_link_libraries(lvglbuilder_lib PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    lvgl
    freetype
)

# ---------------------------------------------------------------------------
# Helper function: add a QTest-based test
# ---------------------------------------------------------------------------
function(add_lvgl_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE lvglbuilder_lib Qt6::Test)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

# ---------------------------------------------------------------------------
# Test registrations
# ---------------------------------------------------------------------------
# add_lvgl_test(tst_example tst_example.cpp)
add_lvgl_test(tst_lvglproject tst_lvglproject/tst_lvglproject.cpp)
add_lvgl_test(tst_properties tst_properties/tst_properties.cpp)
add_lvgl_test(tst_lvglcore tst_lvglcore/tst_lvglcore.cpp)
add_lvgl_test(tst_projectmanager tst_projectmanager/tst_projectmanager.cpp)
