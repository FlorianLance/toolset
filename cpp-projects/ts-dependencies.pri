
# /*******************************************************************************
# ** Toolset                                                                    **
# ** MIT License                                                                **
# ** Copyright (c) [2024] [Florian Lance]                                       **
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

########################################################### BASE
BASE_DEP_INCLUDEPATH =\
    # thirdparty
    ## general libaries
    $$BOOST_INCLUDES $$EIGEN_INCLUDES $$ASSIMP_INCLUDES\
    ## processing
    $$OPEN3D_INCLUDES $$OPENCV_INCLUDES\
    ## compression
    $$TURBOJPG_INCLUDES $$FASTPFOR_INCLUDES\
    # $$TURBOPFOR_INCLUDES\ # to be removed
    ## devices
    $$LIBSOUNDIO_INCLUDES \
    $$LIBUSB_INCLUDES \
    # $$KINECT2_INCLUDES \
    $$KINECT4_INCLUDES $$ORBBEC_INCLUDES\
    $$MODERN_JSON_INCLUDES\
    # $$VERSION2_INCLUDES\

BASE_DEP_LIBS =\
    ## general
    $$OS_LIBS $$BOOST_LIBS $$EIGEN_LIBS $$ASSIMP_LIBS\
    ## processing
    $$OPEN3D_LIBS $$OPENCV_LIBS\
    ## compression
    $$TURBOJPG_LIBS $$FASTPFOR_LIBS\
    # $$TURBOPFOR_LIBS\ # to be removed
    ## devices
    $$LIBSOUNDIO_LIBS \
    $$LIBUSB_LIBS \
    # $$KINECT2_LIBS
    $$KINECT4_LIBS $$ORBBEC_LIBS\


########################################################### BASE_APP
BASE_APP_DEP_INCLUDEPATH =\
    $$BASE_DEP_INCLUDEPATH\
    $$BASE_INCLUDES\
    $$VERSION2_INCLUDES\
    # $$PCL_INCLUDES\

BASE_APP_DEP_LIBS =\
    $$BASE_DEP_LIBS\
    $$BASE_LIB\
    # $$PCL_LIBS\

BASE_APP_PRE_TARGETDEPS =\
    $$BASE_LIB_FILE\

########################################################### BASE_TEST
BASE_TEST_DEP_INCLUDEPATH =\
    # thirdparty
    $$$$GLM_INCLUDES\
    # local
    $$BASE_DEP_INCLUDEPATH\
    $$BASE_INCLUDES\

BASE_TEST_DEP_LIBS =\
    # thidparty
    $$GLM_LIBS\
    # local
    $$BASE_DEP_LIBS\
    $$BASE_LIB\

BASE_TEST_PRE_TARGETDEPS =\
    $$BASE_LIB_FILE\

########################################################### BASE-EXPORT
BASE_EXPORT_DEP_INCLUDEPATH =\
    $$BASE_DEP_INCLUDEPATH\
    $$BASE_INCLUDES\

BASE_EXPORT_DEP_LIBS =\
    $$BASE_DEP_LIBS\
    $$BASE_LIB\

BASE_EXPORT_PRE_TARGETDEPS =\
    $$BASE_LIB_FILE\

########################################################### BASE-EXPORT-APP
BASE_EXPORT_APP_DEP_INCLUDEPATH =\
    $$BASE_EXPORT_DEP_INCLUDEPATH\
    $$BASE_EXPORT_INCLUDES\

BASE_EXPORT_APP_DEP_LIBS =\
    $$BASE_EXPORT_DEP_LIBS\
    $$BASE_EXPORT_OBJ"\*.obj"\

BASE_EXPORT_APP_PRE_TARGETDEPS =\
    $$BASE_LIB_FILE\

########################################################### OPENGL_UTILITY
OPENGL_UTILITY_DEP_INCLUDEPATH =\
    # thirdparty
    $$GLEW_INCLUDES $$GLFW_INCLUDES $$GLM_INCLUDES\
    # local
    $$BASE_DEP_INCLUDEPATH\
    $$BASE_INCLUDES\

