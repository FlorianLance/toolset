
# /*******************************************************************************
# ** Toolset-ts-base                                                            **
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
TARGET = ts-base
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

TS_BASE_DEP_INCLUDEPATH =\
    $$TS_BASE_THIRDPARTY_INCLUDES\

####################################### LIBS

TS_BASE_DEP_LIBS =\
    $$TS_BASE_THIRDPARTY_LIBS\

####################################### DEP

TS_BASE_PRE_TARGETDEPS =\

####################################### GENERATE VARIABLES

include(../ts-gen-var.pri)

####################################### PROJECT FILES
HEADERS += \
    ## depth-camera
    depth-camera/dc_enums.hpp \
    depth-camera/dc_types.hpp \
    ### frame
    depth-camera/frame/dc_frame.hpp \
    ### network
    depth-camera/network/dc_network_enums.hpp \
    depth-camera/network/dc_network_types.hpp \
    ### settings
    depth-camera/settings/dc_calibrator_settings.hpp \
    depth-camera/settings/dc_capture_settings.hpp \
    # depth-camera/settings/dc_client_device_settings.hpp \
    # depth-camera/settings/dc_client_settings.hpp \
    depth-camera/settings/dc_data_frame_generation_settings.hpp \
    # depth-camera/settings/dc_device_connection_settings.hpp \
    depth-camera/settings/dc_device_data_settings.hpp \
    depth-camera/settings/dc_frame_generation_settings.hpp \
    depth-camera/settings/dc_misc_settings.hpp \
    # depth-camera/settings/dc_server_settings.hpp \
    depth-camera/settings/dc_video_player_settings.hpp \
    depth-camera/settings/dc_video_recorder_settings.hpp \
    depth-camera/settings/dc_settings_paths.hpp \
    depth-camera/settings/dc_device_settings.hpp \
    depth-camera/settings/dc_data_settings.hpp \
    depth-camera/settings/dc_color_settings.hpp \
    depth-camera/settings/dc_display_settings.hpp \
    depth-camera/settings/dc_config_settings.hpp \
    depth-camera/settings/dc_filters_settings.hpp \
    depth-camera/settings/dc_model_settings.hpp \
    ### states
    depth-camera/states/dc_video_player_states.hpp \
    depth-camera/states/dc_video_recorder_states.hpp \
    depth-camera/states/dc_calibrator_states.hpp \
    ## geometry
    geometry/triangle.hpp \
    geometry/aabb3.hpp \
    geometry/circle.hpp \
    geometry/obb3.hpp \
    geometry/plane3.hpp \
    geometry/sphere.hpp \
    geometry/matrix.hpp \
    geometry/matrix2.hpp \
    geometry/matrix3.hpp \
    geometry/matrix4.hpp \
    geometry/point.hpp \
    geometry/point2.hpp \
    geometry/point3.hpp \
    geometry/point4.hpp \
    geometry/dummy.hpp \
    geometry/maching_cube.hpp \
    geometry/vertices.hpp \
    geometry/voxel.hpp \
    geometry/quaternion.hpp \
    geometry/_aabb2.hpp \
    geometry/_geometry.hpp \
    geometry/_geometry2.hpp \
    geometry/_geometry3.hpp \
    geometry/_interval.hpp \
    geometry/_line2.hpp \
    geometry/_line3.hpp \
    geometry/_octree.hpp \
    geometry/_ray3.hpp \
    geometry/_raycast.hpp \
    geometry/_rectangle.hpp \
    geometry/_transform.hpp \
    geometry/color_cloud.hpp \
    geometry/color_voxel_grid.hpp \
    geometry/geometry_formatter.hpp \
    geometry/cloud_io.hpp\
    geometry/camera_matrices.hpp \
    geometry/camera.hpp \
    geometry/screen.hpp \
    ## image
    image/color_utility.hpp \
    image/hsv.hpp \
    image/rgb.hpp \
    image/texture.hpp \
    image/cube_map.hpp \
    image/texture_2d.hpp \
    image/texture_enum.hpp \
    image/texture_options.hpp \
    image/image_io.hpp\
    image/light.hpp \
    image/material_enum.hpp \
    image/material.hpp \
    image/frame.hpp \
    ## input
    input/joypad.hpp\
    input/mouse.hpp\
    input/keyboard.hpp\
    # network
    network/checksum.hpp\
    network/network_enums.hpp \
    network/network_types.hpp \
    ### settings
    network/settings/remote_server_settings.hpp \
    network/settings/udp_connection_settings.hpp \
    network/settings/udp_server_settings.hpp \
    # utility
    thirdparty/ColorSpace/ColorSpace.h \
    thirdparty/ColorSpace/Comparison.h \
    thirdparty/ColorSpace/Conversion.h \
    thirdparty/ColorSpace/Utils.h \
    utility/__algorithm.hpp \
    utility/__cmd_args.hpp \
    utility/base_logger.hpp \
    utility/array.hpp \
    utility/benchmark.hpp \
    utility/bit_mask.hpp \
    utility/buffer.hpp \
    utility/constants.hpp \
    utility/decimal.hpp \
    utility/export.hpp \
    utility/format.hpp \
    utility/id_alias_map.hpp \
    utility/image_buffer.hpp \
    utility/int_24.hpp \
    utility/io_data.hpp \
    utility/io_fstream.hpp \
    utility/math.hpp \
    utility/numeric_buffer.hpp \
    utility/paths.hpp \
    utility/ring_buffer.hpp \
    utility/safe_queue.hpp \
    utility/stop_watch.hpp \
    utility/string.hpp \
    utility/string_unordered_map.hpp \
    utility/thread.hpp \
    utility/time_guard.hpp \
    utility/times_data_buffer.hpp \
    utility/types.hpp \
    utility/unordered_map.hpp \
    utility/unordered_set.hpp \
    utility/vector.hpp \
    utility/logger.hpp \
    utility/tuple_array.hpp \
    utility/time.hpp \
    utility/io_file.hpp \
    utility/monitoring.hpp \
    utility/time_diff.hpp \
    utility/visit.hpp \
    utility/json_utility.hpp\
    utility/bdf_header.hpp\
    utility/settings.hpp\
    utility/io_enum.hpp\

