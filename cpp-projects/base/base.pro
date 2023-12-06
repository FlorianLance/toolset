
# /*******************************************************************************
# ** Toolset-base                                                               **
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

####################################### TARGET
equals(CFG, "debug"){
    TARGET = based
}
equals(CFG, "release"){
    TARGET = base
}

####################################### TEMPLATE
TEMPLATE = lib
CONFIG += staticlib
CONFIG -= console

####################################### BUILD FILES
OBJECTS_DIR = $$BASE_OBJ
DESTDIR     = $$BASE_DEST

####################################### CONFIG
CONFIG -= qt

####################################### INCLUDES
INCLUDEPATH += \
    # thirdparty
    $$ASSIMP_INCLUDES \
    $$TURBOJPG_INCLUDES \
    $$FASTPFOR_INCLUDES \
    $$OPENCV_INCLUDES \
    $$KINECT2_INCLUDES \
    $$KINECT4_INCLUDES \
    $$BOOST_INCLUDES \
    $$LIBSOUNDIO_INCLUDES \
    $$LIBUSB_INCLUDES \
    $$EIGEN_INCLUDES \
    $$OPEN3D_INCLUDES \
    $$TURBOPFOR_INCLUDES \
    $$ORBBEC_INCLUDES \

####################################### LIBRAIRIES
LIBS += \
    # thirdparty
    $$ASSIMP_LIBS \
    $$TURBOJPG_LIBS \
    $$FASTPFOR_LIBS \
    $$OPENCV_LIBS \
    $$KINECT2_LIBS \
    $$KINECT4_LIBS \
    $$BOOST_LIBS \
    $$LIBSOUNDIO_LIBS \
    $$LIBUSB_LIBS \
    $$EIGEN_LIBS\
    $$OPEN3D_LIBS \
    $$TURBOPFOR_LIBS \
    $$WINDOWS_LIBS \
    $$ORBBEC_LIBS \

####################################### PROJECT FILES

