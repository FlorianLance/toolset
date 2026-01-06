
# /*******************************************************************************
# ** Toolset-ts-imgui-gl-engine                                                 **
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
TARGET = ts-imgui-gl-engine

TEMPLATE = lib
CONFIG += staticlib
CONFIG -= console
CONFIG -= qt

####################################### PRI
include(../ts-settings.pri)
include(../ts-projects.pri)
include(../ts-thirdparty.pri)
include(../ts-dependencies.pri)

####################################### INCLUDES

TS_IMGUI_GL_ENGINE_DEP_INCLUDEPATH =\
    #
    $$TS_IMGUI_GL_ENGINE_THIRDPARTY_INCLUDES\
    $$TS_BASE_THIRDPARTY_INCLUDES\
    $$TS_MESH_THIRDPARTY_INCLUDES\
    $$TS_OPENGL_THIRDPARTY_INCLUDES\
    #
    $$TS_BASE_INCLUDES\
    $$TS_OPENGL_INCLUDES\
    $$TS_MESH_INCLUDES\

####################################### LIBS

TS_IMGUI_GL_ENGINE_DEP_LIBS =\
    #
    $$TS_IMGUI_GL_ENGINE_THIRDPARTY_LIBS\
    $$TS_BASE_THIRDPARTY_LIBS\
    $$TS_MESH_THIRDPARTY_LIBS\
    $$TS_OPENGL_THIRDPARTY_LIBS\
    #
    $$TS_BASE_LIB\
    $$TS_OPENGL_LIB\
    $$TS_MESH_LIB\

####################################### DEP

TS_IMGUI_GL_ENGINE_PRE_TARGETDEPS =\
    $$TS_BASE_LIB_FILE\
    $$TS_OPENGL_LIB_FILE\
    $$TS_MESH_LIB_FILE\

####################################### GENERATE VARIABLES

include(../ts-gen-var.pri)

####################################### PROJECT FILES

HEADERS += \
    # ui
    imgui-tb/imgui_dc_calibrator_drawer.hpp \
    imgui-tb/imgui_dc_cloud_drawer.hpp \
    imgui-tb/imgui_dc_clouds_scene_drawer.hpp \
    imgui-tb/imgui_dc_device_drawer.hpp \
    imgui-tb/imgui_dc_direct_drawer.hpp \
    imgui-tb/imgui_dc_player_drawer.hpp \
    imgui-tb/imgui_dc_recorder_drawer.hpp \
    imgui-tb/imgui_dc_ui_drawer.hpp \
    immediate_ui.hpp \
    # engine
    engine/managers.hpp \
    # windows
    windows/base_sfml_gl_window.hpp \
    # imgui
    imgui/imgui.h \
    imgui/imconfig.h \
    imgui/imgui_internal.h \
    imgui/imstb_rectpack.h \
    imgui/imstb_textedit.h \
    imgui/imstb_truetype.h \
    imgui/misc/cpp/imgui_stdlib.h \
    imgui/extra/ImGuiFileDialog.h \
    imgui/extra/ImGuiFileDialogConfig.h \
    # imgui-sfml
    imgui-sfml/imgui-SFML.h \
    imgui-sfml/imgui-SFML_export.h \
    # imgui-tb
    imgui-tb/imgui_convert.hpp \
    imgui-tb/imgui_fbo_ui_drawer.hpp \
    imgui-tb/imgui_helper.hpp \
    imgui-tb/imgui_logs.hpp \
    imgui-tb/imgui_texture_ui_drawer.hpp \
    imgui-tb/imgui_types.hpp \
    imgui-tb/imgui_ui_drawer.hpp \
    # imgui-extra
    imgui/extra/implot/implot.h \
    imgui/extra/implot/implot_internal.h \
    # imgui/extra/imgui_markdown.h \
    # imgui/extra/imgui-node-editor/crude_json.h \
    # imgui/extra/imgui-node-editor/imgui_bezier_math.h \
    # imgui/extra/imgui-node-editor/imgui_bezier_math.inl \
    # imgui/extra/imgui-node-editor/imgui_canvas.h \
    # imgui/extra/imgui-node-editor/imgui_extra_math.h \
    # imgui/extra/imgui-node-editor/imgui_extra_math.inl \
    # imgui/extra/imgui-node-editor/imgui_node_editor.h \
    # imgui/extra/imgui-node-editor/imgui_node_editor_internal.h \
    # imgui/extra/imgui-node-editor/imgui_node_editor_internal.inl \

SOURCES += \
    # resources
    imgui-tb/imgui_dc_calibrator_drawer.cpp \
    imgui-tb/imgui_dc_cloud_drawer.cpp \
    imgui-tb/imgui_dc_clouds_scene_drawer.cpp \
    imgui-tb/imgui_dc_device_drawer.cpp \
    imgui-tb/imgui_dc_direct_drawer.cpp \
    imgui-tb/imgui_dc_player_drawer.cpp \
    imgui-tb/imgui_dc_recorder_drawer.cpp \
    imgui-tb/imgui_dc_ui_drawer.cpp \
    resources/resources.cpp \
    # windows
    windows/base_sfml_gl_window.cpp \
    # imgui
    imgui/imgui.cpp \
    imgui/imgui_draw.cpp \
    imgui/imgui_widgets.cpp \
    imgui/imgui_tables.cpp \
    imgui/imgui_demo.cpp \
    imgui/misc/cpp/imgui_stdlib.cpp \
    # imgui-sfml
    imgui-sfml/imgui-SFML.cpp \
    # imgui-tb
    imgui-tb/imgui_fbo_ui_drawer.cpp \
    imgui-tb/imgui_logs.cpp \
    imgui-tb/imgui_texture_ui_drawer.cpp \
    imgui-tb/imgui_ui_drawer.cpp \
    # imgui-extra
    imgui/extra/implot/implot.cpp \
    imgui/extra/implot/implot_items.cpp \
    imgui/extra/ImGuiFileDialog.cpp \
    # imgui/extra/imgui-node-editor/crude_json.cpp \
    # imgui/extra/imgui-node-editor/imgui_canvas.cpp \
    # imgui/extra/imgui-node-editor/imgui_node_editor.cpp \
    # imgui/extra/imgui-node-editor/imgui_node_editor_api.cpp \