OPENGL_UTILITY_DEP_LIBS =\
    # thidparty
    $$GLEW_LIBS $$GLFW_LIBS $$GLM_LIBS\
    # local
    $$BASE_DEP_LIBS\
    $$BASE_LIB\

OPENGL_UTILITY_PRE_TARGETDEPS =\
    $$BASE_LIB_FILE\

########################################################### OPENGL_UTILITY_APP
OPENGL_UTILITY_APP_DEP_INCLUDEPATH =\
    $$OPENGL_UTILITY_DEP_INCLUDEPATH\
    $$OPENGL_UTILITY_INCLUDES\

OPENGL_UTILITY_APP_DEP_LIBS =\
    $$OPENGL_UTILITY_DEP_LIBS\
    $$OPENGL_UTILITY_LIB\

OPENGL_UTILITY_APP_PRE_TARGETDEPS =\
    $$BASE_LIB_FILE\
    $$OPENGL_UTILITY_LIB_FILE\

########################################################### IMGUI_OPENGL_ENGINE
IMGUI_OPENGL_ENGINE_DEP_INCLUDEPATH =\
    # thidparty
    $$SFML_INCLUDES\
    # local
    $$OPENGL_UTILITY_DEP_INCLUDEPATH\
    $$OPENGL_UTILITY_INCLUDES\
    $$IMGUI_OPENGL_ENGINE_INCLUDES"/imgui"\

IMGUI_OPENGL_ENGINE_DEP_LIBS =\
    # thidparty
    $$SFML_LIBS\
    # local
    $$OPENGL_UTILITY_DEP_LIBS\
    $$OPENGL_UTILITY_LIB\

IMGUI_OPENGL_ENGINE_PRE_TARGETDEPS =\
    $$BASE_LIB_FILE\
    $$OPENGL_UTILITY_LIB_FILE\

########################################################### QT_UTILITY
QT_UTILITY_DEP_INCLUDEPATH =\
    # thirdparty
    $$SFML_INCLUDES $$QWT_INCLUDES \
    # local
    $$OPENGL_UTILITY_DEP_INCLUDEPATH\
    $$OPENGL_UTILITY_INCLUDES\
    $$QT_UTILITY_MOC \

QT_UTILITY_DEP_LIBS =\
    # thirdparty
    $$SFML_LIBS $$QWT_LIBS\
    # local
    $$OPENGL_UTILITY_DEP_LIBS\
    $$OPENGL_UTILITY_LIB\

QT_UTILITY_PRE_TARGETDEPS =\
    $$BASE_LIB_FILE\
    $$OPENGL_UTILITY_LIB_FILE\

########################################################### QT_UTILITY_APP
QT_UTILITY_APP_DEP_INCLUDEPATH =\
    $$QT_UTILITY_DEP_INCLUDEPATH\
    $$QT_UTILITY_INCLUDES\

QT_UTILITY_APP_DEP_LIBS =\
    $$QT_UTILITY_DEP_LIBS\
    $$QT_UTILITY_LIB\

QT_UTILITY_APP_PRE_TARGETDEPS =\
    $$BASE_LIB_FILE\
    $$OPENGL_UTILITY_LIB_FILE\
    $$QT_UTILITY_LIB_FILE\

########################################################### QT_UTILITY_QML_APP
QT_UTILITY_QML_APP_DEP_INCLUDEPATH =\
    $$QT_UTILITY_DEP_INCLUDEPATH\
    $$QT_UTILITY_INCLUDES\

QT_UTILITY_QML_APP_DEP_LIBS =\
    $$QT_UTILITY_DEP_LIBS\
    $$QT_UTILITY_LIB\

QT_UTILITY_QML_APP_PRE_TARGETDEPS =\
    $$BASE_LIB_FILE\
    $$OPENGL_UTILITY_LIB_FILE\
    $$QT_UTILITY_LIB_FILE\

########################################################### GUARDIAN
GUARDIAN_DEP_INCLUDEPATH =\
    $$QT_UTILITY_DEP_INCLUDEPATH\
    $$QT_UTILITY_INCLUDES\

GUARDIAN_DEP_LIBS =\
    $$QT_UTILITY_DEP_LIBS\
    $$QT_UTILITY_LIB\

