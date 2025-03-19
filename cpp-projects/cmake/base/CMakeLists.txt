cmake_minimum_required(VERSION 3.16)

project(base LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# target_link_libraries(base PRIVATE

# )


add_executable(base main.cpp
    # utility
    utility/algorithm.hpp
    utility/array.hpp
    utility/base_logger.cpp
    utility/base_logger.hpp
    utility/benchmark.cpp
    utility/benchmark.hpp
    utility/bit_mask.hpp
    utility/buffer.hpp
    utility/cmd_args.cpp
    utility/cmd_args.hpp
    utility/constants.hpp
    utility/decimal.cpp
    utility/decimal.hpp
    utility/export.hpp
    utility/format.hpp
    utility/geometry_formatter.hpp
    utility/id_alias_map.hpp
    utility/image_buffer.hpp
    utility/io_data.hpp
    utility/io_file.cpp
    utility/io_file.hpp
    utility/io_fstream.hpp
    utility/logger.cpp
    utility/logger.hpp
    utility/math.hpp
    utility/monitoring.cpp
    utility/monitoring.hpp
    utility/paths.cpp
    utility/paths.hpp
    utility/ring_buffer.hpp
    utility/safe_queue.hpp
    utility/stop_watch.cpp
    utility/stop_watch.hpp
    utility/string.cpp
    utility/string.hpp
    utility/string_geo.hpp
    utility/string_unordered_map.hpp
    utility/thread.hpp
    utility/time.hpp
    utility/time_diff.cpp
    utility/time_diff.hpp
    utility/time_guard.hpp
    utility/tuple_array.hpp
    utility/types.hpp
    utility/unordered_map.hpp
    utility/unordered_set.hpp
    utility/vector.hpp
    utility/visit.hpp
    geometry/_aabb2.hpp geometry/_geometry.hpp geometry/_geometry2.hpp geometry/_geometry3.hpp geometry/_interval.hpp
    geometry/_line2.hpp geometry/_line3.hpp geometry/_octree.hpp geometry/_ray3.hpp geometry/_raycast.hpp geometry/_rectangle.hpp geometry/_transform.hpp
    geometry/aabb3.hpp geometry/circle.hpp geometry/color_cloud.cpp geometry/color_cloud.hpp geometry/color_voxel_grid.cpp
    geometry/color_voxel_grid.hpp geometry/matrix.hpp geometry/matrix2.hpp
    geometry/matrix3.hpp geometry/matrix4.hpp geometry/obb3.hpp geometry/plane3.hpp geometry/point.hpp geometry/point2.hpp geometry/point3.hpp
    geometry/point4.hpp geometry/quaternion.hpp geometry/sphere.hpp geometry/transformation.hpp geometry/triangle.hpp geometry/vertices.cpp
    geometry/vertices.hpp geometry/voxel.hpp
)

include(GNUInstallDirs)
install(TARGETS base
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

target_include_directories(base PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)



# find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
# find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

# add_library(libPing SHARED
# libPing_global.h
# libping.cpp
# libping.h
# )

# target_link_libraries(libPing PRIVATE Qt${QT_VERSION_MAJOR}::Core)

# target_compile_definitions(libPing PRIVATE LIBPING_LIBRARY)

# target_include_directories(libPing INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})


# https://doc.qt.io/qt-6/cmake-get-started.html


############################################### TYPICAL

# cmake_minimum_required(VERSION 3.16)

# project(helloworld VERSION 1.0.0 LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(Qt6 REQUIRED COMPONENTS Core)
# qt_standard_project_setup()

# qt_add_executable(helloworld
#     main.cpp
# )

# target_link_libraries(helloworld PRIVATE Qt6::Core)

############################################### GUI APP

# cmake_minimum_required(VERSION 3.16)

# project(helloworld VERSION 1.0.0 LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(Qt6 REQUIRED COMPONENTS Widgets)
# qt_standard_project_setup()

# qt_add_executable(helloworld
#     mainwindow.ui
#     mainwindow.cpp
#     main.cpp
# )

# target_link_libraries(helloworld PRIVATE Qt6::Widgets)

# set_target_properties(helloworld PROPERTIES
#     WIN32_EXECUTABLE ON
#     MACOSX_BUNDLE ON
# )

############################################### SUB DIRECTORIES

# ################## PARENT

# cmake_minimum_required(VERSION 3.16)

# project(helloworld VERSION 1.0.0 LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(Qt6 REQUIRED COMPONENTS Widgets)
# qt_standard_project_setup()

# add_subdirectory(src/app)


################## SUB

# qt_add_executable(helloworld
#     mainwindow.ui
#     mainwindow.cpp
#     main.cpp
# )

# target_link_libraries(helloworld PRIVATE Qt6::Widgets)

# set_target_properties(helloworld PROPERTIES
#     WIN32_EXECUTABLE ON
#     MACOSX_BUNDLE ON
# )

############################################### LIBRARY

# qt_add_library(businesslogic STATIC
#     businesslogic.cpp
# )
# target_link_libraries(businesslogic PRIVATE Qt6::Core)
# target_include_directories(businesslogic INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})





# Using libraries

# To use the library we created in the previous section, we instruct CMake to link against it:

# target_link_libraries(helloworld PRIVATE
#     businesslogic
#     Qt6::Widgets
# )


# Adding resources

# We want to display some images in our application, so we add them using the Qt Resource System.

# qt_add_resources(helloworld imageresources
#     PREFIX "/images"
#     FILES logo.png splashscreen.png
# )
