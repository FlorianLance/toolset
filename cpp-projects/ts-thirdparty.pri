
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
ASSIMP_DIR                           = $$TOOLSET_CPP_THIRDPARTY_DIR"/assimp"
BIOPAC_DIR                           = $$TOOLSET_CPP_THIRDPARTY_DIR"/biopac"
BOOST_DIR                            = $$TOOLSET_CPP_THIRDPARTY_DIR"/boost_1_68_0"
EIGEN_DIR                            = $$TOOLSET_CPP_THIRDPARTY_DIR"/eigen-3.4-rc1"
FASTPFOR_DIR                         = $$TOOLSET_CPP_THIRDPARTY_DIR"/FastPFor"
GLEW_DIR                             = $$TOOLSET_CPP_THIRDPARTY_DIR"/glew-2.1.0"
GLFW_DIR                             = $$TOOLSET_CPP_THIRDPARTY_DIR"/glfw-3.3.2"
GLM_DIR                              = $$TOOLSET_CPP_THIRDPARTY_DIR"/glm"
KINECT2_DIR                          = $$TOOLSET_CPP_THIRDPARTY_DIR"/kinect2"
KINECT4_DIR                          = $$TOOLSET_CPP_THIRDPARTY_DIR"/kinect4"
TURBOJPG_DIR                         = $$TOOLSET_CPP_THIRDPARTY_DIR"/libjpeg-turbo-2.0.3"
LIBPNG_DIR                           = $$TOOLSET_CPP_THIRDPARTY_DIR"/libpng"
LIBSOUNDIO_DIR                       = $$TOOLSET_CPP_THIRDPARTY_DIR"/libsoundio"
LIBUSB_DIR                           = $$TOOLSET_CPP_THIRDPARTY_DIR"/libusb"
OPEN3D_DIR                           = $$TOOLSET_CPP_THIRDPARTY_DIR"/Open3D-0.13"
#OPENCV_DIR                           = $$TOOLSET_CPP_THIRDPARTY_DIR"/opencv-3.4.7"
OPENCV_DIR                           = $$TOOLSET_CPP_THIRDPARTY_DIR"/opencv"
QWT_DIR                              = $$TOOLSET_CPP_THIRDPARTY_DIR"/qwt-6.2.0"
SFML_DIR                             = $$TOOLSET_CPP_THIRDPARTY_DIR"/SFML"
TURBOPFOR_DIR                        = $$TOOLSET_CPP_THIRDPARTY_DIR"/TurboPFor-Integer-Compression"
FFMEPG_DIR                           = $$TOOLSET_CPP_THIRDPARTY_DIR"/ffmpeg"
ORBBEC_DIR                           = $$TOOLSET_CPP_THIRDPARTY_DIR"/orbbec"

########################################################## INCLUDES

TURBOPFOR_INCLUDES = \
    $$TURBOPFOR_DIR"/include" \

EIGEN_INCLUDES = \
    $$EIGEN_DIR \

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
        -lopengl32\
        -lgdi32\
        -lshell32\
        -lws2_32\

    OS_LIBS = $$WINDOWS_LIBS\

########################################################## DEBUG LIBS
    # equals(CFG, "debug"){

    # BOOST_LIBS =\
    #     -L$$BOOST_DIR"/lib64-msvc-14.1" \
    #     -llibboost_date_time-vc141-mt-gd-x64-1_68 \
    #     -llibboost_system-vc141-mt-gd-x64-1_68 \
    #     -llibboost_chrono-vc141-mt-gd-x64-1_68 \
    #     -llibboost_python37-vc141-mt-gd-x64-1_68 \
    #     -llibboost_system-vc141-mt-gd-x64-1_68 \
    #     -llibboost_filesystem-vc141-mt-gd-x64-1_68 \
    #     -llibboost_iostreams-vc141-mt-gd-x64-1_68 \

    # GLFW_LIBS =\
    #     -L$$GLFW_DIR"/lib/Debug" \
    #     -lglfw3dll \

    # GLM_LIBS = \
    #     -L$$GLM_DIR"/lib/Debug" \
    #     -lglm_static \
    # }

########################################################## RELEASE LIBS
    # equals(CFG, "release"){

        TURBOPFOR_LIBS =\
            -L$$TURBOPFOR_DIR"/lib" \
            -llibic \

        OPENCV_LIBS = \
            -L$$OPENCV_DIR"/x64/vc16/lib" \
            -lopencv_world480 \

        BOOST_LIBS =\
            -L$$BOOST_DIR"/lib64-msvc-14.1" \
            -llibboost_date_time-vc141-mt-x64-1_68 \
            -llibboost_system-vc141-mt-x64-1_68 \
            -llibboost_chrono-vc141-mt-x64-1_68 \
            -llibboost_python37-vc141-mt-x64-1_68 \
            -llibboost_system-vc141-mt-x64-1_68 \
            -llibboost_filesystem-vc141-mt-x64-1_68 \
            -llibboost_iostreams-vc141-mt-x64-1_68 \

        ASSIMP_LIBS = \
            -L$$ASSIMP_DIR"/lib/x64" \
            -lassimp-vc141-mt \

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

        ### FASTPFOR
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

        OPEN3D_LIBS =\
            -L$$OPEN3D_DIR"/build/lib/Release" \
            -lOpen3D \
            -lqhullcpp \
            -lqhullstatic_r \
            -L$$OPEN3D_DIR"/3rdparty/jsoncpp/lib" \
            -ljsoncpp \
            -L$$OPEN3D_DIR"/3rdparty/rply/lib" \
            -lrply \

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
    # }
}