GUARDIAN_PRE_TARGETDEPS =\
    $$BASE_LIB_FILE\
    $$OPENGL_UTILITY_LIB_FILE\
    $$QT_UTILITY_LIB_FILE\

########################################################### NODES
NODES_DEP_INCLUDEPATH =\
    nodes\
    nodes/internal\
    $$BASE_DEP_INCLUDEPATH\
    $$BASE_INCLUDES\

NODES_DEP_LIBS =\
    $$BASE_DEP_LIBS\
    $$BASE_LIB\

NODES_PRE_TARGETDEPS =\
    $$BASE_LIB_FILE\

########################################################### DEMOS
DEMOS_DEP_INCLUDEPATH =\
    # thirdparty
    $$BIOPAC_INCLUDES\
    # local
    $$IMGUI_OPENGL_ENGINE_DEP_INCLUDEPATH\
    $$IMGUI_OPENGL_ENGINE_INCLUDES\

DEMOS_DEP_LIBS =\
    # thirdparty
    $$BIOPAC_LIBS\
    # local
    $$IMGUI_OPENGL_ENGINE_DEP_LIBS\
    $$IMGUI_OPENGL_ENGINE_LIB\

DEMOS_PRE_TARGETDEPS =\
    $$BASE_LIB_FILE\
    $$OPENGL_UTILITY_LIB_FILE\
    $$IMGUI_OPENGL_ENGINE_LIB_FILE\

########################################################### DC-GRABBER
DC_GRABBER_DEP_INCLUDEPATH =\
    $$IMGUI_OPENGL_ENGINE_DEP_INCLUDEPATH\
    $$IMGUI_OPENGL_ENGINE_INCLUDES\

DC_GRABBER_DEP_LIBS =\
    $$IMGUI_OPENGL_ENGINE_DEP_LIBS\
    $$IMGUI_OPENGL_ENGINE_LIB\

DC_GRABBER_PRE_TARGETDEPS =\
    $$BASE_LIB_FILE\
    $$OPENGL_UTILITY_LIB_FILE\
    $$IMGUI_OPENGL_ENGINE_LIB_FILE\

########################################################### DC-MANAGER
DC_MANAGER_DEP_INCLUDEPATH =\
    $$IMGUI_OPENGL_ENGINE_DEP_INCLUDEPATH\
    $$IMGUI_OPENGL_ENGINE_INCLUDES\

DC_MANAGER_DEP_LIBS =\
    $$IMGUI_OPENGL_ENGINE_DEP_LIBS\
    $$IMGUI_OPENGL_ENGINE_LIB\

DC_MANAGER_PRE_TARGETDEPS =\
    $$BASE_LIB_FILE\
    $$OPENGL_UTILITY_LIB_FILE\
    $$IMGUI_OPENGL_ENGINE_LIB_FILE\

########################################################### DC-PLAYER
DC_PLAYER_DEP_INCLUDEPATH =\
    $$IMGUI_OPENGL_ENGINE_DEP_INCLUDEPATH\
    $$IMGUI_OPENGL_ENGINE_INCLUDES\

DC_PLAYER_DEP_LIBS =\
    $$IMGUI_OPENGL_ENGINE_DEP_LIBS\
    $$IMGUI_OPENGL_ENGINE_LIB\

DC_PLAYER_PRE_TARGETDEPS =\
    $$BASE_LIB_FILE\
    $$OPENGL_UTILITY_LIB_FILE\
    $$IMGUI_OPENGL_ENGINE_LIB_FILE\

########################################################### DC-MONITORING
DC_MONITORING_DEP_INCLUDEPATH =\
    # thirdparty
    $$FFMEPG_INCLUDES \
    # local
    "avcpp"\
    $$IMGUI_OPENGL_ENGINE_DEP_INCLUDEPATH\
    $$IMGUI_OPENGL_ENGINE_INCLUDES\


DC_MONITORING_DEP_LIBS =\
    # thirdparty
    $$FFMEPG_LIBS \
    # local
    $$IMGUI_OPENGL_ENGINE_DEP_LIBS\
    $$IMGUI_OPENGL_ENGINE_LIB\