SOURCES += \
    ## camera
    depth-camera/dc_types.cpp \
    ## frame
    depth-camera/frame/dc_frame.cpp \
    ### settings
    depth-camera/settings/dc_filters_settings.cpp \
    depth-camera/settings/dc_model_settings.cpp \
    depth-camera/settings/dc_config_settings.cpp \
    depth-camera/settings/dc_color_settings.cpp \
    depth-camera/settings/dc_display_settings.cpp \
    depth-camera/settings/dc_data_settings.cpp \
    depth-camera/settings/dc_device_settings.cpp \
    depth-camera/settings/dc_settings_paths.cpp \
    depth-camera/settings/dc_capture_settings.cpp \
    # depth-camera/settings/dc_client_device_settings.cpp \
    # depth-camera/settings/dc_client_settings.cpp \
    depth-camera/settings/dc_data_frame_generation_settings.cpp \
    # depth-camera/settings/dc_device_connection_settings.cpp \
    depth-camera/settings/dc_frame_generation_settings.cpp \
    depth-camera/settings/dc_misc_settings.cpp \
    # depth-camera/settings/dc_server_settings.cpp \
    ## geometry
    geometry/color_cloud.cpp \
    geometry/color_voxel_grid.cpp \
    geometry/cloud_io.cpp\
    geometry/camera.cpp \
    ## image
    image/color_utility.cpp \
    image/texture.cpp \
    image/cube_map.cpp \
    image/texture_2d.cpp \
    image/frame.cpp \
    ## network
    network/network_types.cpp \
    ### settings
    network/settings/remote_server_settings.cpp \
    network/settings/udp_connection_settings.cpp \
    network/settings/udp_server_settings.cpp \
    # utility
    thirdparty/ColorSpace/ColorSpace.cpp \
    thirdparty/ColorSpace/Comparison.cpp \
    thirdparty/ColorSpace/Conversion.cpp \
    utility/__cmd_args.cpp \
    utility/base_logger.cpp \
    utility/benchmark.cpp \
    utility/decimal.cpp \
    utility/io_file.cpp \
    utility/logger.cpp \
    utility/string.cpp \
    utility/stop_watch.cpp \
    utility/monitoring.cpp \
    utility/time_diff.cpp \
    utility/bdf_header.cpp\
    utility/settings.cpp\
    utility/paths.cpp \

