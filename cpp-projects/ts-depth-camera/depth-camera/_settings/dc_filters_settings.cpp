
/*******************************************************************************
** Toolset-ts-depth-camera                                                    **
** MIT License                                                                **
** Copyright (c) [2018] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/

#include "dc_filters_settings.hpp"

// std
#include <format>

// base
#include "utility/io_data.hpp"
#include "utility/logger.hpp"
#include "utility/json_utility.hpp"

using namespace tool::cam;
using namespace tool::img;
using json = nlohmann::json;


auto DCFiltersSettings::default_init_for_calibration() -> DCFiltersSettings{
    DCFiltersSettings filters;
    // TODO
    return filters;
}

auto DCFiltersSettings::init_from_json(const nlohmann::json &json) -> void{

    size_t unreadCount = 0;
    // base
    Settings::init_from_json(read_and_return_object(json, unreadCount, "base"sv));
    // depth filtering
    read_and_update_value(json, unreadCount, "min_widht_f"sv, minWidthF);
    read_and_update_value(json, unreadCount, "max_widht_f"sv, maxWidthF);
    read_and_update_value(json, unreadCount, "min_height_f"sv, minHeightF);
    read_and_update_value(json, unreadCount, "max_height_f"sv, maxHeightF);
    read_and_update_value(json, unreadCount, "min_depth_f"sv, minDepthF);
    read_and_update_value(json, unreadCount, "max_depth_f"sv, maxDepthF);
    // # from depth-sized color
    read_and_update_value(json, unreadCount, "filter_depth_with_color"sv, filterDepthWithColor);
    read_and_update_array<float>(json, unreadCount,   "filter_color"sv, filterColor.array);
    read_and_update_array<float>(json, unreadCount,   "max_diff_color"sv, maxDiffColor.array);
    // # from infra
    read_and_update_value(json, unreadCount, "filter_depth_with_infra"sv, filterDepthWithInfra);
    // # from body tracking
    read_and_update_value(json, unreadCount, "filter_depth_with_body_tracking"sv, filterDepthWithBodyTracking);
    // # from cloud
    read_and_update_value(json, unreadCount, "filter_depth_with_cloud"sv, filterDepthWithCloud);
    p1FMode     = static_cast<PlaneFilteringMode>(read_and_return_value(json, unreadCount, "plane_filtering_mode"sv, static_cast<int>(p1FMode)));
    read_and_update_array<float>(json, unreadCount, "plane1_A"sv, p1A.array);
    read_and_update_array<float>(json, unreadCount, "plane1_B"sv, p1B.array);
    read_and_update_array<float>(json, unreadCount, "plane1_C"sv, p1C.array);
    read_and_update_value(json, unreadCount, "remove_from_point_distance"sv,    removeFromPointDistance);
    read_and_update_array<float>(json, unreadCount, "sphere"sv,           pSphere.array);
    read_and_update_value(json, unreadCount, "max_sphere_distance"sv,           maxSphereDistance);
    read_and_update_value(json, unreadCount, "keep_only_points_inside_OOB"sv,   keepOnlyPointsInsideOOB);
    read_and_update_array<float>(json, unreadCount, "OOB_position"sv,     oob.position.array);
    read_and_update_array<float>(json, unreadCount, "OOB_rotation"sv,     oob.rotation.array);
    read_and_update_array<float>(json, unreadCount, "OOB_size"sv,         oob.size.array);
    // # complex
    read_and_update_value(json, unreadCount, "do_local_diff_filtering"sv,       doLocalDiffFiltering);
    read_and_update_value(json, unreadCount, "max_local_diff"sv,                maxLocalDiff);
    localDiffConnectivity         = static_cast<DCConnectivity>(read_and_return_value(json, unreadCount, "local_diff_connectivity"sv, static_cast<int>(localDiffConnectivity)));
    // ## min neighbours
    read_and_update_value(json, unreadCount, "do_min_neighbours_filtering"sv,   doMinNeighboursFiltering);
    read_and_update_value(json, unreadCount, "nb_min_neighbours"sv,             nbMinNeighbours);
    read_and_update_value(json, unreadCount, "min_neighbours_loops"sv,          minNeighboursLoops);
    minNeighboursConnectivity     = static_cast<DCConnectivity>(read_and_return_value(json, unreadCount, "min_neighbours_connectivity"sv, static_cast<int>(minNeighboursConnectivity)));
    // ## erosion
    read_and_update_value(json, unreadCount, "do_erosion"sv, doErosion);
    read_and_update_value(json, unreadCount, "erosion_loops"sv, erosionLoops);
    erosionConnectivity           = static_cast<DCConnectivity>(read_and_return_value<int>(json, unreadCount, "erosion_connectivity"sv, static_cast<int>(erosionConnectivity)));
    // ## bigger cluster
    read_and_update_value(json, unreadCount, "keep_only_bigger_cluster"sv, keepOnlyBiggestCluster);
    // ## remove after closest point
    read_and_update_value(json, unreadCount, "remove_after_closest_point"sv, removeAfterClosestPoint);
    read_and_update_value(json, unreadCount, "max_distance_after_closest_point"sv, maxDistanceAfterClosestPoint);
    // depth-sized color filtering
    read_and_update_value(json, unreadCount, "invalidate_color_from_depth"sv, invalidateColorFromDepth);
    // infra filtering
    read_and_update_value(json, unreadCount, "invalidate_infra_from_depth"sv, invalidateInfraFromDepth);

    if(unreadCount != 0){
        tool::Log::warning(std::format("[DCFiltersSettings::init_from_json] [{}] values have not been initialized from json data.\n", unreadCount));
    }
}

auto DCFiltersSettings::convert_to_json() const -> nlohmann::json{

    json json;
    // base
    add_value(json, "base"sv,                               Settings::convert_to_json());
    // depth filtering
    // # basic
    add_value(json, "min_widht_f"sv,                        minWidthF);
    add_value(json, "max_widht_f"sv,                        maxWidthF);
    add_value(json, "min_height_f"sv,                       minHeightF);
    add_value(json, "max_height_f"sv,                       maxHeightF);
    add_value(json, "min_depth_f"sv,                        minDepthF);
    add_value(json, "max_depth_f"sv,                        maxDepthF);
    // # from depth-sized color
    add_value(json, "filter_depth_with_color"sv,            filterDepthWithColor);
    add_array<float>(json, "filter_color"sv,                filterColor.array);
    add_array<float>(json, "max_diff_color"sv,              maxDiffColor.array);
    // # from infra
    add_value(json, "filter_depth_with_infra"sv,            filterDepthWithInfra);
        // # from body tracking
    add_value(json, "filter_depth_with_body_tracking"sv,    filterDepthWithBodyTracking);
    // # from cloud
    add_value(json, "filter_depth_with_cloud"sv,            filterDepthWithCloud);
    add_value(json, "plane_filtering_mode"sv,               static_cast<int>(p1FMode));
    add_array<float>(json, "plane1_A"sv,                    p1A.array);
    add_array<float>(json, "plane1_B"sv,                    p1B.array);
    add_array<float>(json, "plane1_C"sv,                    p1C.array);
    add_value(json, "remove_from_point_distance"sv,         removeFromPointDistance);
    add_array<float>(json, "sphere"sv,                      pSphere.array);
    add_value(json, "max_sphere_distance"sv,                maxSphereDistance);
    add_value(json, "keep_only_points_inside_OOB"sv,        keepOnlyPointsInsideOOB);
    add_array<float>(json, "OOB_position"sv,                oob.position.array);
    add_array<float>(json, "OOB_rotation"sv,                oob.rotation.array);
    add_array<float>(json, "OOB_size"sv,                    oob.size.array);
    // # complex
    add_value(json, "do_local_diff_filtering"sv,            doLocalDiffFiltering);
    add_value(json, "max_local_diff"sv,                     maxLocalDiff);
    add_value(json, "local_diff_connectivity"sv,            static_cast<int>(localDiffConnectivity));
    // ## min neighbours
    add_value(json, "do_min_neighbours_filtering"sv,        doMinNeighboursFiltering);
    add_value(json, "nb_min_neighbours"sv,                  nbMinNeighbours);
    add_value(json, "min_neighbours_loops"sv,               minNeighboursLoops);
    add_value(json, "min_neighbours_connectivity"sv,        minNeighboursConnectivity);
    // ## erosion
    add_value(json, "do_erosion"sv,                         doErosion);
    add_value(json, "erosion_loops"sv,                      erosionLoops);
    add_value(json, "erosion_connectivity"sv,               static_cast<int>(erosionConnectivity));
    // ## bigger cluster
    add_value(json, "keep_only_bigger_cluster"sv,           keepOnlyBiggestCluster);
    // ## remove after closest point
    add_value(json, "remove_after_closest_point"sv,         removeAfterClosestPoint);
    add_value(json, "max_distance_after_closest_point"sv,   maxDistanceAfterClosestPoint);
    // depth-sized color filtering
    add_value(json, "invalidate_color_from_depth"sv,        invalidateColorFromDepth);
    // infra filtering
    add_value(json, "invalidate_infra_from_depth"sv,        invalidateInfraFromDepth);

    return json;
}

auto DCFiltersSettings::init_from_data(std::byte const * const data, size_t &offset, size_t sizeData) -> void{
    
    Settings::init_from_data(data, offset, sizeData);
    read(minWidthF, data, offset, sizeData);
    read(maxWidthF, data, offset, sizeData);
    read(minHeightF, data, offset, sizeData);
    read(maxHeightF, data, offset, sizeData);
    read(minDepthF, data, offset, sizeData);
    read(maxDepthF, data, offset, sizeData);
    read(filterDepthWithColor, data, offset, sizeData);
    read(filterColor, data, offset, sizeData);
    read(maxDiffColor, data, offset, sizeData);
    read(filterDepthWithInfra, data, offset, sizeData);
    read(filterDepthWithBodyTracking, data, offset, sizeData);
    read(filterDepthWithCloud, data, offset, sizeData);
    read(p1FMode, data, offset, sizeData);
    read(p1A, data, offset, sizeData);
    read(p1B, data, offset, sizeData);
    read(p1C, data, offset, sizeData);
    read(removeFromPointDistance, data, offset, sizeData);
    read(pSphere, data, offset, sizeData);
    read(maxSphereDistance, data, offset, sizeData);
    read(keepOnlyPointsInsideOOB, data, offset, sizeData);
    read(oob, data, offset, sizeData);
    read(doLocalDiffFiltering, data, offset, sizeData);
    read(maxLocalDiff, data, offset, sizeData);
    read(localDiffConnectivity, data, offset, sizeData);
    read(doMinNeighboursFiltering, data, offset, sizeData);
    read(nbMinNeighbours, data, offset, sizeData);
    read(minNeighboursLoops, data, offset, sizeData);
    read(minNeighboursConnectivity, data, offset, sizeData);
    read(doErosion, data, offset, sizeData);
    read(erosionLoops, data, offset, sizeData);
    read(erosionConnectivity, data, offset, sizeData);
    read(keepOnlyBiggestCluster, data, offset, sizeData);
    read(removeAfterClosestPoint, data, offset, sizeData);
    read(maxDistanceAfterClosestPoint, data, offset, sizeData);
    read(invalidateColorFromDepth, data, offset, sizeData);
    read(invalidateInfraFromDepth, data, offset, sizeData);

    version = SettingsVersion::LastVersion;
}