HEADERS += \
    # files
    files/binary_settings.hpp \
    files/cloud_io.hpp \
    files/assimp_loader.hpp \
    files/files_enum.hpp \
    files/settings.hpp \
    files/text_settings.hpp \
    # geometry 
    geometry/cloud.hpp \
    geometry/geometry.hpp \
    geometry/transform.hpp \
    geometry/aabb3.hpp \
    geometry/circle.hpp \
    geometry/obb3.hpp \
    geometry/plane3.hpp \
    geometry/line2.hpp \
    geometry/line3.hpp \
    geometry/rectangle.hpp \
    geometry/sphere.hpp \
    geometry/ray3.hpp \
    geometry/triangle3.hpp \
    geometry/geometry2.hpp \
    geometry/geometry3.hpp \
    geometry/interval.hpp \
    geometry/matrix.hpp \
    geometry/matrix2.hpp \
    geometry/matrix3.hpp \
    geometry/matrix4.hpp \
    geometry/mesh.hpp \
    geometry/point.hpp \
    geometry/point2.hpp \
    geometry/point3.hpp \
    geometry/point4.hpp \
    geometry/raycast.hpp \
    geometry/dummy.hpp \
    geometry/aabb2.hpp \
    geometry/octree.hpp \
    geometry/maching_cube.hpp \
    geometry/vertices.hpp \
    geometry/voxel.hpp \
    geometry/quaternion.hpp \
    geometry/voxel_grid.hpp \
    # graphics
    graphics/color.hpp \
    graphics/colors.hpp \
    graphics/screen.hpp \
    graphics/light.hpp \
    graphics/material.hpp \
    graphics/texture.hpp \
    graphics/model.hpp \
    graphics/camera.hpp \
    # input
    input/joypad.hpp \
    input/mouse.hpp \
    input/keyboard.hpp \    
    # network
    network/udp_server_network_settings.hpp \
    network/udp_client_network_settings.hpp \
    network/network_enums.hpp \
    network/network_types.hpp \
    network/tcp_reader.hpp \
    network/tcp_sender.hpp \
    network/tcp_server.hpp \
    network/udp_header.hpp \
    network/udp_reader.hpp \
    network/udp_sender.hpp \
    # camera
    camera/dc_camera_data.hpp \
    camera/dc_compressed_frame.hpp \
    camera/dc_frame_compressor.hpp \
    camera/dc_frame_uncompressor.hpp \
    camera/dc_grabber_data_processing.hpp \
    camera/dc_device_manager.hpp \
    camera/dc_server_data.hpp \
    camera/dc_volumetric_video.hpp \
    camera/dc_recorder.hpp \
    camera/dc_player.hpp \
    camera/dc_device.hpp \
    camera/dc_enums.hpp \
    camera/dc_frame.hpp \
    camera/dc_packed_data.hpp \
    camera/dc_types.hpp \
    camera/dc_player_settings.hpp \
    camera/dc_calibrator.hpp \
    ## settings
    camera/settings/dc_device_settings.hpp \
    camera/settings/dc_data_settings.hpp \
    camera/settings/dc_recorder_settings.hpp \
    camera/settings/dc_color_settings.hpp \
    camera/settings/dc_delay_settings.hpp \
    camera/settings/dc_actions_settings.hpp \
    camera/settings/dc_display_settings.hpp \
    camera/settings/dc_calibrator_settings.hpp \
    camera/settings/dc_config_settings.hpp \
    camera/settings/dc_filters_settings.hpp \
    camera/settings/dc_model_settings.hpp \
    ## network
    camera/network/dc_client_connection.hpp \
    camera/network/dc_server_connection.hpp \
    camera/network/dc_server_network.hpp \
    camera/network/dc_udp_reader.hpp \
    camera/network/dc_udp_sender.hpp \
    ## orbbec
    camera/orbbec/orbbec_device.hpp \
    ## kinect2
    camera/kinect2/k2_config_files.hpp \
    camera/kinect2/k2_device.hpp \
    camera/kinect2/k2_manager.hpp \
    camera/kinect2/k2_network.hpp \
    camera/kinect2/k2_types.hpp \
    ## kinect4
    camera/kinect4/k4_device.hpp \
    # exvr
    exvr/ex_element.hpp \
    exvr/ex_experiment.hpp \
    exvr/ex_resource.hpp \
    exvr/ex_component.hpp \
    # utility    
    utility/algorithm.hpp \
    utility/array.hpp \
    utility/benchmark.hpp \
    utility/bit_mask.hpp \
    utility/buffer_vector.hpp \
    utility/cmd_args.hpp \
    utility/constants.hpp \
    utility/decimal.hpp \
    utility/export.hpp \
    utility/format.hpp \
    utility/id_alias_map.hpp \
    utility/io_data.hpp \
    utility/io_fstream.hpp \
    utility/math.hpp \
    utility/paths.hpp \
    utility/stop_watch.hpp \
    utility/string.hpp \
    utility/string_geo.hpp \
    utility/string_unordered_map.hpp \
    utility/thread.hpp \
    utility/types.hpp \
    utility/unordered_map.hpp \
    utility/unordered_set.hpp \
    utility/vector.hpp \
    utility/logger.hpp \
    utility/tuple_array.hpp \
    utility/utility.hpp \
    utility/view.hpp \
    utility/time.hpp \
    utility/io_file.hpp \
    # algorithms
    algorithms/marching_cube.hpp \
    # data
    data/FastDifferentialCoding/fastdelta.h \
    data/integers_encoder.hpp \
    # tests
    tests/marching_cube_test.hpp \
    # thirdparty
    ## rectpack2D
    thirdparty/rectpack2D/best_bin_finder.h \
    thirdparty/rectpack2D/empty_space_allocators.h \
    thirdparty/rectpack2D/empty_spaces.h \
    thirdparty/rectpack2D/finders_interface.h \
    thirdparty/rectpack2D/insert_and_split.h \
    thirdparty/rectpack2D/rect_structs.h \
    ## stb
    thirdparty/stb/stb_image.h \
    thirdparty/stb/stb_image_resize.h \
    thirdparty/stb/stb_image_write.h \
    ## libyuv
    thirdparty/libyuv/libyuv.h \
    thirdparty/libyuv/libyuv/basic_types.h \
    thirdparty/libyuv/libyuv/compare.h \
    thirdparty/libyuv/libyuv/compare_row.h \
    thirdparty/libyuv/libyuv/convert.h \
    thirdparty/libyuv/libyuv/convert_argb.h \
    thirdparty/libyuv/libyuv/convert_from.h \
    thirdparty/libyuv/libyuv/convert_from_argb.h \
    thirdparty/libyuv/libyuv/cpu_id.h \
    thirdparty/libyuv/libyuv/loongson_intrinsics.h \
    thirdparty/libyuv/libyuv/macros_msa.h \
    thirdparty/libyuv/libyuv/mjpeg_decoder.h \
    thirdparty/libyuv/libyuv/planar_functions.h \
    thirdparty/libyuv/libyuv/rotate.h \
    thirdparty/libyuv/libyuv/rotate_argb.h \
    thirdparty/libyuv/libyuv/rotate_row.h \
    thirdparty/libyuv/libyuv/row.h \
    thirdparty/libyuv/libyuv/scale.h \
    thirdparty/libyuv/libyuv/scale_argb.h \
    thirdparty/libyuv/libyuv/scale_rgb.h \
    thirdparty/libyuv/libyuv/scale_row.h \
    thirdparty/libyuv/libyuv/scale_uv.h \
    thirdparty/libyuv/libyuv/version.h \
    thirdparty/libyuv/libyuv/video_common.h \
    ## k4a
    thirdparty/k4a/k4a_body_tracking_helpers.hpp \
    thirdparty/k4a/k4aaudiochanneldatagraph.h \
    thirdparty/k4a/k4aaudiomanager.h \
    thirdparty/k4a/k4aaudiowindow.h \
    thirdparty/k4a/k4adevicecorrelator.h \
    thirdparty/k4a/k4amicrophone.h \
    thirdparty/k4a/k4amicrophonelistener.h \
    thirdparty/k4a/k4asoundio_util.h \
    thirdparty/k4a/k4astaticimageproperties.h \
    ## nanobench
    thirdparty/nanobench/nanobench.h \
    ## binpack2D
    thirdparty/BinPack2D/binpack2d.hpp \
    ## ankerl
    thirdparty/ankerl/unordered_dense.h \
    ## audiofile
    thirdparty/audiofile/AudioFile.h \
    ## catch
    thirdparty/catch/catch.hpp \
    ## sigslot
    thirdparty/sigslot/signal.hpp \


