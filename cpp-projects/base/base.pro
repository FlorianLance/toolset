
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

####################################### REPOSITORY
TOOLSET_REPOSITORY_DIR      = $$PWD"/../.."

####################################### TARGET/TEMPMATE/CONFIG
TARGET = base
TEMPLATE = lib
CONFIG += staticlib
CONFIG -= console
CONFIG -= qt

####################################### PRI
include(../ts-settings.pri)
include(../ts-projects.pri)
include(../ts-thirdparty.pri)
include(../ts-dependencies.pri)


####################################### PROJECT FILES
HEADERS += \
    # io            
    data/bdf_writer.hpp \
    depth-camera/dc_device_array.hpp \
    depth-camera/frame/dc_data_frame.hpp \
    depth-camera/frame/dc_data_frame_buffer.hpp \
    depth-camera/frame/dc_data_frame_generator.hpp \
    depth-camera/frame/dc_depth_indices.hpp \
    depth-camera/frame/dc_device_data.hpp \
    depth-camera/frame/dc_frame_generator.hpp \
    depth-camera/frame/dc_frame_processor.hpp \
    depth-camera/impl/femto_base_device.hpp \
    depth-camera/impl/femto_mega_ethernet_device_impl.hpp \
    depth-camera/impl/femto_mega_usb_device_impl.hpp \
    depth-camera/impl/gemini_215_device_impl.hpp \
    depth-camera/impl/gemini_base_device.hpp \
    depth-camera/network/dc_server.hpp \
    depth-camera/settings/dc_capture_settings.hpp \
    depth-camera/settings/dc_client_device_settings.hpp \
    depth-camera/settings/dc_client_settings.hpp \
    depth-camera/settings/dc_data_frame_generation_settings.hpp \
    depth-camera/settings/dc_device_connection_settings.hpp \
    depth-camera/settings/dc_device_data_settings.hpp \
    depth-camera/settings/dc_frame_generation_settings.hpp \
    depth-camera/settings/dc_misc_settings.hpp \
    depth-camera/settings/dc_server_settings.hpp \
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
    io/cloud_io.hpp \
    io/assimp_loader.hpp \
    io/image_io.hpp \
    io/io_enum.hpp \
    io/settings.hpp \
    io/text_settings.hpp \
    io/joypad.hpp \
    io/mouse.hpp \
    io/keyboard.hpp \
    # geometry 
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
    # graphics
    graphics/light.hpp \
    graphics/mesh.hpp \
    graphics/mesh_generation.hpp \
    ## animation
    graphics/animation/animation.hpp \
    graphics/animation/bones.hpp \
    ## material
    graphics/material/material_enum.hpp \
    graphics/material/material.hpp \
    ## texture
    graphics/texture/texture.hpp \
    graphics/texture/cube_map.hpp \
    graphics/texture/texture_2d.hpp \
    graphics/texture/texture_enum.hpp \
    graphics/texture/texture_options.hpp \
    ## model
    graphics/model/models_manager.hpp \
    graphics/model/model_mesh.hpp \
    ## color        
    graphics/color/color_utility.hpp \
    graphics/color/hsv.hpp \
    graphics/color/rgb.hpp \
    ## camera
    graphics/camera/camera_matrices.hpp \
    graphics/camera/camera.hpp \
    graphics/camera/screen.hpp \
    # network
    network/boost_asio.hpp \
    network/settings/remote_server_settings.hpp \
    network/settings/udp_connection_settings.hpp \
    network/udp_client.hpp \
    network/network_enums.hpp \
    network/network_types.hpp \
    network/tcp_reader.hpp \
    network/tcp_sender.hpp \
    network/tcp_server.hpp \
    network/udp_reader.hpp \
    network/udp_sender.hpp \
    ## settings
    network/settings/udp_server_settings.hpp \
    # camera
    camera/frame.hpp \
    # depth-camera    
    depth-camera/dc_device.hpp \
    depth-camera/dc_enums.hpp \
    depth-camera/dc_packed_data.hpp \
    depth-camera/dc_types.hpp \
    depth-camera/dc_player_settings.hpp \
    depth-camera/dc_calibrator.hpp \
    depth-camera/dc_device_impl.hpp \
    thirdparty/ColorSpace/ColorSpace.h \
    thirdparty/ColorSpace/Comparison.h \
    thirdparty/ColorSpace/Conversion.h \
    thirdparty/ColorSpace/Utils.h \
    utility/__algorithm.hpp \
    utility/base_logger.hpp \
    depth-camera/dc_video.hpp \
    depth-camera/dc_video_player.hpp \
    depth-camera/dc_video_recorder.hpp \
    ## frame
    depth-camera/frame/dc_frame.hpp \
    ## client
    depth-camera/client/dc_client.hpp \
    depth-camera/client/dc_client_processing.hpp \
    depth-camera/client/dc_client_device.hpp \
    depth-camera/client/dc_client_local_device.hpp \
    depth-camera/client/dc_client_remote_device.hpp \
    ## states
    depth-camera/states/dc_video_player_states.hpp \
    depth-camera/states/dc_video_recorder_states.hpp \
    depth-camera/states/dc_calibrator_states.hpp \
    ## settings
    depth-camera/settings/dc_video_player_settings.hpp \
    depth-camera/settings/dc_video_recorder_settings.hpp \
    depth-camera/settings/dc_settings_paths.hpp \
    depth-camera/settings/dc_device_settings.hpp \
    depth-camera/settings/dc_data_settings.hpp \
    depth-camera/settings/dc_color_settings.hpp \
    depth-camera/settings/dc_display_settings.hpp \
    depth-camera/settings/dc_calibrator_settings.hpp \
    depth-camera/settings/dc_config_settings.hpp \
    depth-camera/settings/dc_filters_settings.hpp \
    depth-camera/settings/dc_model_settings.hpp \
    ## network
    depth-camera/network/dc_network_enums.hpp \
    depth-camera/network/dc_network_types.hpp \
    ## impl
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
    # # exvr
    # exvr/ex_element.hpp \
    # exvr/ex_experiment.hpp \
    # exvr/ex_resource.hpp \
    # exvr/ex_component.hpp \
    # exvr/ex_logger.hpp \
    # utility        
    utility/array.hpp \
    utility/benchmark.hpp \
    utility/bit_mask.hpp \
    utility/buffer.hpp \
    utility/cmd_args.hpp \
    utility/constants.hpp \
    utility/decimal.hpp \
    utility/export.hpp \
    utility/format.hpp \
    utility/geometry_formatter.hpp \
    utility/id_alias_map.hpp \
    utility/image_buffer.hpp \
    utility/int_24.hpp \
    utility/io_data.hpp \
    utility/io_fstream.hpp \
    utility/math.hpp \
    utility/paths.hpp \
    utility/ring_buffer.hpp \
    utility/safe_queue.hpp \
    utility/stop_watch.hpp \
    utility/string.hpp \
    utility/string_geo.hpp \
    utility/string_unordered_map.hpp \
    utility/thread.hpp \
    utility/time_guard.hpp \
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
    # algorithms
    algorithms/marching_cube.hpp \
    # data
    data/fastpfor_encoding.hpp \
    data/jpeg_encoding.hpp \
    data/FastDifferentialCoding/fastdelta.h \
    data/checksum.hpp \
    data/json_utility.hpp \    
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
    ## taskflow
    thirdparty/taskflow/taskflow.hpp \


