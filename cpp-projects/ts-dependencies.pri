
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

########################################################### TS-BASE

TS_BASE_THIRDPARTY_INCLUDES =\
    $$ANKERL_INCLUDES\
    $$CATCH_INCLUDES\
    $$JSON_INCLUDES\
    $$SIGSLOT_INCLUDES\
    $$STB_INCLUDES\
    $$BOOST_INCLUDES\

TS_BASE_THIRDPARTY_LIBS =\
    $$OS_LIBS\
    $$BOOST_SYS_LIBS\

########################################################### TS-BASE-TEST

TS_BASE_TEST_THIRDPARTY_INCLUDES =\
    $$GLM_INCLUDES\

TS_BASE_TEST_THIRDPARTY_LIBS =\
    $$GLM_LIBS\

########################################################### TS-DATA

TS_DATA_THIRDPARTY_INCLUDES =\
    $$TURBOJPG_INCLUDES\
    $$FASTPFOR_INCLUDES\

TS_DATA_THIRDPARTY_LIBS =\
    $$TURBOJPG_LIBS\
    $$FASTPFOR_LIBS\

########################################################### TS-NETWORK

TS_NETWORK_THIRDPARTY_INCLUDES =\

TS_NETWORK_THIRDPARTY_LIBS =\
    #$$BOOST_OTHERS_LIBS\

########################################################### TS-MESH

TS_MESH_THIRDPARTY_INCLUDES =\
    $$ASSIMP_INCLUDES\

TS_MESH_THIRDPARTY_LIBS =\
    $$ASSIMP_LIBS\

########################################################### TS-DEPTH-CAMERA

TS_DEPTH_CAMERA_THIRDPARTY_INCLUDES =\
    $$TASKFLOW_INCLUDES \
    $$KINECT4_INCLUDES\
    $$OPEN3D_INCLUDES\
    $$OPENCV_INCLUDES\
    $$LIBSOUNDIO_INCLUDES \
    $$ORBBEC_INCLUDES\
    $$EIGEN_INCLUDES \

TS_DEPTH_CAMERA_THIRDPARTY_LIBS =\
    $$KINECT4_LIBS\
    $$OPEN3D_LIBS\
    $$OPENCV_LIBS\
    $$LIBSOUNDIO_LIBS \
    $$ORBBEC_LIBS \
    $$EIGEN_LIBS \

########################################################### TS-OPENGL

TS_OPENGL_THIRDPARTY_INCLUDES =\
    $$GLEW_INCLUDES\

TS_OPENGL_THIRDPARTY_LIBS =\
    $$GLEW_LIBS\
    $$GL_LIBS\

########################################################### TS-OPENGL-APP

TS_OPENGL_APP_THIRDPARTY_INCLUDES =\
    $$GLFW_INCLUDES\

TS_OPENGL_APP_THIRDPARTY_LIBS =\
    $$GLFW_LIBS\

########################################################### TS-IMGUI-GL-ENGINE

TS_IMGUI_GL_ENGINE_THIRDPARTY_INCLUDES =\
    $$BINPACK2D_INCLUDES\
    $$SFML_INCLUDES\
    $$TS_IMGUI_GL_ENGINE_INCLUDES"/imgui"\

TS_IMGUI_GL_ENGINE_THIRDPARTY_LIBS =\
    $$SFML_LIBS\

########################################################### TS-DEMOS

############################## THIRDPARTY
TS_DEMOS_THIRDPARTY_INCLUDES =\
    $$GLM_INCLUDES\
    $$BIOPAC_INCLUDES\

TS_DEMOS_THIRDPARTY_LIBS =\
    $$GLM_LIBS\
    $$BIOPAC_LIBS\

########################################################### TS-EXPORT

TS_EXPORT_THIRDPARTY_INCLUDES =\

TS_EXPORT_THIRDPARTY_LIBS =\


########################################################### TS-EXPORT-APP

TS_EXPORT_APP_THIRDPARTY_INCLUDES =\

TS_EXPORT_APP_THIRDPARTY_LIBS =\

########################################################### TS-GLOBAL-APP

TS_GLOBAL_APP_THIRDPARTY_INCLUDES =\
    $$VERSION2_INCLUDES\

TS_GLOBAL_APP_THIRDPARTY_LIBS =\


########################################################### TS-QT

TS_QT_THIRDPARTY_INCLUDES =\
    $$QWT_INCLUDES\

TS_QT_THIRDPARTY_LIBS =\
    $$QWT_LIBS\

########################################################### TS-QT-APP

TS_QT_APP_THIRDPARTY_INCLUDES =\

TS_QT_APP_THIRDPARTY_LIBS =\


########################################################### TS-QT-GL

TS_QT_GL_THIRDPARTY_INCLUDES =\
    $$SFML_INCLUDES\

TS_QT_GL_THIRDPARTY_LIBS =\
    $$SFML_LIBS\

########################################################### TS-QT-NODES

TS_QT_NODES_THIRDPARTY_INCLUDES =\

TS_QT_NODES_THIRDPARTY_LIBS =\

########################################################### DC-GRABBER

DC_GRABBER_THIRDPARTY_INCLUDES =\

DC_GRABBER_THIRDPARTY_LIBS =\

########################################################### DC-MANAGER

DC_MANAGER_THIRDPARTY_INCLUDES =\

DC_MANAGER_THIRDPARTY_LIBS =\

########################################################### DC-MONITORING

DC_MONITORING_THIRDPARTY_INCLUDES =\
    $$OPENCV_INCLUDES\
    $$FFMEPG_INCLUDES \
    $$DC_MONITORING_INCLUDES"/avcpp"\

DC_MONITORING_THIRDPARTY_LIBS =\
    $$OPENCV_LIBS\
    $$FFMEPG_LIBS \

########################################################### GUARDIAN

GUARDIAN_THIRDPARTY_INCLUDES =\

GUARDIAN_THIRDPARTY_LIBS =\