SOURCES += \
    # files
    files/assimp_loader.cpp \
    files/cloud_io.cpp \
    files/settings.cpp \
    # graphics     
    graphics/color.cpp \
    graphics/model.cpp \
    graphics/texture.cpp \
    # geometry
    geometry/cloud.cpp \
    geometry/mesh.cpp \
    geometry/vertices.cpp \
    geometry/voxel_grid.cpp \
    # network
    network/udp_server_network_settings.cpp \
    network/udp_client_network_settings.cpp \
    network/network_types.cpp \
    # utility    
    utility/benchmark.cpp \
    utility/cmd_args.cpp \
    utility/decimal.cpp \
    utility/io_file.cpp \
    utility/logger.cpp \
    utility/paths.cpp \
    utility/string.cpp \
    # network
    network/tcp_reader.cpp \
    network/tcp_sender.cpp \
    network/tcp_server.cpp \
    network/udp_header.cpp \
    network/udp_reader.cpp \
    network/udp_sender.cpp \
    # camera
    camera/dc_compressed_frame.cpp \
    camera/dc_camera_data.cpp \
    camera/dc_frame_compressor.cpp \
    camera/dc_frame_uncompressor.cpp \
    camera/dc_grabber_data_processing.cpp \
    camera/dc_device_manager.cpp \
    camera/dc_server_data.cpp \
    camera/dc_volumetric_video.cpp \
    camera/dc_recorder.cpp \
    camera/dc_player.cpp \
    camera/dc_frame.cpp \
    camera/dc_packed_data.cpp \
    camera/dc_calibrator.cpp \
    ## network
    camera/network/dc_client_connection.cpp \
    camera/network/dc_server_connection.cpp \
    camera/network/dc_server_network.cpp \
    camera/network/dc_udp_reader.cpp \
    camera/network/dc_udp_sender.cpp \
    ## settings
    camera/settings/dc_filters_settings.cpp \
    camera/settings/dc_model_settings.cpp \
    camera/settings/dc_config_settings.cpp \
    camera/settings/dc_color_settings.cpp \
    camera/settings/dc_actions_settings.cpp \
    camera/settings/dc_display_settings.cpp \
    camera/settings/dc_delay_settings.cpp \
    camera/settings/dc_data_settings.cpp \
    camera/settings/dc_device_settings.cpp \
    ## kinect2
    camera/kinect2/k2_config_files.cpp \
    camera/kinect2/k2_device.cpp \
    camera/kinect2/k2_manager.cpp \
    camera/kinect2/k2_types.cpp \
    ## kinect4
    camera/kinect4/k4_device.cpp \
    ## orbbec
    camera/orbbec/orbbec_device.cpp \
    # exvr
    exvr/ex_component.cpp \
    exvr/ex_element.cpp \
    exvr/ex_experiment.cpp \
    exvr/ex_resource.cpp \
    # data
    data/FastDifferentialCoding/fastdelta.c \
    data/integers_encoder.cpp \
    # thirdparty
    ## nanobench
    thirdparty/nanobench/nanobench.cpp \
    ## stb
    thirdparty/stb/stb_image.cpp \
    thirdparty/stb/stb_image_resize.cpp \
    thirdparty/stb/stb_image_write.cpp \
    ## k4a
    thirdparty/k4a/k4aaudiochanneldatagraph.cpp \
    thirdparty/k4a/k4aaudiomanager.cpp \
    thirdparty/k4a/k4aaudiowindow.cpp \
    thirdparty/k4a/platform/windows/k4adevicecorrelator.cpp \
    thirdparty/k4a/k4amicrophone.cpp \
    thirdparty/k4a/k4amicrophonelistener.cpp \
    ## libyuv
    thirdparty/libyuv/compare.cc \
    thirdparty/libyuv/compare_common.cc \
    thirdparty/libyuv/compare_gcc.cc \
    thirdparty/libyuv/compare_mmi.cc \
    thirdparty/libyuv/compare_msa.cc \
    thirdparty/libyuv/compare_neon.cc \
    thirdparty/libyuv/compare_neon64.cc \
    thirdparty/libyuv/compare_win.cc \
    thirdparty/libyuv/convert.cc \
    thirdparty/libyuv/convert_argb.cc \
    thirdparty/libyuv/convert_from.cc \
    thirdparty/libyuv/convert_from_argb.cc \
    thirdparty/libyuv/convert_jpeg.cc \
    thirdparty/libyuv/convert_to_argb.cc \
    thirdparty/libyuv/convert_to_i420.cc \
    thirdparty/libyuv/cpu_id.cc \
    thirdparty/libyuv/mjpeg_decoder.cc \
    thirdparty/libyuv/mjpeg_validate.cc \
    thirdparty/libyuv/planar_functions.cc \
    thirdparty/libyuv/rotate.cc \
    thirdparty/libyuv/rotate_any.cc \
    thirdparty/libyuv/rotate_argb.cc \
    thirdparty/libyuv/rotate_common.cc \
    thirdparty/libyuv/rotate_gcc.cc \
    thirdparty/libyuv/rotate_lsx.cc \
    thirdparty/libyuv/rotate_mmi.cc \
    thirdparty/libyuv/rotate_msa.cc \
    thirdparty/libyuv/rotate_neon.cc \
    thirdparty/libyuv/rotate_neon64.cc \
    thirdparty/libyuv/rotate_win.cc \
    thirdparty/libyuv/row_any.cc \
    thirdparty/libyuv/row_common.cc \
    thirdparty/libyuv/row_gcc.cc \
    thirdparty/libyuv/row_lasx.cc \
    thirdparty/libyuv/row_lsx.cc \
    thirdparty/libyuv/row_mmi.cc \
    thirdparty/libyuv/row_msa.cc \
    thirdparty/libyuv/row_neon.cc \
    thirdparty/libyuv/row_neon64.cc \
    thirdparty/libyuv/row_win.cc \
    thirdparty/libyuv/scale.cc \
    thirdparty/libyuv/scale_any.cc \
    thirdparty/libyuv/scale_argb.cc \
    thirdparty/libyuv/scale_common.cc \
    thirdparty/libyuv/scale_gcc.cc \
    thirdparty/libyuv/scale_lsx.cc \
    thirdparty/libyuv/scale_mmi.cc \
    thirdparty/libyuv/scale_msa.cc \
    thirdparty/libyuv/scale_neon.cc \
    thirdparty/libyuv/scale_neon64.cc \
    thirdparty/libyuv/scale_rgb.cc \
    thirdparty/libyuv/scale_uv.cc \
    thirdparty/libyuv/scale_win.cc \
    thirdparty/libyuv/video_common.cc \



#DISTFILES += \

