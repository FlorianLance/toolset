
# /*******************************************************************************
# ** Toolset-qt-utility                                                         **
# ** MIT License                                                                **
# ** Copyright (c) [2018] [Florian Lance]                                       **
# **                                                                            **
# ** Permission is hereby granted, free of charge, to any person obtaining a    **
# ** copy of this software and associated documentation files (the "Software"), **
# ** to deal in the Software without restriction, including without limitation  **
# ** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
# ** and/or sell copies of the Software, and to permit persons to whom the      **
# ** Software is furnished to do so, subject to the following conditions:       **
# **                                                                            **
# ** The above copyright notice and this permission notice shall be included in **
# ** all copies or substantial portions of the Software.                        **
# **                                                                            **
# ** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
# ** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
# ** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
# ** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
# ** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
# ** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
# ** DEALINGS IN THE SOFTWARE.                                                  **
# **                                                                            **
# ********************************************************************************/

####################################### repo
TOOLSET_REPOSITORY_DIR      = $$PWD"/../.."

####################################### PRI
# defines compiling options
include(../ts-settings.pri)
# defines projects paths and variables
include(../ts-projects.pri)
# defines thirdparty includes and libs
include(../ts-thirdparty.pri)
# defines dependencies
include(../ts-dependencies.pri)

####################################### TARGET
equals(CFG, "debug"){
    TARGET = qt-utilityd
}
equals(CFG, "release"){
    TARGET = qt-utility
}

####################################### TEMPLATE
TEMPLATE = lib
CONFIG += staticlib
CONFIG -= console

####################################### BUILD FILES
OBJECTS_DIR = $$QT_UTILITY_OBJ
MOC_DIR     = $$QT_UTILITY_MOC
RCC_DIR     = $$QT_UTILITY_RCC
UI_DIR      = $$QT_UTILITY_UI
DESTDIR     = $$QT_UTILITY_DEST

####################################### CONFIG
CONFIG += qt
QT += core gui opengl widgets printsupport network
DEFINES += QWT_DLL

####################################### DEPS
INCLUDEPATH += $$QT_UTILITY_DEP_INCLUDEPATH
LIBS += $$QT_UTILITY_DEP_LIBS
PRE_TARGETDEPS += $$QT_UTILITY_PRE_TARGETDEPS

####################################### PROJECT FILES

HEADERS += \
    # global    
    gui/widgets/base_qt_sfml_gl_widget.hpp \
    qt_highlighting.hpp \
    qt_io.hpp \
    qt_logger.hpp \
    qt_process.hpp \
    qt_str.hpp \
    qt_types.hpp \
    qt_ui.hpp \
    qt_convertors.hpp \
    # data
    data/argument.hpp \
    data/ex_widgets_types.hpp \
    data/id_key.hpp \
    data/unity_types.hpp \
    # widgets
    gui/widgets/code_editor_w.hpp \
    gui/widgets/custom_combobox_w.hpp \
    gui/widgets/curve_widget.hpp \
    gui/widgets/grabber_parameters_widget.hpp \
    gui/widgets/list_widget.hpp \
    gui/widgets/image_viewer.hpp \
    gui/widgets/rich_text_edit.hpp \
    gui/widgets/sfmlqt_gl_widget.hpp \
    gui/widgets/text_widget_highlighter.hpp \
    gui/widgets/tree_view_w.hpp \
    # ex_widgets
    gui/ex_widgets/ex_notepad_w.hpp \
    gui/ex_widgets/ex_base_w.hpp \
    gui/ex_widgets/ex_item_w.hpp \
    gui/ex_widgets/ex_checkbox_w.hpp \
    gui/ex_widgets/ex_code_editor_w.hpp \
    gui/ex_widgets/ex_color_frame_w.hpp \
    gui/ex_widgets/ex_combo_box_index_w.hpp \
    gui/ex_widgets/ex_combo_box_text_w.hpp \
    gui/ex_widgets/ex_curve_w.hpp \
    gui/ex_widgets/ex_curve_x_w.hpp \
    gui/ex_widgets/ex_double_spin_box_w.hpp \
    gui/ex_widgets/ex_float_spin_box_w.hpp \
    gui/ex_widgets/ex_label_w.hpp \
    gui/ex_widgets/ex_line_edit_w.hpp \
    gui/ex_widgets/ex_list_labels_w.hpp \
    gui/ex_widgets/ex_pushbutton_w.hpp \
    gui/ex_widgets/ex_radio_button_w.hpp \
    gui/ex_widgets/ex_select_color_w.hpp \
    gui/ex_widgets/ex_slider_w.hpp \
    gui/ex_widgets/ex_spin_box_w.hpp \
    gui/ex_widgets/ex_text_edit_w.hpp \
    gui/ex_widgets/ex_transformation_w.hpp \
    gui/ex_widgets/ex_vector2d_w.hpp \
    gui/ex_widgets/ex_vector3d_w.hpp \
    # wrapper
    wrapper/qt_gl_wrapper.hpp \

SOURCES += \
    # global    
    gui/widgets/base_qt_sfml_gl_widget.cpp \
    qt_highlighting.cpp \
    qt_logger.cpp \
    qt_str.cpp \
    qt_ui.cpp \
    # data
    data/argument.cpp \
    data/id_key.cpp \
    # widgets
    gui/widgets/code_editor_w.cpp \
    gui/widgets/grabber_parameters_widget.cpp \
    gui/widgets/image_viewer.cpp \
    gui/widgets/list_widget.cpp \
    gui/widgets/rich_text_edit.cpp \
    gui/widgets/sfmlqt_gl_widget.cpp \
    gui/widgets/curve_widget.cpp  \
    gui/widgets/text_widget_highlighter.cpp \
    gui/widgets/tree_view_w.cpp \
    # ex_widgets
    gui/ex_widgets/ex_notepad_w.cpp \
    gui/ex_widgets/ex_base_w.cpp \
    gui/ex_widgets/ex_checkbox_w.cpp \
    gui/ex_widgets/ex_code_editor_w.cpp \
    gui/ex_widgets/ex_color_frame_w.cpp \
    gui/ex_widgets/ex_combo_box_index_w.cpp \
    gui/ex_widgets/ex_combo_box_text_w.cpp \
    gui/ex_widgets/ex_curve_w.cpp \
    gui/ex_widgets/ex_curve_x_w.cpp \
    gui/ex_widgets/ex_double_spin_box_w.cpp \
    gui/ex_widgets/ex_float_spin_box_w.cpp \
    gui/ex_widgets/ex_label_w.cpp \
    gui/ex_widgets/ex_line_edit_w.cpp \
    gui/ex_widgets/ex_list_labels_w.cpp \
    gui/ex_widgets/ex_pushbutton_w.cpp \
    gui/ex_widgets/ex_radio_button_w.cpp \
    gui/ex_widgets/ex_select_color_w.cpp \
    gui/ex_widgets/ex_slider_w.cpp \
    gui/ex_widgets/ex_spin_box_w.cpp \
    gui/ex_widgets/ex_text_edit_w.cpp \
    gui/ex_widgets/ex_transformation_w.cpp \
    gui/ex_widgets/ex_vector2d_w.cpp \
    gui/ex_widgets/ex_vector3d_w.cpp \

RESOURCES += \
    $$TOOLSET_CPP_RESOURCES_DIR"/resources.qrc" \

FORMS += \
    gui/forms/dc_all_networks_settings_w.ui \
    gui/forms/dc_network_settings_w.ui \
    gui/forms/grabber_parameters.ui \


