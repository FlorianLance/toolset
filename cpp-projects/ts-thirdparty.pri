
# /*******************************************************************************
# ** Toolset                                                                    **
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

########################################################## DIRECTORIES
############################################################ HEADERS ONLY
ANKERL_DIR                           = $$TOOLSET_CPP_THIRDPARTY_DIR"/ankerl"
BINPACK2D_DIR                        = $$TOOLSET_CPP_THIRDPARTY_DIR"/BinPack2D"
CATCH_DIR                            = $$TOOLSET_CPP_THIRDPARTY_DIR"/catch"
EIGEN_DIR                            = $$TOOLSET_CPP_THIRDPARTY_DIR"/eigen-3.4-rc1"
EIGEN_MASTER_DIR                     = $$TOOLSET_CPP_THIRDPARTY_DIR"/eigen-master"
JSON_DIR                             = $$TOOLSET_CPP_THIRDPARTY_DIR"/json"
SIGSLOT_DIR                          = $$TOOLSET_CPP_THIRDPARTY_DIR"/sigslot"
STB_DIR                              = $$TOOLSET_CPP_THIRDPARTY_DIR"/stb"
TASKFLOW_DIR                         = $$TOOLSET_CPP_THIRDPARTY_DIR"/taskflow"
VERSION2_DIR                         = $$TOOLSET_CPP_THIRDPARTY_DIR"/version2-2.02.01"
############################################################ HEADERS + LIBS
ASSIMP_DIR                           = $$TOOLSET_CPP_THIRDPARTY_DIR"/assimp"
GLM_DIR                              = $$TOOLSET_CPP_THIRDPARTY_DIR"/glm"
FASTPFOR_DIR                         = $$TOOLSET_CPP_THIRDPARTY_DIR"/FastPFor"
TURBOJPG_DIR                         = $$TOOLSET_CPP_THIRDPARTY_DIR"/libjpeg-turbo-2.0.3"
KINECT4_DIR                          = $$TOOLSET_CPP_THIRDPARTY_DIR"/kinect4"

BIOPAC_DIR                           = $$TOOLSET_CPP_THIRDPARTY_DIR"/biopac"
BOOST_DIR                            = $$TOOLSET_CPP_THIRDPARTY_DIR"/boost_1_68_0"
GLEW_DIR                             = $$TOOLSET_CPP_THIRDPARTY_DIR"/glew-2.1.0"
GLFW_DIR                             = $$TOOLSET_CPP_THIRDPARTY_DIR"/glfw-3.3.2"
KINECT2_DIR                          = $$TOOLSET_CPP_THIRDPARTY_DIR"/kinect2"
LIBPNG_DIR                           = $$TOOLSET_CPP_THIRDPARTY_DIR"/libpng"
LIBSOUNDIO_DIR                       = $$TOOLSET_CPP_THIRDPARTY_DIR"/libsoundio"
LIBUSB_DIR                           = $$TOOLSET_CPP_THIRDPARTY_DIR"/libusb"
OPEN3D_DIR                           = $$TOOLSET_CPP_THIRDPARTY_DIR"/Open3D-0.18"
OPENCV_DIR                           = $$TOOLSET_CPP_THIRDPARTY_DIR"/opencv"
QWT_DIR                              = $$TOOLSET_CPP_THIRDPARTY_DIR"/qwt-6.3.0"
SFML_DIR                             = $$TOOLSET_CPP_THIRDPARTY_DIR"/SFML"
FFMEPG_DIR                           = $$TOOLSET_CPP_THIRDPARTY_DIR"/ffmpeg"
ORBBEC_DIR                           = $$TOOLSET_CPP_THIRDPARTY_DIR"/orbbec"
JKQTPLOTTER_DIR                      = $$TOOLSET_CPP_THIRDPARTY_DIR"/jkqtplotter"
PCL_DIR                              = "E:/_librairies/PCL 1.14.1"

########################################################## INCLUDES (HEADERS ONLY)

ANKERL_INCLUDES = \
    $$ANKERL_DIR"\include"

BINPACK2D_INCLUDES = \
    $$BINPACK2D_DIR"\include"

CATCH_INCLUDES = \
    $$CATCH_DIR"\include"

EIGEN_INCLUDES = \
    $$EIGEN_DIR \

EIGEN_MASTER_INCLUDES = \
    $$EIGEN_MASTER_DIR \

JSON_INCLUDES = \
    $$JSON_DIR"\include"

SIGSLOT_INCLUDES = \
    $$SIGSLOT_DIR"\include"

STB_INCLUDES = \
    $$STB_DIR"\include"

TASKFLOW_INCLUDES = \
    $$TASKFLOW_DIR"\include"

VERSION2_INCLUDES = \
    $$VERSION2_DIR"\include"


########################################################## INCLUDES (OTHERS)


JKQTPLOTTER_INCLUDES = \
    $$JKQTPLOTTER_DIR"/include"\