DC_MONITORING_PRE_TARGETDEPS =\
    $$BASE_LIB_FILE\
    $$OPENGL_UTILITY_LIB_FILE\
    $$IMGUI_OPENGL_ENGINE_LIB_FILE\

########################################################### REALSTREAM-EDITOR
REALSTREAM_EDITOR_DEP_INCLUDEPATH =\
    $$QT_UTILITY_DEP_INCLUDEPATH\
    $$QT_UTILITY_INCLUDES\

REALSTREAM_EDITOR_DEP_LIBS =\
    $$QT_UTILITY_DEP_LIBS\
    $$QT_UTILITY_LIB\

REALSTREAM_EDITOR_PRE_TARGETDEPS =\
    $$BASE_LIB_FILE\
    $$OPENGL_UTILITY_LIB_FILE\
    $$QT_UTILITY_LIB_FILE\


########################################################### REALSTREAM-EXPORT
REALSTREAM_EXPORT_DEP_INCLUDEPATH =\
    $$QT_UTILITY_DEP_INCLUDEPATH\
    $$QT_UTILITY_INCLUDES\

REALSTREAM_EXPORT_DEP_LIBS =\
    $$QT_UTILITY_DEP_LIBS\
    $$QT_UTILITY_LIB\

REALSTREAM_EXPORT_PRE_TARGETDEPS =\
    $$BASE_LIB_FILE\
    $$OPENGL_UTILITY_LIB_FILE\
    $$QT_UTILITY_LIB_FILE\

########################################################### SCANER-COMPONENT [LEGACY]
SCANER_COMPONENT_DEP_INCLUDEPATH =\
    $$BASE_DEP_INCLUDEPATH\
    $$BASE_INCLUDES\

SCANER_COMPONENT_DEP_LIBS =\
    $$BASE_DEP_LIBS\
    $$BASE_LIB\

SCANER_COMPONENT_PRE_TARGETDEPS =\
    $$BASE_LIB_FILE\

########################################################### SCANER-GRABBER [LEGACY]
SCANER_GRABBER_DEP_INCLUDEPATH =\
    $$QT_UTILITY_UI \
    $$QT_UTILITY_DEP_INCLUDEPATH\
    $$QT_UTILITY_INCLUDES\

SCANER_GRABBER_APP_DEP_LIBS =\
    $$QT_UTILITY_DEP_LIBS\
    $$QT_UTILITY_LIB\

SCANER_GRABBER_APP_PRE_TARGETDEPS =\
    $$BASE_LIB_FILE\
    $$OPENGL_UTILITY_LIB_FILE\
    $$QT_UTILITY_LIB_FILE\

########################################################### SCANER-MANAGER [LEGACY]
SCANER_MANAGER_DEP_INCLUDEPATH =\
    $$QT_UTILITY_UI \
    $$QT_UTILITY_DEP_INCLUDEPATH\
    $$QT_UTILITY_INCLUDES\

SCANER_MANAGER_APP_DEP_LIBS =\
    $$QT_UTILITY_DEP_LIBS\
    $$QT_UTILITY_LIB\

SCANER_MANAGER_APP_PRE_TARGETDEPS =\
    $$BASE_LIB_FILE\
    $$OPENGL_UTILITY_LIB_FILE\
    $$QT_UTILITY_LIB_FILE\


########################################################### FIND CURRENT PROJECT FROM LIST
PROJECT_FOUND="NULL"
for(project, TOOLSET_CPP_PROJECTS):{
    debug_project = $$project"d"
    TLOW = $$lower($$TARGET)

    equals(TLOW, $$project){
        PROJECT_FOUND = $$project
    }
    equals(TLOW, $$debug_project){
        PROJECT_FOUND = $$project
    }
}

########################################################### GENERATE PROJECTS VARIABLES

!equals(PROJECT_FOUND, "NULL"){
    UPT = $$upper($$PROJECT_FOUND)
    UPT = $$replace(UPT, "-", "_")

    INCLUDEPATH     += $$eval($$UPT"_DEP_INCLUDEPATH")
    LIBS            += $$eval($$UPT"_DEP_LIBS")
    PRE_TARGETDEPS  += $$eval($$UPT"_PRE_TARGETDEPS")
}