SOURCES += \
    # io    
    depth-camera/client/dc_client.cpp \
    depth-camera/dc_device_array.cpp \
    depth-camera/frame/dc_data_frame.cpp \
    depth-camera/frame/dc_data_frame_buffer.cpp \
    depth-camera/frame/dc_data_frame_generator.cpp \
    depth-camera/frame/dc_depth_indices.cpp \
    depth-camera/frame/dc_device_data.cpp \
    depth-camera/frame/dc_frame_generator.cpp \
    depth-camera/frame/dc_frame_processor.cpp \
    depth-camera/impl/femto_base_device.cpp \
    depth-camera/impl/femto_mega_ethernet_device_impl.cpp \
    depth-camera/impl/femto_mega_usb_device_impl.cpp \
    depth-camera/impl/gemini_215_device_impl.cpp \
    depth-camera/impl/gemini_base_device.cpp \
    depth-camera/network/dc_server.cpp \
    depth-camera/settings/dc_capture_settings.cpp \
    depth-camera/settings/dc_client_device_settings.cpp \
    depth-camera/settings/dc_client_settings.cpp \
    depth-camera/settings/dc_data_frame_generation_settings.cpp \
    depth-camera/settings/dc_device_connection_settings.cpp \
    depth-camera/settings/dc_frame_generation_settings.cpp \
    depth-camera/settings/dc_misc_settings.cpp \
    depth-camera/settings/dc_server_settings.cpp \
    geometry/color_cloud.cpp \
    geometry/color_voxel_grid.cpp \
    io/assimp_loader.cpp \
    io/cloud_io.cpp \
    io/settings.cpp \
    # graphics     
    graphics/mesh.cpp \
    graphics/mesh_generation.cpp \
    ## animation
    graphics/animation/animation.cpp \
    graphics/animation/bones.cpp \
    ## model    
    graphics/model/model_mesh.cpp \
    graphics/model/models_manager.cpp \
    ## texture
    graphics/texture/texture.cpp \
    graphics/texture/cube_map.cpp \
    graphics/texture/texture_2d.cpp \
    ## color
    graphics/color/color_utility.cpp \
    ## camera
    graphics/camera/camera.cpp \
    # geometry    
    geometry/vertices.cpp \
    # network
    network/network_types.cpp \
    ## settings    
    network/settings/remote_server_settings.cpp \
    network/settings/udp_connection_settings.cpp \
    network/settings/udp_server_settings.cpp \
    # utility    
    thirdparty/ColorSpace/ColorSpace.cpp \
    thirdparty/ColorSpace/Comparison.cpp \
    thirdparty/ColorSpace/Conversion.cpp \
    utility/base_logger.cpp \
    utility/benchmark.cpp \
    utility/cmd_args.cpp \
    utility/decimal.cpp \
    utility/io_file.cpp \
    utility/logger.cpp \
    utility/paths.cpp \
    utility/string.cpp \
    utility/stop_watch.cpp \
    utility/monitoring.cpp \
    utility/time_diff.cpp \
    # network
    network/tcp_reader.cpp \
    network/tcp_sender.cpp \
    network/tcp_server.cpp \
    network/udp_reader.cpp \
    network/udp_sender.cpp \
    # camera
    camera/frame.cpp \
    # depth-camera            
    depth-camera/dc_video_player.cpp \
    depth-camera/dc_video_recorder.cpp \
    depth-camera/dc_packed_data.cpp \
    depth-camera/dc_calibrator.cpp \
    depth-camera/dc_device.cpp \
    depth-camera/dc_device_impl.cpp \    
    depth-camera/dc_types.cpp \
    depth-camera/dc_video.cpp \
    ## frame
    depth-camera/frame/dc_frame.cpp \
    ## client
    depth-camera/client/dc_client_processing.cpp \
    depth-camera/client/dc_client_local_device.cpp \
    depth-camera/client/dc_client_remote_device.cpp \
    ## network
    ## settings
    depth-camera/settings/dc_filters_settings.cpp \
    depth-camera/settings/dc_model_settings.cpp \
    depth-camera/settings/dc_config_settings.cpp \
    depth-camera/settings/dc_color_settings.cpp \
    depth-camera/settings/dc_display_settings.cpp \
    depth-camera/settings/dc_data_settings.cpp \
    depth-camera/settings/dc_device_settings.cpp \
    depth-camera/settings/dc_settings_paths.cpp \
    ## impl
    depth-camera/impl/k2_config_files.cpp \
    depth-camera/impl/k2_device.cpp \
    depth-camera/impl/k2_manager.cpp \
    depth-camera/impl/k2_types.cpp \
    depth-camera/impl/azure_kinect_device_impl.cpp \
    depth-camera/impl/azure_base_device.cpp \
    depth-camera/impl/femto_bolt_device_impl.cpp \
    depth-camera/impl/recording_device_impl.cpp \
    # exvr
    # exvr/ex_logger.cpp \
    # exvr/ex_component.cpp \
    # exvr/ex_element.cpp \
    # exvr/ex_experiment.cpp \
    # exvr/ex_resource.cpp \
    # data
    data/fastpfor_encoding.cpp \
    data/jpeg_encoding.cpp \
    data/FastDifferentialCoding/fastdelta.c \
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