PCL_INCLUDES = \
    $$PCL_DIR"/include/pcl-1.14"\
    # $$PCL_DIR"/3rdParty/VTK/include/vtk-9.3"\
    # $$PCL_DIR"/3rdParty/Boost/include/boost-1_84"\



# VERSION2_INCLUDES = \
#     $$VERSION2_DIR\

# TURBOPFOR_INCLUDES = \
#     $$TURBOPFOR_DIR"/include" \


EIGEN_MASTER_INCLUDES = \
    $$EIGEN_MASTER_DIR \

OPEN3D_INCLUDES =\
    $$OPEN3D_DIR"/src" \
    $$OPEN3D_DIR"/3rdparty/fmt/include" \
    $$OPEN3D_DIR"/3rdparty/jsoncpp/include" \
    $$OPEN3D_DIR"/3rdparty/rply/include" \

LIBUSB_INCLUDES = \
    $$LIBUSB_DIR"/includes" \

LIBSOUNDIO_INCLUDES = \
    $$LIBSOUNDIO_DIR"/includes" \

OPENCV_INCLUDES = \    
    $$OPENCV_DIR"/include" \

BOOST_INCLUDES =\
    $$BOOST_DIR\

ASSIMP_INCLUDES = \
    $$ASSIMP_DIR"/include" \

KINECT2_INCLUDES = \
        $$KINECT2_DIR"/inc"

KINECT4_INCLUDES = \
    $$KINECT4_DIR"/include"

GLM_INCLUDES = \
    $$GLM_DIR"/include" \

BIOPAC_INCLUDES = \
    $$BIOPAC_DIR"/include" \

GLFW_INCLUDES =\
    $$GLFW_DIR"/include"\

GLEW_INCLUDES =\
    $$GLEW_DIR"/include"\

SFML_INCLUDES =\
    $$SFML_DIR"/include"\

TURBOJPG_INCLUDES =\
    $$TURBOJPG_DIR"/"

FASTPFOR_INCLUDES =\
    $$FASTPFOR_DIR"/"

QWT_INCLUDES = \
    $$QWT_DIR"/src" \

LIBPNG_INCLUDES = \
    $$LIBPNG_DIR"/include" \

FFMEPG_INCLUDES = \
    $$FFMEPG_DIR"/include" \


ORBBEC_INCLUDES = \
    $$ORBBEC_DIR"/include" \




