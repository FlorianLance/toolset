
# /*******************************************************************************
# ** Toolset-ts-depth-camera                                                    **
# ** MIT License                                                                **
# ** Copyright (c) [2025] [Florian Lance]                                       **
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

####################################### REPOSITORY
TOOLSET_REPOSITORY_DIR      = $$PWD"/../.."

####################################### TARGET/TEMPMATE/CONFIG
TARGET = ts-depth-camera
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

TS_DEPTH_CAMERA_DEP_INCLUDEPATH =\
    $$TS_DEPTH_CAMERA_THIRDPARTY_INCLUDES \
    $$TS_NETWORK_THIRDPARTY_INCLUDES \
    $$TS_DATA_THIRDPARTY_INCLUDES \
    $$TS_MESH_THIRDPARTY_INCLUDES \
    $$TS_BASE_THIRDPARTY_INCLUDES \
    $$TS_BASE_INCLUDES\
    $$TS_MESH_INCLUDES\
    $$TS_DATA_INCLUDES\
    $$TS_NETWORK_INCLUDES\

####################################### LIBS

TS_DEPTH_CAMERA_DEP_LIBS =\
    $$TS_DEPTH_CAMERA_THIRDPARTY_LIBS\
    $$TS_BASE_THIRDPARTY_LIBS\
    $$TS_DATA_THIRDPARTY_LIBS\
    $$TS_MESH_THIRDPARTY_LIBS\
    $$TS_NETWORK_THIRDPARTY_LIBS\
    $$TS_BASE_LIB\
    $$TS_NETWORK_LIB\
    $$TS_DATA_LIB\
    $$TS_MESH_LIB\

####################################### DEP

TS_DEPTH_CAMERA_PRE_TARGETDEPS =\
    $$TS_NETWORK_LIB_FILE\
    $$TS_DATA_LIB_FILE\
    $$TS_MESH_LIB_FILE\
    $$TS_BASE_LIB_FILE\


####################################### GENERATE VARIABLES

include(../ts-gen-var.pri)

