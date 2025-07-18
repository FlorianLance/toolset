
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

####################################### TARGET/TEMPMATE/CONFIG
TARGET = qt-utility
TEMPLATE = lib
CONFIG += staticlib
CONFIG -= console
CONFIG += qt
QT += core gui opengl widgets printsupport network
DEFINES += QWT_DLL

####################################### PRI
include(../ts-settings.pri)
include(../ts-projects.pri)
include(../ts-thirdparty.pri)
include(../ts-dependencies.pri)

####################################### PROJECT FILES
HEADERS += \
    # global    
    base_app_controller.hpp \
    gui/items/qt_plot_graphic_item.hpp \
    gui/widgets/__grabber_parameters_widget.hpp \
    gui/widgets/__image_viewer.hpp \
    gui/widgets/__qt_plot_multi_curves_2d_w.h \
    gui/widgets/__qt_rich_text_edit_w.hpp \
    gui/widgets/__qt_sfml_gl_w.hpp \
    gui/widgets/__qt_tree_view_w.hpp \
    gui/widgets/qt_bar_plot_w.hpp \
    gui/widgets/qt_base_sfml_gl_w.hpp \
    gui/widgets/qt_code_editor_w.hpp \
    gui/widgets/qt_custom_combobox_w.hpp \
    gui/widgets/qt_fast_multi_curves_plot_w.hpp \
    gui/widgets/qt_list_w.hpp \
    gui/widgets/qt_logger_w.hpp \
    gui/widgets/qt_multi_curves_plot_w.hpp \
    gui/widgets/qt_speedometer_w.hpp \
    gui/widgets/qt_text_widget_highlighter.hpp \
    qstring_unordered_map.hpp \
    qt_highlighting.hpp \
    qt_io.hpp \
    qt_logger.hpp \
    qt_paths.hpp \
    qt_process.hpp \
    qt_str.hpp \
    qt_style.hpp \
    qt_types.hpp \
    qt_ui.hpp \
    qt_convertors.hpp \
    # data
    data/argument.hpp \
    data/ex_widgets_types.hpp \
    data/id_key.hpp \
    data/unity_types.hpp \
    # widgets
    gui/widgets/list_widget.hpp \
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
    gui/items/qt_plot_graphic_item.cpp \
    gui/widgets/__grabber_parameters_widget.cpp \
    gui/widgets/__image_viewer.cpp \
    gui/widgets/__qt_plot_multi_curves_2d_w.cpp \
    gui/widgets/__qt_rich_text_edit_w.cpp \
    gui/widgets/__qt_sfml_gl_w.cpp \
    gui/widgets/__qt_tree_view_w.cpp \
    gui/widgets/qt_bar_plot_w.cpp \
    gui/widgets/qt_base_sfml_gl_w.cpp \
    gui/widgets/qt_code_editor_w.cpp \
    gui/widgets/qt_fast_multi_curves_plot_w.cpp \
    gui/widgets/qt_list_w.cpp \
    gui/widgets/qt_logger_w.cpp \
    gui/widgets/qt_multi_curves_plot_w.cpp \
    gui/widgets/qt_speedometer_w.cpp \
    gui/widgets/qt_text_widget_highlighter.cpp \
    qt_highlighting.cpp \
    qt_logger.cpp \
    qt_paths.cpp \
    qt_str.cpp \
    qt_style.cpp \
    qt_ui.cpp \
    # data
    data/argument.cpp \
    data/id_key.cpp \
    # widgets
    gui/widgets/list_widget.cpp \
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