equals(COMPILER, "vs"){

########################################################## WINDOWS LIBS
    WINDOWS_LIBS = \
        -luser32\
        -lgdi32\
        -lshell32\
        -lws2_32\

    GL_LIBS = \
        -lopengl32\

    OS_LIBS = \
        $$WINDOWS_LIBS\


########################################################## LIBS

    equals(CFG, "release"){
        PCL_LIBS =\
            -L$$PCL_DIR"/lib" \
            -lpcl_common \
            -lpcl_io\
    }

    # BOOST
    equals(CFG, "debug"){

        BOOST_SYS_LIBS =\
            -L$$BOOST_DIR"/lib64-msvc-14.1" \
            -llibboost_system-vc141-mt-gd-x64-1_68 \

        BOOST_OTHERS_LIBS =\
            -L$$BOOST_DIR"/lib64-msvc-14.1" \
            -llibboost_date_time-vc141-mt-gd-x64-1_68 \
            -llibboost_chrono-vc141-mt-gd-x64-1_68 \
            -llibboost_python37-vc141-mt-gd-x64-1_68 \
            -llibboost_filesystem-vc141-mt-gd-x64-1_68 \
            -llibboost_iostreams-vc141-mt-gd-x64-1_68 \
    }
    equals(CFG, "release"){

        BOOST_SYS_LIBS =\
            -L$$BOOST_DIR"/lib64-msvc-14.1" \
            -llibboost_system-vc141-mt-x64-1_68 \

        BOOST_OTHERS_LIBS =\
            -L$$BOOST_DIR"/lib64-msvc-14.1" \
            -llibboost_date_time-vc141-mt-x64-1_68 \
            -llibboost_chrono-vc141-mt-x64-1_68 \
            -llibboost_python37-vc141-mt-x64-1_68 \
            -llibboost_filesystem-vc141-mt-x64-1_68 \
            -llibboost_iostreams-vc141-mt-x64-1_68 \
    }

    # ASSIMP
    equals(CFG, "debug"){
        ASSIMP_LIBS = \
            -L$$ASSIMP_DIR"/lib/x64" \
            -lassimp-vc143-mtd \
    }
    equals(CFG, "release"){
        ASSIMP_LIBS = \
            -L$$ASSIMP_DIR"/lib/x64" \
            -lassimp-vc143-mt \
    }

    # FASTPFOR
    equals(CFG, "debug"){
        FASTPFOR_LIBS =\
            -L$$FASTPFOR_DIR"/msvc/x64/Debug" \
            -lFastPFor \
    }
    equals(CFG, "release"){
        FASTPFOR_LIBS =\
            -L$$FASTPFOR_DIR"/msvc/x64/Release" \
            -lFastPFor \
    }

    # OPENCV
    # equals(CFG, "debug"){
    #     OPENCV_LIBS = \
    #         -L$$OPENCV_DIR"/x64/vc16/lib" \
    #         -lopencv_world490d \
    # }
    # equals(CFG, "release"){
    #     OPENCV_LIBS = \
    #         -L$$OPENCV_DIR"/x64/vc16/lib" \
    #         -lopencv_world490 \
    # }

    equals(CFG, "debug"){
        OPENCV_LIBS = \
            -L$$OPENCV_DIR"/x64/vc17/lib" \
            -lopencv_core4100d \
            -lopencv_imgproc4100d \
            -lopencv_video4100d \
            -lopencv_imgcodecs4100d \
            -lopencv_videoio4100d \
    }
    equals(CFG, "release"){
        OPENCV_LIBS = \
            -L$$OPENCV_DIR"/x64/vc17/lib" \
            -lopencv_core4100 \
            -lopencv_imgproc4100 \
            -lopencv_video4100 \
            -lopencv_imgcodecs4100 \
            -lopencv_videoio4100 \
    }

opencv_core480

    # OPEN3D
    equals(CFG, "debug"){
        OPEN3D_LIBS = \
            -L$$OPEN3D_DIR"/lib/Debug" \
            -lOpen3D \
    }
    equals(CFG, "release"){
        OPEN3D_LIBS = \
            -L$$OPEN3D_DIR"/lib/Release" \
            -lOpen3D \
    }

# OPEN3D_LIBS =\
#     -L$$OPEN3D_DIR"/build/lib/Release" \
#     -lOpen3D \
#     -lqhullcpp \
#     -lqhullstatic_r \
#     -L$$OPEN3D_DIR"/3rdparty/jsoncpp/lib" \
#     -ljsoncpp \
#     -L$$OPEN3D_DIR"/3rdparty/rply/lib" \
#     -lrply \



    # GLFW_LIBS =\
    #     -L$$GLFW_DIR"/lib/Debug" \
    #     -lglfw3dll \

    # GLM_LIBS = \
    #     -L$$GLM_DIR"/lib/Debug" \
    #     -lglm_static \


    TURBOPFOR_LIBS =\
        -L$$TURBOPFOR_DIR"/lib" \
        -llibic \



    KINECT2_LIBS =\
        -L$$KINECT2_DIR"/Lib/x64" \
        -lKinect20 \

    KINECT4_LIBS =\
        -L$$KINECT4_DIR"/lib" \
        -lk4a \
        -lk4arecord \
        -lk4abt \

    ORBBEC_K4A_LIBS =\
        -L$$ORBBEC_K4A_DIR"/lib" \
        -lk4a \
        -lk4arecord \

    GLM_LIBS = \
        -L$$GLM_DIR"/lib/Release" \
        -lglm_static \

    BIOPAC_LIBS = \
        -L$$BIOPAC_DIR"/lib/x64" \
        -lmpdev \

    GLFW_LIBS =\
        -L$$GLFW_DIR"/lib/Release" \
        -lglfw3dll \

    GLEW_LIBS =\
        -L$$GLEW_DIR"/lib/Release/x64" \
        -lglew32 \

    SFML_LIBS =\
        -L$$SFML_DIR"/lib" \
        -lsfml-graphics \
        -lsfml-system \
        -lsfml-window \

    TURBOJPG_LIBS =\
        -L$$TURBOJPG_DIR"/build/Release" \
        -lturbojpeg \
        -ljpeg \



    QWT_LIBS =\
        -L$$QWT_DIR"/lib/release" \
        -lqwt\

    LIBSOUNDIO_LIBS =\
        -L$$LIBSOUNDIO_DIR"/lib/x64" \
        -llibsoundio\

    LIBUSB_LIBS =\
        -L$$LIBUSB_DIR"/lib" \
        -llibusb-1.0\

    LIBPNG_LIBS =\
        -L$$LIBPNG_DIR"/lib" \
        -llibpng16_static\



    FFMEPG_LIBS =\
        -L$$FFMEPG_DIR"/lib" \
        -lavcodec \
        -lavdevice \
        -lavfilter \
        -lavformat \
        -lavutil \
        -lswresample \
        -lswscale \
        -lpostproc \

    ORBBEC_LIBS =\
        -L$$ORBBEC_DIR"/lib" \
        -lOrbbecSDK \

    # JQTPLOTTER
    equals(CFG, "debug"){
        # ..
    }
    equals(CFG, "release"){
        JKQTPLOTTER_LIBS =\
            -L$$JKQTPLOTTER_DIR"/shared_lib/release" \
            -ljkqtplotterlib \
            -ljkqtpcommonlib \
            -ljkqtmathtextlib \

    }
}