####################################### PROJECT FILES
HEADERS += \
    depth-camera/dc_device.hpp \
    depth-camera/dc_packed_data.hpp \
    depth-camera/dc_player_settings.hpp \
    depth-camera/dc_calibrator.hpp \
    depth-camera/dc_device_impl.hpp \
    depth-camera/dc_video.hpp \
    depth-camera/dc_video_player.hpp \
    depth-camera/dc_video_recorder.hpp \
    depth-camera/dc_device_array.hpp \
    ## client
    depth-camera/client/dc_client.hpp \
    depth-camera/client/dc_client_processing.hpp \
    depth-camera/client/dc_client_device.hpp \
    depth-camera/client/dc_client_local_device.hpp \
    depth-camera/client/dc_client_remote_device.hpp \
    # frame
    depth-camera/frame/dc_data_frame.hpp \
    depth-camera/frame/dc_data_frame_buffer.hpp \
    depth-camera/frame/dc_data_frame_generator.hpp \
    depth-camera/frame/dc_depth_indices.hpp \
    depth-camera/frame/dc_device_data.hpp \
    depth-camera/frame/dc_frame_generator.hpp \
    depth-camera/frame/dc_frame_processor.hpp \
    # impl
    depth-camera/impl/femto_base_device.hpp \
    depth-camera/impl/femto_mega_ethernet_device_impl.hpp \
    depth-camera/impl/femto_mega_usb_device_impl.hpp \
    depth-camera/impl/gemini_215_device_impl.hpp \
    depth-camera/impl/gemini_base_device.hpp \
    depth-camera/impl/recording_device_impl.hpp \
    depth-camera/impl/k2_config_files.hpp \
    depth-camera/impl/k2_device.hpp \
    depth-camera/impl/k2_manager.hpp \
    depth-camera/impl/k2_network.hpp \
    depth-camera/impl/k2_types.hpp \
    depth-camera/impl/k2_enums.hpp \
    depth-camera/impl/azure_kinect_device_impl.hpp \
    depth-camera/impl/playback_device_impl.hpp \
    depth-camera/impl/azure_base_device.hpp \
    depth-camera/impl/azure_utility.hpp \
    depth-camera/impl/femto_bolt_device_impl.hpp \
    # network
    depth-camera/network/dc_server.hpp \
    # settings
    depth-camera/settings/dc_client_device_settings.hpp \
    depth-camera/settings/dc_server_settings.hpp \
    depth-camera/settings/dc_client_settings.hpp \
    depth-camera/settings/dc_device_connection_settings.hpp \



    ## impl

    # thirdparty/ColorSpace/ColorSpace.h \
    # thirdparty/ColorSpace/Comparison.h \
    # thirdparty/ColorSpace/Conversion.h \
    # thirdparty/ColorSpace/Utils.h \

    # # tests
    # tests/marching_cube_test.hpp \
    # # thirdparty
    # ## rectpack2D
    # thirdparty/rectpack2D/best_bin_finder.h \
    # thirdparty/rectpack2D/empty_space_allocators.h \
    # thirdparty/rectpack2D/empty_spaces.h \
    # thirdparty/rectpack2D/finders_interface.h \
    # thirdparty/rectpack2D/insert_and_split.h \
    # thirdparty/rectpack2D/rect_structs.h \
    # ## stb
    # thirdparty/stb/stb_image.h \
    # thirdparty/stb/stb_image_resize.h \
    # thirdparty/stb/stb_image_write.h \
    # ## libyuv
    # thirdparty/libyuv/libyuv.h \
    # thirdparty/libyuv/libyuv/basic_types.h \
    # thirdparty/libyuv/libyuv/compare.h \
    # thirdparty/libyuv/libyuv/compare_row.h \
    # thirdparty/libyuv/libyuv/convert.h \
    # thirdparty/libyuv/libyuv/convert_argb.h \
    # thirdparty/libyuv/libyuv/convert_from.h \
    # thirdparty/libyuv/libyuv/convert_from_argb.h \
    # thirdparty/libyuv/libyuv/cpu_id.h \
    # thirdparty/libyuv/libyuv/loongson_intrinsics.h \
    # thirdparty/libyuv/libyuv/macros_msa.h \
    # thirdparty/libyuv/libyuv/mjpeg_decoder.h \
    # thirdparty/libyuv/libyuv/planar_functions.h \
    # thirdparty/libyuv/libyuv/rotate.h \
    # thirdparty/libyuv/libyuv/rotate_argb.h \
    # thirdparty/libyuv/libyuv/rotate_row.h \
    # thirdparty/libyuv/libyuv/row.h \
    # thirdparty/libyuv/libyuv/scale.h \
    # thirdparty/libyuv/libyuv/scale_argb.h \
    # thirdparty/libyuv/libyuv/scale_rgb.h \
    # thirdparty/libyuv/libyuv/scale_row.h \
    # thirdparty/libyuv/libyuv/scale_uv.h \
    # thirdparty/libyuv/libyuv/version.h \
    # thirdparty/libyuv/libyuv/video_common.h \
    # ## k4a
    # thirdparty/k4a/k4a_body_tracking_helpers.hpp \
    # thirdparty/k4a/k4aaudiochanneldatagraph.h \
    # thirdparty/k4a/k4aaudiomanager.h \
    # thirdparty/k4a/k4aaudiowindow.h \
    # thirdparty/k4a/k4adevicecorrelator.h \
    # thirdparty/k4a/k4amicrophone.h \
    # thirdparty/k4a/k4amicrophonelistener.h \
    # thirdparty/k4a/k4asoundio_util.h \
    # thirdparty/k4a/k4astaticimageproperties.h \
    # ## nanobench
    # thirdparty/nanobench/nanobench.h \
    # ## binpack2D
    # thirdparty/BinPack2D/binpack2d.hpp \
    # ## ankerl
    # thirdparty/ankerl/unordered_dense.h \
    # ## audiofile
    # thirdparty/audiofile/AudioFile.h \
    # ## catch
    # thirdparty/catch/catch.hpp \
    # ## sigslot
    # thirdparty/sigslot/signal.hpp \
    # ## taskflow
    # thirdparty/taskflow/taskflow.hpp \

