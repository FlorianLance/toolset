
/*******************************************************************************
** Toolset-base                                                               **
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

// local
#include "utility/io_data.hpp"
#include "utility/logger.hpp"

using namespace tool::cam;

DCFiltersSettings::DCFiltersSettings(){
    sType   = io::SettingsType::Filters;
    version = io::Version::v1_0;
}

auto DCFiltersSettings::default_init_for_calibration() -> DCFiltersSettings{
    DCFiltersSettings filters;
    // TODO
    return filters;
}

auto DCFiltersSettings::init_from_data(std::int8_t const * const data, size_t &offset, size_t sizeData) -> void{

    if(offset + total_data_size() > sizeData){
        tool::Logger::error(std::format("DCFiltersSettings::init_from_data: Not enought data space for initializing data: [{}] required [{}]\n", sizeData-offset, total_data_size()));
        return;
    }

    BaseSettings::init_from_data(data, offset, sizeData);
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
}

auto DCFiltersSettings::write_to_data(std::int8_t * const data, size_t &offset, size_t sizeData) const -> void{

    if(offset + total_data_size() > sizeData){
        tool::Logger::error(std::format("DCFiltersSettings::write_to_data: Not enought data space for writing to data: [{}] required [{}]\n", sizeData-offset, total_data_size()));
        return;
    }

    BaseSettings::write_to_data(data, offset, sizeData);
    write(minWidthF, data, offset, sizeData);
    write(maxWidthF, data, offset, sizeData);
    write(minHeightF, data, offset, sizeData);
    write(maxHeightF, data, offset, sizeData);
    write(minDepthF, data, offset, sizeData);
    write(maxDepthF, data, offset, sizeData);
    write(filterDepthWithColor, data, offset, sizeData);
    write(filterColor, data, offset, sizeData);
    write(maxDiffColor, data, offset, sizeData);
    write(filterDepthWithInfra, data, offset, sizeData);
    write(filterDepthWithBodyTracking, data, offset, sizeData);
    write(filterDepthWithCloud, data, offset, sizeData);
    write(p1FMode, data, offset, sizeData);
    write(p1A, data, offset, sizeData);
    write(p1B, data, offset, sizeData);
    write(p1C, data, offset, sizeData);
    write(removeFromPointDistance, data, offset, sizeData);
    write(pSphere, data, offset, sizeData);
    write(maxSphereDistance, data, offset, sizeData);
    write(doLocalDiffFiltering, data, offset, sizeData);
    write(maxLocalDiff, data, offset, sizeData);
    write(localDiffConnectivity, data, offset, sizeData);
    write(doMinNeighboursFiltering, data, offset, sizeData);
    write(nbMinNeighbours, data, offset, sizeData);
    write(minNeighboursLoops, data, offset, sizeData);
    write(minNeighboursConnectivity, data, offset, sizeData);
    write(doErosion, data, offset, sizeData);
    write(erosionLoops, data, offset, sizeData);
    write(erosionConnectivity, data, offset, sizeData);
    write(keepOnlyBiggestCluster, data, offset, sizeData);
    write(removeAfterClosestPoint, data, offset, sizeData);
    write(maxDistanceAfterClosestPoint, data, offset, sizeData);
    write(invalidateColorFromDepth, data, offset, sizeData);
    write(invalidateInfraFromDepth, data, offset, sizeData);
}

auto DCFiltersSettings::total_data_size() const noexcept -> size_t{
    return
        BaseSettings::total_data_size() +
        sizeof(minWidthF) +
        sizeof(maxWidthF) +
        sizeof(minHeightF) +
        sizeof(maxHeightF) +
        sizeof(minDepthF) +
        sizeof(maxDepthF) +
        sizeof(filterDepthWithColor) +
        sizeof(filterColor) +
        sizeof(maxDiffColor) +
        sizeof(filterDepthWithInfra) +
        sizeof(filterDepthWithBodyTracking) +
        sizeof(filterDepthWithCloud) +
        sizeof(p1FMode) +
        sizeof(p1A) +
        sizeof(p1B) +
        sizeof(p1C) +
        sizeof(removeFromPointDistance) +
        sizeof(pSphere) +
        sizeof(maxSphereDistance) +
        sizeof(doLocalDiffFiltering) +
        sizeof(maxLocalDiff) +
        sizeof(localDiffConnectivity) +
        sizeof(doMinNeighboursFiltering) +
        sizeof(nbMinNeighbours) +
        sizeof(minNeighboursLoops) +
        sizeof(minNeighboursConnectivity) +
        sizeof(doErosion) +
        sizeof(erosionLoops) +
        sizeof(erosionConnectivity) +
        sizeof(keepOnlyBiggestCluster) +
        sizeof(removeAfterClosestPoint) +
        sizeof(maxDistanceAfterClosestPoint) +
        sizeof(invalidateColorFromDepth) +
        sizeof(invalidateInfraFromDepth);
}