SOURCES += \    
    depth-camera/dc_device_array.cpp \
    depth-camera/dc_video_player.cpp \
    depth-camera/dc_video_recorder.cpp \
    depth-camera/dc_packed_data.cpp \
    depth-camera/dc_calibrator.cpp \
    depth-camera/dc_device.cpp \
    depth-camera/dc_device_impl.cpp \
    depth-camera/dc_video.cpp \
    ## client
    depth-camera/client/dc_client.cpp \
    depth-camera/client/dc_client_processing.cpp \
    depth-camera/client/dc_client_local_device.cpp \
    depth-camera/client/dc_client_remote_device.cpp \
    ## frame
    depth-camera/frame/dc_data_frame.cpp \
    depth-camera/frame/dc_data_frame_buffer.cpp \
    depth-camera/frame/dc_data_frame_generator.cpp \
    depth-camera/frame/dc_depth_indices.cpp \
    depth-camera/frame/dc_device_data.cpp \
    depth-camera/frame/dc_frame_generator.cpp \
    depth-camera/frame/dc_frame_processor.cpp \
    ## impl
    depth-camera/impl/k2_config_files.cpp \
    depth-camera/impl/k2_device.cpp \
    depth-camera/impl/k2_manager.cpp \
    depth-camera/impl/k2_types.cpp \
    depth-camera/impl/azure_kinect_device_impl.cpp \
    depth-camera/impl/azure_base_device.cpp \
    depth-camera/impl/femto_bolt_device_impl.cpp \
    depth-camera/impl/recording_device_impl.cpp \
    depth-camera/impl/femto_base_device.cpp \
    depth-camera/impl/femto_mega_ethernet_device_impl.cpp \
    depth-camera/impl/femto_mega_usb_device_impl.cpp \
    depth-camera/impl/gemini_215_device_impl.cpp \
    depth-camera/impl/gemini_base_device.cpp \
    ## network
    depth-camera/network/dc_server.cpp \
    ## settings
    depth-camera/settings/dc_client_device_settings.cpp \
    depth-camera/settings/dc_server_settings.cpp \
    depth-camera/settings/dc_client_settings.cpp \
    depth-camera/settings/dc_device_connection_settings.cpp \

    # thirdparty
    ## nanobench
    # thirdparty/nanobench/nanobench.cpp \
    # ## stb
    # thirdparty/stb/stb_image.cpp \
    # thirdparty/stb/stb_image_resize.cpp \
    # thirdparty/stb/stb_image_write.cpp \
    # ## k4a
    # thirdparty/k4a/k4aaudiochanneldatagraph.cpp \
    # thirdparty/k4a/k4aaudiomanager.cpp \
    # thirdparty/k4a/k4aaudiowindow.cpp \
    # thirdparty/k4a/platform/windows/k4adevicecorrelator.cpp \
    # thirdparty/k4a/k4amicrophone.cpp \
    # thirdparty/k4a/k4amicrophonelistener.cpp \
    # ## libyuv
    # thirdparty/libyuv/compare.cc \
    # thirdparty/libyuv/compare_common.cc \
    # thirdparty/libyuv/compare_gcc.cc \
    # thirdparty/libyuv/compare_mmi.cc \
    # thirdparty/libyuv/compare_msa.cc \
    # thirdparty/libyuv/compare_neon.cc \
    # thirdparty/libyuv/compare_neon64.cc \
    # thirdparty/libyuv/compare_win.cc \
    # thirdparty/libyuv/convert.cc \
    # thirdparty/libyuv/convert_argb.cc \
    # thirdparty/libyuv/convert_from.cc \
    # thirdparty/libyuv/convert_from_argb.cc \
    # thirdparty/libyuv/convert_jpeg.cc \
    # thirdparty/libyuv/convert_to_argb.cc \
    # thirdparty/libyuv/convert_to_i420.cc \
    # thirdparty/libyuv/cpu_id.cc \
    # thirdparty/libyuv/mjpeg_decoder.cc \
    # thirdparty/libyuv/mjpeg_validate.cc \
    # thirdparty/libyuv/planar_functions.cc \
    # thirdparty/libyuv/rotate.cc \
    # thirdparty/libyuv/rotate_any.cc \
    # thirdparty/libyuv/rotate_argb.cc \
    # thirdparty/libyuv/rotate_common.cc \
    # thirdparty/libyuv/rotate_gcc.cc \
    # thirdparty/libyuv/rotate_lsx.cc \
    # thirdparty/libyuv/rotate_mmi.cc \
    # thirdparty/libyuv/rotate_msa.cc \
    # thirdparty/libyuv/rotate_neon.cc \
    # thirdparty/libyuv/rotate_neon64.cc \
    # thirdparty/libyuv/rotate_win.cc \
    # thirdparty/libyuv/row_any.cc \
    # thirdparty/libyuv/row_common.cc \
    # thirdparty/libyuv/row_gcc.cc \
    # thirdparty/libyuv/row_lasx.cc \
    # thirdparty/libyuv/row_lsx.cc \
    # thirdparty/libyuv/row_mmi.cc \
    # thirdparty/libyuv/row_msa.cc \
    # thirdparty/libyuv/row_neon.cc \
    # thirdparty/libyuv/row_neon64.cc \
    # thirdparty/libyuv/row_win.cc \
    # thirdparty/libyuv/scale.cc \
    # thirdparty/libyuv/scale_any.cc \
    # thirdparty/libyuv/scale_argb.cc \
    # thirdparty/libyuv/scale_common.cc \
    # thirdparty/libyuv/scale_gcc.cc \
    # thirdparty/libyuv/scale_lsx.cc \
    # thirdparty/libyuv/scale_mmi.cc \
    # thirdparty/libyuv/scale_msa.cc \
    # thirdparty/libyuv/scale_neon.cc \
    # thirdparty/libyuv/scale_neon64.cc \
    # thirdparty/libyuv/scale_rgb.cc \
    # thirdparty/libyuv/scale_uv.cc \
    # thirdparty/libyuv/scale_win.cc \
    # thirdparty/libyuv/video_common.cc \

