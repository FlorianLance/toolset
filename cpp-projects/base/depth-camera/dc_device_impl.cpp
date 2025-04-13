
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

#include "dc_device_impl.hpp"

// std
#include <execution>
// #include <iostream>

#include <thirdparty/taskflow/algorithm/transform.hpp>
#include <thirdparty/taskflow/algorithm/for_each.hpp>

// opencv
#include <opencv2/imgproc.hpp>


// local
#include "utility/logger.hpp"
#include "utility/time.hpp"
#include "utility/vector.hpp"
#include "graphics/color/color_utility.hpp"

using namespace tool::geo;
using namespace tool::cam;


DCDeviceImpl::DCDeviceImpl(){

    // taskflow
    {

        auto copyTimeT = m_processFramesT.emplace([&](){
            Log::message("###################### COPY TIMES\n");
            timesLocker.lock();
            times = timeM.times;
            timesLocker.unlock();
        }).name("copyTime");

        auto readFrameT = m_processFramesT.emplace([&](tf::Subflow& readFrameSubflow){

            auto captureFrameT = readFrameSubflow.emplace([&](){

                Log::message("###################### CAPTURE\n");

                fData.reset_spans();

                auto tCF = TimeDiffGuard(timeM, "CAPTURE_FRAME"sv);

                auto t1 = Time::nanoseconds_since_epoch();
                captureSuccess = capture_frame(mInfos.timeout_ms());

                Log::fmessage("C[{}] [{}] \n", captureSuccess, Time::now_difference_micro_s(t1));
                if(!captureSuccess){
                    dataIsValid = false;
                    return 1;
                }
                return 0;
            }).name("captureFrame");

            // auto readImagesT = readFrameSubflow.emplace([&](tf::Subflow& readImagesSubflow){
            auto readImagesT = readFrameSubflow.emplace([&](){

               auto tRI = TimeDiffGuard(timeM, "READ_IMAGES"sv);
               {
                   auto tRCI = TimeDiffGuard(timeM, "READ_COLOR_IMAGE"sv);
                   read_color_image(mInfos.has_color() && settings.data.capture.color);
               }
               {
                   auto tRDI = TimeDiffGuard(timeM, "READ_DEPTH_IMAGE"sv);
                   read_depth_image(mInfos.has_depth() && settings.data.capture.depth);
               }
               {
                   auto tRII = TimeDiffGuard(timeM, "READ_INFRA_IMAGE"sv);
                   read_infra_image(mInfos.has_infra() && settings.data.capture.infra);
               }
               {
                   auto tBT = TimeDiffGuard(timeM, "READ_BODY_TRACKING"sv);
                   read_body_tracking(settings.data.capture.bodyTracking && settings.config.btEnabled && mInfos.has_depth());
               }
               {
                   auto tRA = TimeDiffGuard(timeM, "READ_AUDIO"sv);
                   read_audio(settings.data.capture.audio && mInfos.has_audio());
               }
               {
                   auto tRI = TimeDiffGuard(timeM, "READ_IMU"sv);
                   read_IMU(settings.data.capture.imu);
               }

                // auto readColorImageT = readImagesSubflow.emplace([&](){
                //     auto tRCI = TimeDiffGuard(timeM, "READ_COLOR_IMAGE"sv);
                //     read_color_image(mInfos.has_color() && settings.data.capture.color);
                // }).name("readColorImage");

                // auto readDepthImageT = readImagesSubflow.emplace([&](){
                //     auto tRDI = TimeDiffGuard(timeM, "READ_DEPTH_IMAGE"sv);
                //     read_depth_image(mInfos.has_depth() && settings.data.capture.depth);
                // }).name("readDepthImage");

                // auto readInfraImageT = readImagesSubflow.emplace([&](){
                //     auto tRII = TimeDiffGuard(timeM, "READ_INFRA_IMAGE"sv);
                //     read_infra_image(mInfos.has_infra() && settings.data.capture.infra);
                // }).name("readInfraImage");

                // auto readBodyTrackingT = readImagesSubflow.emplace([&](){
                //     auto tBT = TimeDiffGuard(timeM, "READ_BODY_TRACKING"sv);
                //     read_body_tracking(settings.data.capture.bodyTracking && settings.config.btEnabled && mInfos.has_depth());
                // }).name("readBodyTracking");

                // auto readAudioT = readImagesSubflow.emplace([&](){
                //     auto tRA = TimeDiffGuard(timeM, "READ_AUDIO"sv);
                //     read_audio(settings.data.capture.audio && mInfos.has_audio());
                // }).name("readAudio");

                // auto readIMUT = readImagesSubflow.emplace([&](){
                //     auto tRI = TimeDiffGuard(timeM, "READ_IMU"sv);
                //     read_IMU(settings.data.capture.imu);
                // }).name("readIMU");

                // readColorImageT.precede(readDepthImageT);
                // readDepthImageT.precede(readInfraImageT);
                // readInfraImageT.precede(readBodyTrackingT);

            }).name("readImages");

            auto resetFramesT = readFrameSubflow.emplace([&](){                
                frame   = nullptr;
                dFrame  = nullptr;
            }).name("resetFrames");


            auto checkDataValidityT = readFrameSubflow.emplace([&](){
                dataIsValid = check_data_validity();
                return (captureSuccess && dataIsValid) ? 0 : 1;
            }).name("checkDataValidity");



            auto processDataT = readFrameSubflow.emplace([&](tf::Subflow& processDataSubflow){

                auto initFramesT = processDataSubflow.emplace([&](){

                    timeM.start("PROCESSING_DATA"sv);

                    // Log::message("initFramesT\n");
                    framerateB.add_frame();

                    if(settings.data.generation.has_data()){
                        frame = std::make_shared<DCFrame>();
                    }

                    if(settings.data.sending.has_data()){
                        dFrame = std::make_shared<DCDataFrame>();
                    }

                    // update parameters

                    minW   = static_cast<std::uint16_t>(mInfos.depth_width()  * settings.filters.minWidthF);
                    maxW   = static_cast<std::uint16_t>(mInfos.depth_width()  * settings.filters.maxWidthF);
                    minH   = static_cast<std::uint16_t>(mInfos.depth_height() * settings.filters.minHeightF);
                    maxH   = static_cast<std::uint16_t>(mInfos.depth_height() * settings.filters.maxHeightF);

                    dRange                              = mInfos.depth_range_mm();
                    diffRange                           = dRange.y()-dRange.x();
                    minD                                = static_cast<std::uint16_t>(dRange.x() + settings.filters.minDepthF * diffRange);
                    maxD                                = static_cast<std::uint16_t>(dRange.x() + settings.filters.maxDepthF * diffRange);
                    hsvDiffColor                        = Convert::to_hsv(settings.filters.filterColor);
                    maxDiffColor                        = settings.filters.maxDiffColor;
                    fdc                                 = settings.filters.filterDepthWithColor && (fData.depthSizedColor.size() == fData.depth.size());
                    // Log::fmessage("FDC {} {} {}\n", fdc, fData.depthSizedColor.size(), fData.depth.size());

                    // # plane filtering
                    p1                                  = settings.filters.p1A*1000.f;
                    p2                                  = settings.filters.p1B*1000.f;
                    p3                                  = settings.filters.p1C*1000.f;
                    meanPt                              = (p1+p2+p3)/3.f;
                    AB                                  = vec(p2,p1);
                    AC                                  = vec(p3,p1);
                    normalV                             = normalize(cross(AB,AC));
                    // # sphere filtering
                    pSphere                             = settings.filters.pSphere*1000.f;
                    squareMaxDistanceFromPoint          = settings.filters.maxSphereDistance*settings.filters.maxSphereDistance;
                    // # oob filtering
                    oobRot                              = settings.filters.oob.rotation;
                    oobOrientation                      = geo::rotation_m3x3(geo::Vec3f{deg_2_rad(oobRot.x()), deg_2_rad(oobRot.y()), deg_2_rad(oobRot.z())});
                    invO                                = inverse(oobOrientation);
                    halfDimensions                      = settings.filters.oob.size * 500.f;
                    oobPos                              = settings.filters.oob.position * 1000.f;


                    minNeighBoursFilteringIterations    = settings.filters.minNeighboursLoops;
                    erosionsIterations                  = settings.filters.erosionLoops;

                    currentZoneId                       = 1;
                    biggestZone                         = -1;
                    sizeBiggestZone                     = 0;

                    fData.meanBiggestZoneId             = 0;

                    std::fill(fData.depthMask.begin(), fData.depthMask.end(), 1);

                }).name("initFrames");

                tf::Task convertColorImageT = processDataSubflow.emplace([&](){
                    auto tG = TimeDiffGuard(timeM, "CONVERT_COLOR_IMAGE"sv);
                    convert_color_image();
                }).name("convertColorImage");

                tf::Task resizeColorImageT  = processDataSubflow.emplace([&](){
                    auto tG = TimeDiffGuard(timeM, "RESIZE_IMAGES"sv);
                    resize_images();
                }).name("resizeColorImage");

                tf::Task generateCloudT = processDataSubflow.emplace([&](){
                    auto tGC = TimeDiffGuard(timeM, "GENERATE_CLOUD"sv);
                    generate_cloud(settings.data.capture_cloud() || settings.filters.filterDepthWithCloud);
                }).name("generateCloud");

                tf::Task checkFilterDepthT = processDataSubflow.emplace([&](){
                    return !fData.depth.empty() ? 0 : 1;
                }).name("checkFilterDepth");

                tf::Task checkFilterDepthFromDepthSizedColorT = processDataSubflow.emplace([&](){
                    timeM.start("FILTER"sv);
                    return fdc ? 0 : 1;
                }).name("checkFilterDepthFromDepthSizedColor");

                auto fillHSVDiffMaskT = processDataSubflow.for_each(std::ref(dIndices.depths1DStart), std::ref(dIndices.depths1DEnd), [&](size_t id){
                    auto hsv = Convert::to_hsv(fData.depthSizedColor[id]);
                    fData.hsvDiffMask[id] =
                    (
                        (std::abs(hsv.h()- hsvDiffColor.h()) <= maxDiffColor.h()) &&
                        (std::abs(hsv.s()- hsvDiffColor.s()) <= maxDiffColor.s()) &&
                        (std::abs(hsv.v()- hsvDiffColor.v()) <= maxDiffColor.v())
                    ) ? 1 : 0;
                }, tf::DynamicPartitioner(64)).name("fillHSVDiffMask");

                auto endColorProcessingT = processDataSubflow.emplace([&](){

                }).name("endColorProcessing");

                tf::Task generateFramesT = processDataSubflow.emplace([&](){

                }).name("generateFrames");

                auto filterDepthBasicT = processDataSubflow.for_each(std::ref(dIndices.depths3DStart), std::ref(dIndices.depths3DEnd), [&](const Pt3<size_t> &dIndex){

                    const auto &currentDepth = fData.depth[dIndex.x()];
                    fData.depthMask[dIndex.x()] =
                    (
                        (currentDepth == dc_invalid_depth_value) ||     // invalid
                        (dIndex.y()   < minW) ||                        // width
                        (dIndex.y()   > maxW) ||                        //   -
                        (dIndex.z()   < minH) ||                        // height
                        (dIndex.z()   > maxH) ||                        //   -
                        (currentDepth < minD) ||                        // depth
                        (currentDepth > maxD) ||
                        (fdc ? (fData.hsvDiffMask[dIndex.x()] == 0) : 0)
                    ) ? 0 : 1;

                }, tf::DynamicPartitioner(64)).name("filterDepthBasic");

                auto checkFilterDepthFromCloudT = processDataSubflow.emplace([&](){
                    return
                        ((settings.filters.filterDepthWithCloud && !fData.depth.empty() && !fData.depthCloud.empty()) &&
                        ((settings.filters.p1FMode != PlaneFilteringMode::None) || settings.filters.removeFromPointDistance || settings.filters.keepOnlyPointsInsideOOB)) ? 0 : 1;
                }).name("checkFilterDepthFromCloud");

                auto filterDepthFromCloudT = processDataSubflow.for_each(std::ref(dIndices.depthVertexCorrrespondanceStart), std::ref(dIndices.depthVertexCorrrespondanceEnd), [&](const auto &idC){

                    auto idD = std::get<0>(idC);

                    if(fData.depth[idD] == dc_invalid_depth_value){
                        return;
                    }

                    if(settings.filters.p1FMode != PlaneFilteringMode::None){
                        if(dot(normalV,vec(meanPt, fData.depthCloud[idD].template conv<float>())) < 0){
                            if(settings.filters.p1FMode == PlaneFilteringMode::Above){
                                fData.depthMask[idD] = 0;
                                return;
                            }
                        }else{
                            if(settings.filters.p1FMode == PlaneFilteringMode::Below){
                                fData.depthMask[idD] = 0;
                                return;
                            }
                        }
                    }

                    if(settings.filters.removeFromPointDistance){
                        if(square_norm(vec(pSphere,fData.depthCloud[idD].template conv<float>())) > squareMaxDistanceFromPoint){
                            fData.depthMask[idD] = 0;
                            return;
                        }
                    }

                    if(settings.filters.keepOnlyPointsInsideOOB){

                        auto pointLocal = (fData.depthCloud[idD].template conv<float>()) - oobPos;// settings.filters.oob.position;
                        auto pointLocalRotated = pointLocal * invO;
                        if(std::abs(pointLocalRotated.x())  > halfDimensions.x() ||
                            std::abs(pointLocalRotated.y()) > halfDimensions.y() ||
                            std::abs(pointLocalRotated.z()) > halfDimensions.z()){
                            fData.depthMask[idD] = 0;
                            return;
                        }
                    }

                }, tf::DynamicPartitioner(64)).name("filterDepthFromCloud");

                auto filterDepthComplexT = processDataSubflow.emplace([&](tf::Subflow& filterDepthComplexSubflow){

                    auto initDoMinNeighboursFilteringT =  filterDepthComplexSubflow.emplace([&](){
                        minNeighBoursFilteringIterations = settings.filters.minNeighboursLoops;
                    }).name("initDoMinNeighboursFiltering");

                    auto checkDoMinNeighboursFilteringT = filterDepthComplexSubflow.emplace([&](){
                        return (settings.filters.doMinNeighboursFiltering && minNeighBoursFilteringIterations > 0) ? 0 : 1;
                    }).name("checkDoMinNeighboursFiltering");

                    auto doMinNeighboursFilteringT = filterDepthComplexSubflow.for_each(std::ref(dIndices.depths1DNoBordersStart), std::ref(dIndices.depths1DNoBordersEnd), [&](const auto &id){
                        fData.filteringMask[id] = (fData.depthMask[id] != 0) ? (std::accumulate(dIndices.neighbours8Depth1D[id].begin(), dIndices.neighbours8Depth1D[id].end(), 0, [&](int total, size_t cId){
                            return total + fData.depthMask[cId];
                        }) >= settings.filters.nbMinNeighbours ? 1 : 0) : 0;
                    }, tf::DynamicPartitioner(64)).name("doMinNeighboursFiltering");

                    auto doUpdateDepthMask1T = filterDepthComplexSubflow.for_each(std::ref(dIndices.depths1DNoBordersStart), std::ref(dIndices.depths1DNoBordersEnd), [&](const auto &id){
                        if(fData.filteringMask[id] == 0){
                            fData.depthMask[id] = 0;
                        }
                    },tf::DynamicPartitioner(64)).name("doUpdateDepthMask1");

                    auto backToDoMinNeighboursFilteringT =  filterDepthComplexSubflow.emplace([&](){
                        --minNeighBoursFilteringIterations;
                        return 0;
                    }).name("backToDoMinNeighboursFiltering");

                    auto initDoErosionT =  filterDepthComplexSubflow.emplace([&](){
                        erosionsIterations = settings.filters.erosionLoops;
                    }).name("initDoErosion");

                    auto checkDoErosionT = filterDepthComplexSubflow.emplace([&](){
                        return (settings.filters.doErosion && erosionsIterations > 0) ? 0 : 1;
                    }).name("checkDoErosion");

                    auto doErosionT = filterDepthComplexSubflow.for_each(std::ref(dIndices.depths1DNoBordersStart), std::ref(dIndices.depths1DNoBordersEnd), [&](const auto &id){
                        fData.filteringMask[id] = (fData.depthMask[id] != 0) ? (std::accumulate(dIndices.neighbours4Depth1D[id].begin(), dIndices.neighbours4Depth1D[id].end(), 0, [&](int total, size_t cId){
                            return total + fData.depthMask[cId];
                        }) >= 4 ? 1 : 0) : 0;
                    }, tf::DynamicPartitioner(64)).name("doErosion");

                    auto doUpdateDepthMask2T = filterDepthComplexSubflow.for_each(std::ref(dIndices.depths1DNoBordersStart), std::ref(dIndices.depths1DNoBordersEnd), [&](const auto &id){
                        if(fData.filteringMask[id] == 0){
                            fData.depthMask[id] = 0;
                        }
                    },tf::DynamicPartitioner(64)).name("doUpdateDepthMask2");

                    auto backToDoErosionT =  filterDepthComplexSubflow.emplace([&](){
                        --erosionsIterations;
                        return 0;
                    }).name("backToDoErosion");

                    auto checkDoLocalDiffFilteringT = filterDepthComplexSubflow.emplace([&](){
                        return settings.filters.doLocalDiffFiltering ? 0 : 1;
                    }).name("checkDoLocalDiffFiltering");

                    auto doLocalDiffFilteringT = filterDepthComplexSubflow.for_each(std::ref(dIndices.depths1DNoBordersStart), std::ref(dIndices.depths1DNoBordersEnd), [&](const auto &id){

                        float currDepth = fData.depth[id];
                        float meanDiff = 0.f;
                        std::uint8_t count = 0;
                        for(const auto &cId : dIndices.neighbours4Depth1D[id]){
                            if(fData.depthMask[cId] == 1){
                                meanDiff += abs(fData.depth[cId]-currDepth);
                                ++count;
                            }
                        }
                        fData.filteringMask[id] = (count == 0) ? 0 : ((1.*meanDiff/count < settings.filters.maxLocalDiff) ? 1 : 0);

                    }, tf::DynamicPartitioner(64)).name("doLocalDiffFiltering");

                    auto doUpdateDepthMask3T = filterDepthComplexSubflow.for_each(std::ref(dIndices.depths1DNoBordersStart), std::ref(dIndices.depths1DNoBordersEnd), [&](const auto &id){
                        if(fData.filteringMask[id] == 0){
                            fData.depthMask[id] = 0;
                        }
                    },tf::DynamicPartitioner(64)).name("doUpdateDepthMask3");

                    auto checkKeepOnlyBiggestClusterT = filterDepthComplexSubflow.emplace([&](){
                        return settings.filters.keepOnlyBiggestCluster ? 0 : 1;
                    }).name("checkKeepOnlyBiggestCluster");

                    auto keepOnlyBiggestClusterT = filterDepthComplexSubflow.emplace([&](){
                        keep_only_biggest_cluster();
                    }).name("keepOnlyBiggestCluster");

                    auto checkRemoveAfterClosestPointT = filterDepthComplexSubflow.emplace([&](){
                        return settings.filters.removeAfterClosestPoint ? 0 : 1;
                    }).name("checkRemoveAfterClosestPoint");

                    auto removeAfterClosestPointT = filterDepthComplexSubflow.emplace([&](tf::Subflow& removeAfterClosestPointSubflow){

                        auto removeAfterClosestPointSub1T = removeAfterClosestPointSubflow.emplace([&](){

                            minDist = std::numeric_limits<std::uint16_t>::max();
                            std::for_each(std::execution::unseq, std::begin(dIndices.depths1D), std::end(dIndices.depths1D), [&](size_t id){
                                if(fData.depthMask[id] != 0){
                                    if(minDist > fData.depth[id]){
                                        minDist = fData.depth[id];
                                    }
                                }
                            });
                            maxDist = minDist + static_cast<std::uint16_t>(1000.f * settings.filters.maxDistanceAfterClosestPoint);

                        }).name("removeAfterClosestPointSub1");

                        auto removeAfterClosestPointSub2T = removeAfterClosestPointSubflow.for_each(std::ref(dIndices.depths1DStart), std::ref(dIndices.depths1DEnd), [&](const auto &id){
                            if(fData.depth[id] > maxDist){
                                fData.depthMask[id] = 0;
                            }

                        }, tf::DynamicPartitioner(64)).name("removeAfterClosestPointSub2");

                        removeAfterClosestPointSub1T.precede(removeAfterClosestPointSub2T);

                    }).name("removeAfterClosestPoint");

                    initDoMinNeighboursFilteringT.precede(checkDoMinNeighboursFilteringT);
                    checkDoMinNeighboursFilteringT.precede(doMinNeighboursFilteringT, initDoErosionT);
                    doMinNeighboursFilteringT.precede(doUpdateDepthMask1T);
                    doUpdateDepthMask1T.precede(backToDoMinNeighboursFilteringT);
                    backToDoMinNeighboursFilteringT.precede(checkDoMinNeighboursFilteringT);

                    initDoErosionT.precede(checkDoErosionT);
                    checkDoErosionT.precede(doErosionT, checkDoLocalDiffFilteringT);
                    doErosionT.precede(doUpdateDepthMask2T);
                    doUpdateDepthMask2T.precede(backToDoErosionT);
                    backToDoErosionT.precede(checkDoErosionT);

                    checkDoLocalDiffFilteringT.precede(doLocalDiffFilteringT, checkKeepOnlyBiggestClusterT);
                    doLocalDiffFilteringT.precede(doUpdateDepthMask3T);
                    doUpdateDepthMask3T.precede(checkKeepOnlyBiggestClusterT);

                    checkKeepOnlyBiggestClusterT.precede(keepOnlyBiggestClusterT, checkRemoveAfterClosestPointT);
                    keepOnlyBiggestClusterT.precede(checkRemoveAfterClosestPointT);
                    checkRemoveAfterClosestPointT.precede(removeAfterClosestPointT);

                }).name("filterDepthComplex");

                auto updateValidDepthValuesT = processDataSubflow.emplace([&](){
                    update_valid_depth_values();
                    timeM.end("FILTER"sv);
                }).name("updateValidDepthValues");

                auto checkFilterDepthSizedColorFromDepthT = processDataSubflow.emplace([&](){
                    return settings.filters.invalidateColorFromDepth && (!fData.depthSizedColor.empty() && (fData.depth.size() == fData.depthSizedColor.size())) ? 0 : 1;
                }).name("checkFilterDepthSizedColorFromDepth");

                auto filterDepthSizedColorFromDepthT = processDataSubflow.for_each(std::ref(dIndices.depths1DStart), std::ref(dIndices.depths1DEnd), [&](const auto &id){
                    if(fData.depth[id] == dc_invalid_depth_value){
                        fData.depthSizedColor[id] = ColorRGBA8{dc_invalid_color_value};
                    }
                }, tf::DynamicPartitioner(64)).name("filterDepthSizedColorFromDepthT");


                auto checkFilterInfraFromDepthT = processDataSubflow.emplace([&](){
                    return settings.filters.invalidateInfraFromDepth && (!fData.infra.empty() && (fData.depth.size() == fData.infra.size())) ? 0 : 1;
                }).name("checkFilterInfraFromDepth");

                auto filterInfraFromDepthT = processDataSubflow.for_each(std::ref(dIndices.depths1DStart), std::ref(dIndices.depths1DEnd), [&](const auto &id){
                    if(fData.depth[id] == dc_invalid_depth_value){
                        fData.infra[id] = dc_invalid_infra_value;
                    }
                }, tf::DynamicPartitioner(64)).name("filterInfraFromDepth");

                // auto endInfraFilteringT = processDataSubflow.emplace([&](){

                // }).name("endInfraFiltering");


                auto updateDataFrameT = processDataSubflow.emplace([&](){
                    auto tUCF = TimeDiffGuard(timeM, "UPDATE_DATA_FRAME"sv);
                    // Log::message("updateDataFrameT\n");
                    update_data_frame_color();
                    update_data_frame_depth_sized_color();
                    update_data_frame_depth();
                    update_data_frame_infra();
                    update_data_frame_cloud();
                    update_data_frame_audio();
                    update_data_frame_imu();
                    update_data_frame_body_tracking();
                    update_data_frame_calibration();
                    // Log::message("end updateDataFrameT\n");
                }).name("updateDataFrame");

                auto updateFrameT = processDataSubflow.emplace([&](){
                    auto tUF = TimeDiffGuard(timeM, "UPDATE_FRAME"sv);
                    // Log::message("updateFrameT2\n");
                    update_frame_original_size_color();
                    update_frame_depth_sized_color();
                    update_frame_depth();
                    update_frame_infra();
                    update_frame_cloud();
                    update_frame_audio();
                    update_frame_imu();
                    update_frame_body_tracking();
                    update_frame_calibration();
                    // Log::message("end updateFrameT\n");
                }).name("updateFrame");

                // auto updateDataFrameColorT = processDataSubflow.emplace([&](){
                //     update_data_frame_color();
                // }).name("updateDataFrameColor");

                // auto updateDataFrameDepthSizedColorT = processDataSubflow.emplace([&](){
                //     update_data_frame_depth_sized_color();
                // }).name("updateDataFrameDepthSizedColor");

                // auto updateDataFrameDepthT = processDataSubflow.emplace([&](){
                //     update_data_frame_depth();
                // }).name("updateDataFrameDepth");

                // auto updateDataFrameInfraT = processDataSubflow.emplace([&](){
                //     update_data_frame_infra();
                // }).name("updateDataFrameInfra");

                // auto updateDataFrameCloudT = processDataSubflow.emplace([&](){
                //     update_data_frame_cloud();
                //     // decompose ...
                // }).name("updateDataFrameCloud");

                // auto updateDataFrameAudioT = processDataSubflow.emplace([&](){
                //     Log::message("update_data_frame_audio\n");
                //     update_data_frame_audio();
                // }).name("updateDataFrameAudio");

                // auto updateDataFrameIMUT = processDataSubflow.emplace([&](){
                //     Log::message("update_data_frame_imu\n");
                //     update_data_frame_imu();
                // }).name("updateDataFrameIMU");

                // auto updateDataFrameBodyTrackingT = processDataSubflow.emplace([&](){
                //     Log::message("update_data_frame_body_tracking\n");
                //     update_data_frame_body_tracking();
                // }).name("updateDataFrameBodyTracking");

                // auto updateDataFrameCalibrationT = processDataSubflow.emplace([&](){
                //     Log::message("update_data_frame_calibration\n");
                //     update_data_frame_calibration();
                // }).name("updateDataFrameCalibration");

                auto finalizeDataFrameT = processDataSubflow.emplace([&](){

                    auto tFCF = TimeDiffGuard(timeM, "FINALIZE_DATA_FRAME"sv);
                    if(dFrame != nullptr){
                        // set infos
                        dFrame->idDevice           = static_cast<std::uint8_t>(settings.config.idDevice);
                        dFrame->idCapture          = static_cast<std::int32_t>(mInfos.id_capture());
                        dFrame->afterCaptureTS     = timeM.get_end("CAPTURE_FRAME"sv).count();
                        dFrame->receivedTS         = dFrame->afterCaptureTS; // default init received TS with after capture TS
                        dFrame->mode               = settings.config.mode;
                        dFrame->validVerticesCount = fData.validDepthValues;

                        // add frame
                        frames.add_data_frame(std::move(dFrame));
                    }
                }).name("finalizeDataFrame");

                auto sendDataFrameT = processDataSubflow.emplace([&](){
                    auto tSCF = TimeDiffGuard(timeM, "SEND_DATA_FRAME"sv);
                    if(auto dataFrameToSend = frames.get_data_frame_with_delay(timeM.get_end("CAPTURE_FRAME"sv), settings.misc.delayMs)){
                        dFrame =dataFrameToSend;
                        new_data_frame_signal(std::move(dataFrameToSend));
                    }
                }).name("sendDataFrame");

                auto finalizeFrameT = processDataSubflow.emplace([&](){

                    auto tFF = TimeDiffGuard(timeM, "FINALIZE_FRAME"sv);
                    if(frame != nullptr){
                        // set infos
                        frame->idCapture       = static_cast<std::int32_t>(mInfos.id_capture());
                        frame->afterCaptureTS  = timeM.get_end("CAPTURE_FRAME"sv).count();
                        frame->receivedTS      = frame->afterCaptureTS;  // default init received TS with after capture TS

                        frame->infosB[DCInfoType::DCMode] = static_cast<std::int64_t>(settings.config.mode);
                        frame->mode            = settings.config.mode;

                        frames.add_frame(std::move(frame));
                    }
                }).name("finalizeFrame");

                auto sendFrameT = processDataSubflow.emplace([&](){
                    auto tSF = TimeDiffGuard(timeM, "SEND_FRAME"sv);
                    if(auto frameToSend = frames.take_frame_with_delay(timeM.get_end("CAPTURE_FRAME"sv), settings.misc.delayMs)){
                        frame = frameToSend;
                        new_frame_signal(std::move(frameToSend));
                    }
                }).name("sendFrame");


                auto endProcessingT = processDataSubflow.emplace([&](){
                    if(captureSuccess){
                        mInfos.increment_capture_id();
                    }
                    timeM.end("PROCESSING_DATA"sv);
                }).name("endProcessing");


                // initFramesT.precede(convertColorImageT, generateCloudT);
                convertColorImageT.precede(resizeColorImageT);
                resizeColorImageT.precede(checkFilterDepthFromDepthSizedColorT);

                checkFilterDepthFromDepthSizedColorT.precede(fillHSVDiffMaskT, endColorProcessingT);
                fillHSVDiffMaskT.precede(endColorProcessingT);

                generateCloudT.precede(initFramesT);
                endColorProcessingT.precede(initFramesT);

                initFramesT.precede(checkFilterDepthT);
                checkFilterDepthT.precede(filterDepthBasicT, updateValidDepthValuesT);

                filterDepthBasicT.precede(checkFilterDepthFromCloudT);

                checkFilterDepthFromCloudT.precede(filterDepthFromCloudT, filterDepthComplexT);
                filterDepthFromCloudT.precede(filterDepthComplexT);
                filterDepthComplexT.precede(updateValidDepthValuesT);

                updateValidDepthValuesT.precede(checkFilterDepthSizedColorFromDepthT);
                checkFilterDepthSizedColorFromDepthT.precede(filterDepthSizedColorFromDepthT, checkFilterInfraFromDepthT);
                filterDepthSizedColorFromDepthT.precede(checkFilterInfraFromDepthT);

                checkFilterInfraFromDepthT.precede(filterInfraFromDepthT, generateFramesT);
                filterInfraFromDepthT.precede(generateFramesT);
                generateFramesT.precede(updateDataFrameT, updateFrameT);


                updateDataFrameT.precede(finalizeDataFrameT);
                finalizeDataFrameT.precede(sendDataFrameT);

                updateFrameT.precede(finalizeFrameT);
                finalizeFrameT.precede(sendFrameT);

                endProcessingT.succeed(sendDataFrameT, sendFrameT);

            }).name("processData");

            captureFrameT.precede(readImagesT, resetFramesT);
            readImagesT.precede(resetFramesT);
            resetFramesT.precede(checkDataValidityT);

            checkDataValidityT.precede(processDataT);

        }).name("ReadFrame");

        readFrameT.succeed(copyTimeT);
    }
}

DCDeviceImpl::~DCDeviceImpl(){
}

auto DCDeviceImpl::process_frames() -> std::tuple<std::shared_ptr<DCFrame>, std::shared_ptr<DCDataFrame>>{

    auto tf = TimeDiffGuard(timeM, "PROCESS_FRAMES"sv);

    timesLocker.lock();
    {
        times = timeM.times;
    }
    timesLocker.unlock();

    if(read_frame()){
        return process_data();
    }

    return {nullptr,nullptr};
}

auto DCDeviceImpl::initialize(const DCConfigSettings &newConfigS) -> void{

    auto lg = LogG("DCDeviceImpl::initialize"sv);
    auto t = TimeDiffGuard(timeM, "INITIALIZE"sv);
    settings.config = newConfigS;
    mInfos.initialize(settings.config.mode);
    dIndices.initialize(mInfos.has_depth(), mInfos.depth_width(), mInfos.depth_height());
    fData.reset(mInfos.has_depth(), mInfos.depth_size(), mInfos.has_color(), mInfos.color_width(), mInfos.color_height());
// VCL_NAMESPACE::Vec16us vnull(0);

    {
        // size_t offset = 32;
        // xIdsSIMD.resize(mInfos.depth_size()/offset);
        // yIdsSIMD.resize(mInfos.depth_size()/offset);
        // depthSIMD.resize(mInfos.depth_size()/offset);
        // filterDepthSIMD.resize(mInfos.depth_size());
        // maskSIMD.resize(mInfos.depth_size()/offset);

        // size_t idV = 0;
        // for(size_t ii = 0; ii < mInfos.depth_height(); ++ii){
        //     for(size_t jj = 0; jj < mInfos.depth_width()/offset; ++jj){
        //         // yIdsSIMD[idV] = VCL_NAMESPACE::Vec16us(ii);
        //         yIdsSIMD[idV] = VCL_NAMESPACE::Vec32us(ii);
        //         ++idV;
        //     }
        // }

        // idV = 0;
        // for(size_t ii = 0; ii < mInfos.depth_height(); ++ii){
        //     for(size_t jj = 0; jj < mInfos.depth_width()/offset; ++jj){
        //         for(size_t kk = 0; kk < offset; ++kk){
        //             xIdsSIMD[idV].insert(kk, jj*offset+kk);
        //         }
        //         ++idV;
        //     }
        // }

        // offset = 16;
        // rgbSIMD.resize(mInfos.depth_size()/offset);

        // xIdsSIMD32.resize(mInfos.depth_size()/32);
        // yIdsSIMD32.resize(mInfos.depth_size()/32);
        // depthSIMD32.resize(mInfos.depth_size()/32);
        // filterDepthSIMD.resize(mInfos.depth_size());

        // size_t idV = 0;
        // for(size_t ii = 0; ii < mInfos.depth_height(); ++ii){
        //     for(size_t jj = 0; jj < mInfos.depth_width()/32; ++jj){
        //         yIdsSIMD32[idV] = VCL_NAMESPACE::Vec32us(ii);
        //         ++idV;
        //     }
        // }
        // idV = 0;
        // for(size_t ii = 0; ii < mInfos.depth_height(); ++ii){
        //     for(size_t jj = 0; jj < mInfos.depth_width()/32; ++jj){
        //         for(size_t kk = 0; kk < 32; ++kk){
        //             xIdsSIMD32[idV].insert(kk, jj*32+kk);
        //         }
        //         ++idV;
        //     }
        // }
    }
}

auto DCDeviceImpl::convert_color_image() -> void{

    fData.originalSizeColor = {};

    if(!mInfos.has_color() || fData.rawColor.empty()){
        return;
    }

    // auto t1 = Time::nanoseconds_since_epoch();

    if(mInfos.image_format() == DCImageFormat::NV12){

        cv::Mat yuvImage(static_cast<int>(mInfos.color_height() * 3 / 2), static_cast<int>(mInfos.color_width()), CV_8UC1);
        std::copy(fData.rawColor.data(), fData.rawColor.data() + mInfos.color_size(),  reinterpret_cast<std::byte*>(yuvImage.data)); // copy Y plane
        std::copy(fData.rawColor.data() + mInfos.color_size(), fData.rawColor.data() + mInfos.color_size() + mInfos.color_size()/2, reinterpret_cast<std::byte*>(yuvImage.data) + mInfos.color_size());

        cv::Mat convMat(static_cast<int>(mInfos.color_height()), static_cast<int>(mInfos.color_width()), CV_8UC4, fData.convertedColorData.get_byte_data());
        cv::cvtColor(yuvImage, convMat, cv::COLOR_YUV2RGBA_NV12);

    }else if(mInfos.image_format() == DCImageFormat::MJPG){

        auto success = jpegColorDecoder.decode(
            mInfos.color_width(), mInfos.color_height(), fData.rawColor,
            fData.convertedColorData
        );

        if(!success){
            Log::error("DCDeviceImpl::convert_color_image: Uncompression error.\n");
            return;
        }
    }else if(mInfos.image_format() == DCImageFormat::BGRA){

        cv::Mat rawMat(static_cast<int>(mInfos.color_height()), static_cast<int>(mInfos.color_width()), CV_8UC4, fData.rawColor.data());
        cv::Mat convMat(static_cast<int>(mInfos.color_height()), static_cast<int>(mInfos.color_width()), CV_8UC4, fData.convertedColorData.get_byte_data()); // ?
        cv::cvtColor(rawMat, convMat, cv::COLOR_BGRA2RGBA);

    }else if(mInfos.image_format() == DCImageFormat::YUY2 ){

        //YUY2 is a 4:2:2 format, so there are 4 bytes per 'chunk' of data, and each 'chunk' represents 2 pixels.
        cv::Mat rawMat(static_cast<int>(mInfos.color_height()), static_cast<int>(mInfos.color_width()), CV_8UC2, fData.rawColor.data());
        cv::Mat convMat(static_cast<int>(mInfos.color_height()), static_cast<int>(mInfos.color_width()), CV_8UC4, fData.convertedColorData.get_byte_data()); // ?
        cv::cvtColor(rawMat, convMat, cv::COLOR_YUV2RGBA_YUY2);

    }else{
        Log::error("DCDeviceImpl::convert_color_image: Invalid color format.\n");
        return;
    }

    fData.originalSizeColor = fData.convertedColorData.span();


    // std::cout << "fData.originalSizeColor " << fData.originalSizeColor.size() << "\n";
    // Log::fmessage("[cc:{}]", Time::now_difference_micro_s(t1).count());
}

auto DCDeviceImpl::simd_filter() -> void{

    auto t = TimeDiffGuard(timeM, "FILTER_DEPTH_BASIC"sv);

    // depth / width / height
    if(fData.depth.empty()){
        return;
    }


    auto minW   = static_cast<std::uint16_t>(mInfos.depth_width()  * settings.filters.minWidthF);
    auto maxW   = static_cast<std::uint16_t>(mInfos.depth_width()  * settings.filters.maxWidthF);
    auto minH   = static_cast<std::uint16_t>(mInfos.depth_height() * settings.filters.minHeightF);
    auto maxH   = static_cast<std::uint16_t>(mInfos.depth_height() * settings.filters.maxHeightF);

    auto dRange     = mInfos.depth_range_mm();
    auto diffRange  = dRange.y()-dRange.x();
    auto minD = static_cast<std::uint16_t>(dRange.x() + settings.filters.minDepthF * diffRange);
    auto maxD = static_cast<std::uint16_t>(dRange.x() + settings.filters.maxDepthF * diffRange);

    // reset depth mask
    std::fill(fData.depthMask.begin(), fData.depthMask.end(), 1);

    // size_t offset = 32;

    // load depth data
    // for(size_t id = 0; id < mInfos.depth_size()/offset; ++id){
    //     depthSIMD[id].load(fData.depth.data() + id * offset);
    // }

    // auto filterSimple = false;
    // auto filterColor = settings.filters.filterDepthWithColor;

    // depth depth with width/height/depth
    // VCL_NAMESPACE::Vec16us vnull(0);
    // VCL_NAMESPACE::Vec32us vnull(0);

    // auto t1 = Time::nanoseconds_since_epoch();
    // for(int ii = 0; ii < depthSIMD.size(); ++ii){

        // depthSIMD[ii] = select(
        //     depthSIMD[ii] >= minD &&
        //     depthSIMD[ii] <= maxD &&
        //     xIdsSIMD[ii] >= minW &&
        //     xIdsSIMD[ii] <= maxW &&
        //     yIdsSIMD[ii] >= minH &&
        //     yIdsSIMD[ii] <= maxH,
        //     depthSIMD[ii],
        //     vnull
        // );

    //     // depthSIMD[ii] = select(
    //     //     filterSimple ?
    //     //     (depthSIMD[ii] >= minD &&
    //     //     depthSIMD[ii] <= maxD &&
    //     //     xIdsSIMD[ii] >= minW &&
    //     //     xIdsSIMD[ii] <= maxW &&
    //     //     yIdsSIMD[ii] >= minH &&
    //     //     yIdsSIMD[ii] <= maxH) : true,
    //     //     depthSIMD[ii],
    //     //     vnull
    //     // );
    // }


    // // filter colors
    // if(settings.filters.filterDepthWithColor && (fData.depthSizedColor.size() == fData.depth.size())){

    //     // VCL_NAMESPACE::Vec4f rgbColors;
    //     // VCL_NAMESPACE::Vec4f hsvColors;

    //     // float fCMax = VCL_NAMESPACE::horizontal_max(rgbColors);
    //     // float fCMin = VCL_NAMESPACE::horizontal_min(rgbColors);
    //     // float fDelta = fCMax - fCMin;

    //     auto hsv = Convert::to_hsv(fData.depthSizedColor[0]);
    // }

    // rgbSIMD
    // auto t = TimeDiffGuard(timeM, "FILTER_DEPTH_FROM_DEPTH_SIZED_COLOR"sv);

    // // invalidate depth from depth-sized color
    // if(!settings.filters.filterDepthWithColor){
    //     return;
    // }
    // if(fData.depth.empty()){
    //     return;
    // }
    // if(!fData.depthSizedColor.empty() && (fData.depthSizedColor.size() != fData.depth.size())){
    //     return;
    // }

    // auto hsvDiffColor = Convert::to_hsv(settings.filters.filterColor);
    // std::for_each(std::execution::par_unseq, std::begin(dIndices.depths3D), std::end(dIndices.depths3D), [&](const Pt3<size_t> &dIndex){
    //     size_t id = dIndex.x();
    //     auto hsv = Convert::to_hsv(fData.depthSizedColor[id]);
    //     if((std::abs(hsv.h()- hsvDiffColor.h()) > settings.filters.maxDiffColor.x()) ||
    //         (std::abs(hsv.s()- hsvDiffColor.s()) > settings.filters.maxDiffColor.y()) ||
    //         (std::abs(hsv.v()- hsvDiffColor.v()) > settings.filters.maxDiffColor.z())){
    //         fData.depthMask[id] = 0;
    //         return;
    //     }
    // });



    // Log::fmessage("[db5:{}]", Time::now_difference_micro_s(t1).count());

    // for(size_t id = 0; id < mInfos.depth_size()/offset; ++id){
    //     depthSIMD[id].store(filterDepthSIMD.data() + id *offset);
    // }
    // for(size_t idV = 0; idV < fData.depthMask.size(); ++idV){
    //     fData.depthMask[idV] = filterDepthSIMD[idV] != 0;
    // }

    // 1 1 1
    // 1 0 1
    // 1 1 1
    // std::vector<VCL_NAMESPACE::Vec8us> v8;

    // for(size_t ii = 0; ii < mInfos.depth_width(); ++ii){
    //     for(size_t jj = 0; jj < mInfos.depth_height(); ++jj){

    //         if(ii > 0 && ii < mInfos.depth_width()-1 && jj > 0 && jj < mInfos.depth_height()-1){

    //             VCL_NAMESPACE::Vec8us u;

    //         }else if(ii == 0){
    //             // if(jj > 0 && jj < mInfos.depth_height()-1){

    //             // }
    //         }
    //         // ...

    //     }
    // }

    // std::vector<std::uint8_t> vvv;
    // vvv.resize(mInfos.depth_size());


    // std::vector<VCL_NAMESPACE::Vec64uc> v64;
    // v64.resize(mInfos.depth_size()/64);
    // size_t id = 0;
    // const auto data = v64.data();
    // for(size_t ii = 0; ii < mInfos.depth_width()/8; ++ii){
    //     for(size_t jj = 0; jj < mInfos.depth_height()/8; ++jj){


    //         // VCL_NAMESPACE::Vec8uc v1;
    //         v64[id].load_partial(8, data[(ii+0)*mInfos.depth_width()+jj*8]);
    //     }
    // }

    // 80 x 72 = 5760
    // [8x8] [8x8] ... [8x8]
    // ...
    // [8x8]

}

auto DCDeviceImpl::filter_depth_basic() -> void{

    auto t = TimeDiffGuard(timeM, "FILTER_DEPTH_BASIC"sv);

    // depth / width / height
    if(fData.depth.empty()){
        return;
    }

    minW   = static_cast<std::uint16_t>(mInfos.depth_width()  * settings.filters.minWidthF);
    maxW   = static_cast<std::uint16_t>(mInfos.depth_width()  * settings.filters.maxWidthF);
    minH   = static_cast<std::uint16_t>(mInfos.depth_height() * settings.filters.minHeightF);
    maxH   = static_cast<std::uint16_t>(mInfos.depth_height() * settings.filters.maxHeightF);

    dRange     = mInfos.depth_range_mm();
    diffRange  = dRange.y()-dRange.x();
    minD = static_cast<std::uint16_t>(dRange.x() + settings.filters.minDepthF * diffRange);
    maxD = static_cast<std::uint16_t>(dRange.x() + settings.filters.maxDepthF * diffRange);

    hsvDiffColor = Convert::to_hsv(settings.filters.filterColor);
    maxDiffColor = settings.filters.maxDiffColor;

    fdc = settings.filters.filterDepthWithColor && (fData.depthSizedColor.size() == fData.depth.size());
    if(fdc){
        auto tS = TimeDiffGuard(timeM, "FILTER_DEPTH_FROM_DEPTH_SIZED_COLOR"sv);

        // std::for_each(std::execution::par_unseq, std::begin(dIndices.depths1D), std::end(dIndices.depths1D), [&](size_t id){
        //     fData.hsvDiffMask[id] = ColorHSV(Convert::to_hsv(fData.depthSizedColor[id]) - hsvDiffColor).abs() < maxDiffColor ? 0 : 1;
        // });
        std::for_each(std::execution::par_unseq, std::begin(dIndices.depths1D), std::end(dIndices.depths1D), [&](size_t id){
            auto hsv = Convert::to_hsv(fData.depthSizedColor[id]);
            fData.hsvDiffMask[id] =
                (
                    (std::abs(hsv.h()- hsvDiffColor.h()) <= maxDiffColor.h()) &&
                    (std::abs(hsv.s()- hsvDiffColor.s()) <= maxDiffColor.s()) &&
                    (std::abs(hsv.v()- hsvDiffColor.v()) <= maxDiffColor.v())
                ) ? 1 : 0;
        });
    }

    // depth/width/height/mask/color/infra filtering
    std::for_each(std::execution::par_unseq, std::begin(dIndices.depths3D), std::end(dIndices.depths3D), [&](const Pt3<size_t> &dIndex){

        const auto &currentDepth = fData.depth[dIndex.x()];
        fData.depthMask[dIndex.x()] =
            (
                (currentDepth == dc_invalid_depth_value) ||     // invalid
                (dIndex.y()   < minW) ||                        // width
                (dIndex.y()   > maxW) ||                        //   -
                (dIndex.z()   < minH) ||                        // height
                (dIndex.z()   > maxH) ||                        //   -
                (currentDepth < minD) ||                        // depth
                (currentDepth > maxD) ||                        //   -
                (fdc ? fData.hsvDiffMask[dIndex.x()] == 0 : 0)
            ) ? 0 : 1;
    });

}

auto DCDeviceImpl::filter_depth_from_depth_sized_color() -> void{

    // // auto t = TimeDiffGuard(timeM, "FILTER_DEPTH_FROM_DEPTH_SIZED_COLOR"sv);

    // // invalidate depth from depth-sized color
    // if(!settings.filters.filterDepthWithColor){
    //     return;
    // }
    // if(fData.depth.empty()){
    //     return;
    // }
    // if(!fData.depthSizedColor.empty() && (fData.depthSizedColor.size() != fData.depth.size())){
    //     return;
    // }
    // auto t1 = Time::nanoseconds_since_epoch();
    // auto hsvDiffColor = Convert::to_hsv(settings.filters.filterColor);
    // std::for_each(std::execution::par_unseq, std::begin(dIndices.depths3D), std::end(dIndices.depths3D), [&](const Pt3<size_t> &dIndex){
    //     size_t id = dIndex.x();
    //     auto hsv = Convert::to_hsv(fData.depthSizedColor[id]);
    //     if((std::abs(hsv.h()- hsvDiffColor.h()) > settings.filters.maxDiffColor.x()) ||
    //         (std::abs(hsv.s()- hsvDiffColor.s()) > settings.filters.maxDiffColor.y()) ||
    //         (std::abs(hsv.v()- hsvDiffColor.v()) > settings.filters.maxDiffColor.z())){
    //         fData.depthMask[id] = 0;
    //         return;
    //     }
    // });
    // Log::fmessage("[d:{}]", Time::now_difference_micro_s(t1).count());
}

auto DCDeviceImpl::filter_depth_from_infra() -> void{

    auto t = TimeDiffGuard(timeM, "FILTER_DEPTH_FROM_INFRA"sv);

    // invalidate depth from infra
    if(!settings.filters.filterDepthWithInfra){
        return;
    }
    if(fData.depth.empty()){
        return;
    }
    if(!fData.infra.empty() && (fData.infra.size() != fData.depth.size())){
        return;
    }

    // std::for_each(std::execution::par_unseq, std::begin(fIndices.depths3D), std::end(fIndices.depths3D), [&](const Pt3<size_t> &dIndex){
    //    // ...
    // });
}

auto DCDeviceImpl::filter_depth_from_cloud() -> void{

    auto t = TimeDiffGuard(timeM, "FILTER_DEPTH_FROM_CLOUD"sv);

    // invalidate depth from cloud geometry
    if(!settings.filters.filterDepthWithCloud){
        return;
    }
    if(fData.depth.empty() || fData.depthCloud.empty()){
        return;
    }

    if(settings.filters.p1FMode == PlaneFilteringMode::None && !settings.filters.removeFromPointDistance && !settings.filters.keepOnlyPointsInsideOOB){
        return; // do nothing
    }

    auto t1 = Time::nanoseconds_since_epoch();

    // plane filtering parameters
    p1             = settings.filters.p1A*1000.f;
    p2             = settings.filters.p1B*1000.f;
    p3             = settings.filters.p1C*1000.f;
    meanPt         = (p1+p2+p3)/3.f;
    AB             = vec(p2,p1);
    AC             = vec(p3,p1);
    normalV        = cross(AB,AC);
    normalV             = normalize(normalV);

    pSphere   = settings.filters.pSphere*1000.f;
    squareMaxDistanceFromPoint = settings.filters.maxSphereDistance*settings.filters.maxSphereDistance;

    oobRot = settings.filters.oob.rotation;
    oobOrientation = geo::rotation_m3x3(geo::Vec3f{deg_2_rad(oobRot.x()), deg_2_rad(oobRot.y()), deg_2_rad(oobRot.z())});
    invO = inverse(oobOrientation);
    halfDimensions = settings.filters.oob.size * 500.f;
    oobPos = settings.filters.oob.position * 1000.f;

    std::for_each(std::execution::par_unseq, std::begin(dIndices.depthVertexCorrrespondance), std::end(dIndices.depthVertexCorrrespondance), [&](const auto &idC){

        auto idD = std::get<0>(idC);

        if(fData.depth[idD] == dc_invalid_depth_value){
            return;
        }

        if(settings.filters.p1FMode != PlaneFilteringMode::None){
            if(dot(normalV,vec(meanPt, fData.depthCloud[idD].template conv<float>())) > 0){
                if(settings.filters.p1FMode == PlaneFilteringMode::Above){
                    fData.depthMask[idD] = 0;
                    return;
                }
            }else{
                if(settings.filters.p1FMode == PlaneFilteringMode::Below){
                    fData.depthMask[idD] = 0;
                    return;
                }
            }
        }

        if(settings.filters.removeFromPointDistance){
            if(square_norm(vec(pSphere,fData.depthCloud[idD].template conv<float>())) > squareMaxDistanceFromPoint){
                fData.depthMask[idD] = 0;
                return;
            }
        }

        if(settings.filters.keepOnlyPointsInsideOOB){

            auto pointLocal = (fData.depthCloud[idD].template conv<float>()) - oobPos;// settings.filters.oob.position;
            auto pointLocalRotated = pointLocal * invO;
            if(std::abs(pointLocalRotated.x())  > halfDimensions.x() ||
                std::abs(pointLocalRotated.y()) > halfDimensions.y() ||
                std::abs(pointLocalRotated.z()) > halfDimensions.z()){
                fData.depthMask[idD] = 0;
                return;
            }

        }
    });
    // Log::fmessage("[dc:{}]", Time::now_difference_micro_s(t1).count());
    // v.add_value(Time::now_difference_micro_s(t1).count());
    // Log::fmessage("cloud  time {}\n", v.get());
}

auto DCDeviceImpl::filter_depth_from_body_tracking() -> void{

    auto t = TimeDiffGuard(timeM, "FILTER_DEPTH_FROM_BODY_TRACKING"sv);
    // invalidate depth from body tracking
    if(fData.depth.empty()){
        return;
    }

    // filter cloud from body tracking positions
    // filter cloud from body tracking map
    // if(fData.depth.empty() || fData.depthCloud.empty()){
    //     return;
    // }
}

auto DCDeviceImpl::filter_depth_complex() -> void{

    auto t = TimeDiffGuard(timeM, "FILTER_DEPTH_COMPLEX"sv);
    if(fData.depth.empty()){
        return;
    }

    // auto t1 = Time::nanoseconds_since_epoch();


    // if(settings.filters.doErosion)
    // {
    //     std::fill(fData.filteringMask.begin(), fData.filteringMask.end(), 1);


    //     for(int ii = 0; ii < settings.filters.erosionLoops; ++ii){
    //         std::for_each(std::execution::par_unseq, std::begin(fIndices.depths1DNoBorders), std::end(fIndices.depths1DNoBorders), [&](size_t id){

    //             if(fData.depthMask[id] == 0){
    //                 return;
    //             }

    //             float currDepth = fData.depth[id];
    //             auto hsv = Convert::to_hsv(fData.depthSizedColor[id]);

    //             for(auto cId : fIndices.neighbours4Depth1D[id]){

    //                 // if(fData.depth[cId] < currDepth){
    //                 //     continue;
    //                 // }

    //                 if((fData.depthMask[cId] == 0) || (abs(fData.depth[cId]-currDepth) > 500)){
    //                     // auto hsvDiffColor = Convert::to_hsv(fData.depthSizedColor[cId]);
    //                     // if((std::abs(hsv.h()- hsvDiffColor.h()) < settings.filters.maxDiffColor.x()) ||
    //                     // (std::abs(hsv.s()- hsvDiffColor.s()) < settings.filters.maxDiffColor.y()) ||
    //                     // (std::abs(hsv.v()- hsvDiffColor.v()) < settings.filters.maxDiffColor.z())){
    //                     //     fData.filteringMask[id] = 0;
    //                     //     return;
    //                     // }
    //                     fData.filteringMask[id] = 0;
    //                     return;
    //                 }
    //             }
    //         });

    //         for(size_t ii = 0; ii < fData.filteringMask.size(); ++ii){
    //             if(fData.filteringMask[ii] == 0){
    //                 fData.depthMask[ii] = 0;
    //             }
    //         }

    //     }
    // }




    // minimum neighbours filtering
    if(settings.filters.doMinNeighboursFiltering){
        erode(settings.filters.minNeighboursLoops, DCConnectivity::Connectivity_8, settings.filters.nbMinNeighbours);
    }

    // erosion
    if(settings.filters.doErosion){
        erode(settings.filters.erosionLoops, DCConnectivity::Connectivity_4, 4);
        // erode(settings.filters.erosionLoops, DCConnectivity::Connectivity_8, 8);
    }

    if(settings.filters.doLocalDiffFiltering){
        maximum_local_depth_difference(DCConnectivity::Connectivity_4, settings.filters.maxLocalDiff);
    }



    // keep only biggest cluster
    if(settings.filters.keepOnlyBiggestCluster){
        keep_only_biggest_cluster();
    }else{
        fData.meanBiggestZoneId = 0;
    }

    // remove after closest point
    if(settings.filters.removeAfterClosestPoint){

        std::uint16_t minDist = std::numeric_limits<std::uint16_t>::max();
        std::for_each(std::execution::unseq, std::begin(dIndices.depths1D), std::end(dIndices.depths1D), [&](size_t id){
            if(fData.depthMask[id] != 0){
                if(minDist > fData.depth[id]){
                    minDist = fData.depth[id];
                }
            }
        });

        std::uint16_t maxDist = minDist + static_cast<std::uint16_t>(1000.f * settings.filters.maxDistanceAfterClosestPoint);
        std::for_each(std::execution::par_unseq, std::begin(dIndices.depths1D), std::end(dIndices.depths1D), [&](size_t id){
            if(fData.depthMask[id] == 0){
                return;
            }

            if(fData.depth[id] > maxDist){
                fData.depthMask[id] = 0;
            }
        });
    }

    // Log::fmessage("[dco:{}]", Time::now_difference_micro_s(t1).count());
}

auto DCDeviceImpl::update_valid_depth_values() -> void{

    if(fData.depth.empty()){
        return;
    }

    auto t1 = Time::nanoseconds_since_epoch();

    // count valid depth values
    fData.validDepthValues = 0;

    std::for_each(std::execution::unseq, std::begin(dIndices.depths1D), std::end(dIndices.depths1D), [&](size_t id){
        if(fData.depthMask[id] == 0){
            fData.depth[id] = dc_invalid_depth_value;
            dIndices.depthVertexCorrrespondance[id] = {id, -1};
        }else{
            dIndices.depthVertexCorrrespondance[id] = {id, fData.validDepthValues};
            fData.validDepthValues++;
        }
    });

    // Log::fmessage("[uv:{}]", Time::now_difference_micro_s(t1).count());
}

auto DCDeviceImpl::filter_depth_sized_color_from_depth() -> void{

    auto t = TimeDiffGuard(timeM, "FILTER_DEPTH_SIZED_COLOR_FROM_DEPTH"sv);

    if(fData.depthSizedColor.empty() || fData.depth.empty()){
        return;
    }

    if(fData.depth.size() != fData.depthSizedColor.size()){
        return;
    }

    std::for_each(std::execution::par_unseq, std::begin(dIndices.depths1D), std::end(dIndices.depths1D), [&](size_t id){
        if(settings.filters.invalidateColorFromDepth){
            if(fData.depth[id] == dc_invalid_depth_value){
                fData.depthSizedColor[id] = ColorRGBA8{dc_invalid_color_value};
            }
            // else{
            //     if(settings.filters.keepOnlyBiggestCluster){
            //         // colorBuffer[meanBiggestZoneId] = ColorRGBA8{255,0,0,255};// TODO ?
            //     }
            // }
        }

        // if(fData.bodiesIdMap.size() == fData.depth.size()){
        //     if(fData.bodiesIdMap[id] != 255){ // K4ABT_BODY_INDEX_MAP_BACKGROUND
        //         fData.depthSizedColor[id] = ColorRGBA8{255,0,0,255};
        //     }
        // }
    });
}

auto DCDeviceImpl::filter_infra_from_depth() -> void{

    auto t = TimeDiffGuard(timeM, "FILTER_INFRA_FROM_DEPTH"sv);

    if(fData.infra.empty() || fData.depth.empty()){
        return;
    }

    if(fData.depth.size() != fData.infra.size()){
        return;
    }

    std::for_each(std::execution::par_unseq, std::begin(dIndices.depths1D), std::end(dIndices.depths1D), [&](size_t id){
        if(settings.filters.invalidateInfraFromDepth){
            if(fData.depth[id] == dc_invalid_depth_value){
                fData.infra[id] = dc_invalid_infra_value;
            }
        }

        // if(writeBodies){
        //     if(fData.bodiesIdMap[id] != 255){ // K4ABT_BODY_INDEX_MAP_BACKGROUND
        //         fData.infra[id] = 16000;
        //     }
        // }
    });
}


auto DCDeviceImpl::update_frame_original_size_color() -> void{

    auto t = TimeDiffGuard(timeM, "UPDATE_FRAME_COLOR"sv);
    if(frame == nullptr){
        return;
    }

    if(!fData.originalSizeColor.empty() && settings.data.generation.originalSizeColorImage){
        frame->insert_image_buffer<ColorRGBA8>(DCImageBufferType::OriginalColorRGBA8, mInfos.color_width(), mInfos.color_height(), fData.originalSizeColor);
    }
}

auto DCDeviceImpl::update_frame_depth_sized_color() -> void{

    auto t = TimeDiffGuard(timeM, "UPDATE_FRAME_DEPTH_SIZED_COLOR"sv);
    if(frame == nullptr){
        return;
    }

    if(!fData.depthSizedColor.empty() && settings.data.generation.depthSizedColorImage){
        frame->insert_image_buffer<ColorRGBA8>(DCImageBufferType::DepthSizedColorRGBA8, mInfos.depth_width(), mInfos.depth_height(), fData.depthSizedColor);
    }
}

auto DCDeviceImpl::update_frame_depth() -> void{

    auto t = TimeDiffGuard(timeM, "UPDATE_FRAME_DEPTH"sv);
    if(frame == nullptr){
        return;
    }

    if(fData.depth.empty()){
        return;
    }

    frame->insert_image_buffer<std::uint16_t>(DCImageBufferType::Depth16, mInfos.depth_width(), mInfos.depth_height(), fData.depth);

    if(settings.data.generation.depthImage){

        const auto dRange = mInfos.depth_range_mm();
        const auto diff   = dRange.y() - dRange.x();
        // static constexpr std::array<Pt3f,5> depthGradient ={
        //     Pt3f{0.f,0.f,1.f},
        //     {0.f,1.f,1.f},
        //     {0.f,1.f,0.f},
        //     {1.f,1.f,0.f},
        //     {1.f,0.f,0.f},
        // };
        static constexpr std::array<Pt3f,11> depthGradient ={
            Pt3f
            {0.f,0.f,1.f},
            {0.f,0.f,0.5f},
            {0.f,0.5f,0.f},
            {0.f,1.f,0.f},
            {0.5f,0.5f,0.f},
            {1.f,0.f,0.f},
            {1.f,0.5f,0.f},
            {1.f,1.f,0.f},
            {0.5f,1.f,0.f},
            {0.0f,1.f,0.5f},
            {0.0f,1.f,1.f},
        };

        auto rgbIBSpan = frame->insert_image_buffer<ColorRGB8>(DCImageBufferType::DepthRGB8, mInfos.depth_width(), mInfos.depth_height(), ColorRGB8(0,0,0));
        std::for_each(std::execution::par_unseq, std::begin(dIndices.depths1D), std::end(dIndices.depths1D), [&](size_t id){

            if(fData.depth[id] == dc_invalid_depth_value){
                return;
            }

            float vF = (static_cast<float>(fData.depth[id]) - dRange.x())/diff;
            float intPart;
            float decPart = std::modf((vF*(depthGradient.size()-1)), &intPart);
            size_t idG = static_cast<size_t>(intPart);

            auto col = depthGradient[idG]*(1.f-decPart) + depthGradient[idG+1]*decPart;
            rgbIBSpan[id] = ColorRGB8{
                static_cast<std::uint8_t>(255*col.x()),
                static_cast<std::uint8_t>(255*col.y()),
                static_cast<std::uint8_t>(255*col.z())
            };
        });
    }
}

auto DCDeviceImpl::update_frame_infra() -> void{

    auto t = TimeDiffGuard(timeM, "UPDATE_FRAME_INFRA"sv);
    if(frame == nullptr){
        return;
    }

    if(fData.infra.empty()){
        return;
    }

    frame->insert_image_buffer<std::uint16_t>(DCImageBufferType::Infrared16, mInfos.infra_width(), mInfos.infra_height(), fData.infra);

    if(settings.data.generation.infraImage){

        auto rgbIBSpan = frame->insert_image_buffer<ColorRGB8>(DCImageBufferType::InfraredRGB8, mInfos.infra_width(), mInfos.infra_height(), ColorRGB8(0,0,0));

        const float max = 2000;
        std::for_each(std::execution::par_unseq, std::begin(dIndices.depths1D), std::end(dIndices.depths1D), [&](size_t id){

            float vF = static_cast<float>(fData.infra[id]);
            if(vF > max){
                vF = max;
            }
            vF/=max;

            rgbIBSpan[id] = ColorRGB8{
                static_cast<std::uint8_t>(255*vF),
                static_cast<std::uint8_t>(255*vF),
                static_cast<std::uint8_t>(255*vF)
            };
        });
    }
}

auto DCDeviceImpl::update_frame_cloud() -> void{

    auto t = TimeDiffGuard(timeM, "UPDATE_FRAME_CLOUD"sv);
    if(frame == nullptr){
        return;
    }

    if(!fData.depth.empty() && !fData.depthCloud.empty() && settings.data.generation.cloud){

        auto volumeB = frame->insert_volume_buffer(DCVolumeBufferType::ColoredCloud, geo::ColorCloud{});
        volumeB->vertices.resize(fData.validDepthValues);
        volumeB->colors.resize(fData.validDepthValues);
        // volumeB->normals.resize(fData.validDepthValues);

        bool addColors = !fData.depthSizedColor.empty() && settings.data.generation.cloudColorMode == CloudColorMode::FromDepthSizedColorImage;
        const auto dRange = mInfos.depth_range_mm();
        const auto diff   = dRange.y() - dRange.x();
        static constexpr std::array<Pt3f,5> depthGradient ={
            Pt3f{0.f,0.f,1.f},
            {0.f,1.f,1.f},
            {0.f,1.f,0.f},
            {1.f,1.f,0.f},
            {1.f,0.f,0.f},
        };

        std::for_each(std::execution::par_unseq, std::begin(dIndices.depthVertexCorrrespondance), std::end(dIndices.depthVertexCorrrespondance), [&](auto idC){

            auto idD = std::get<0>(idC);
            if(fData.depth[idD] == dc_invalid_depth_value){
                return;
            }

            auto idV = std::get<1>(idC);

            Pt3f currentP = fData.depthCloud[idD].template conv<float>();
            volumeB->vertices[idV]     = currentP * 0.001f;
            volumeB->vertices[idV].x() = volumeB->vertices[idV].x();
            volumeB->vertices[idV].y() = volumeB->vertices[idV].y();

            if(addColors){
                volumeB->colors[idV] = (fData.depthSizedColor[idD].template conv<float>()/255.f).xyz();
            }else{

                float vF = (static_cast<float>(fData.depth[idD]) - dRange.x())/diff;
                float intPart;
                float decPart = std::modf((vF*(depthGradient.size()-1)), &intPart);
                size_t idG = static_cast<size_t>(intPart);
                volumeB->colors[idV] = depthGradient[idG]*(1.f-decPart) + depthGradient[idG+1]*decPart;
            }

            // A B C
            // D I E
            // F G H

            // 0 1 2
            // 3 X 4
            // 5 6 7
            // const auto &idN  = dIndices.neighbours8Depth1D[idD];
            // float cD = fData.depth[idD];
            // float dB = (idN[1] != -1) ? ((fData.depth[idN[1]] != dc_invalid_depth_value) ? fData.depth[idN[1]] : cD) : cD;
            // float dD = (idN[3] != -1) ? ((fData.depth[idN[3]] != dc_invalid_depth_value) ? fData.depth[idN[3]] : cD) : cD;
            // float dE = (idN[4] != -1) ? ((fData.depth[idN[4]] != dc_invalid_depth_value) ? fData.depth[idN[4]] : cD) : cD;
            // float dG = (idN[6] != -1) ? ((fData.depth[idN[6]] != dc_invalid_depth_value) ? fData.depth[idN[6]] : cD) : cD;

            // auto hV = normalize(Vec3f(dE-dD,0,-2));
            // auto vV = normalize(Vec3f(0,dG-dB,-2));
            // volumeB->normals[idV] = normalize(hV+vV);
        });
    }
}

auto DCDeviceImpl::update_frame_imu() -> void{

    auto t = TimeDiffGuard(timeM, "UPDATE_FRAME_IMU"sv);
    if(frame == nullptr){
        return;
    }

    if(!fData.binaryIMU.empty() && settings.data.generation.imu){
        frame->insert_data_buffer(DCDataBufferType::IMU, fData.binaryIMU);
    }
}

auto DCDeviceImpl::update_frame_body_tracking() -> void{

    auto t = TimeDiffGuard(timeM, "UPDATE_FRAME_BODIES"sv);
    if(frame == nullptr){
        return;
    }

    // skeleton
    if(!fData.bodies.empty() && settings.data.generation.bodiesSkeleton){
        frame->insert_data_buffer(DCDataBufferType::BodiesSkeleton, std::span<std::byte>(reinterpret_cast<std::byte*>(fData.bodies.data()), fData.bodies.size_bytes()));
    }

    // bodies id map
    if(!fData.bodiesId.empty()){// && (fData.bodiesId.size() == fData.depth.size())){

        if(settings.data.generation.bodiesIdImage){

            static constexpr std::array<ColorRGB8,10> bodiesColor ={
                ColorRGB8
                {255,0,0},
                {0,255,0},
                {0,0,255},
                {255,255,0},
                {255,0,255},
                {0,255,255},
                {122,122,0},
                {122,0,122},
                {0,122,122},
                {255,255,255}
            };

            frame->insert_image_buffer<ColorGray8>(DCImageBufferType::BodiesIdMap8, mInfos.depth_width(), mInfos.depth_height(), fData.bodiesId);

            auto rgbIBSpan = frame->insert_image_buffer<ColorRGB8>(DCImageBufferType::BodiesIdMapRGB8, mInfos.depth_width(), mInfos.depth_height(), ColorRGB8(0,0,0));
            std::for_each(std::execution::par_unseq, std::begin(dIndices.depths1D), std::end(dIndices.depths1D), [&](size_t id){
                if(fData.bodiesId[id] != 255){ // K4ABT_BODY_INDEX_MAP_BACKGROUND
                    rgbIBSpan[id] = bodiesColor[fData.bodiesId[id]%10];
                }
            });
        }
    }


}

auto DCDeviceImpl::update_frame_calibration() -> void{

    auto t = TimeDiffGuard(timeM, "UPDATE_FRAME_CALIBRATION"sv);
    if(frame == nullptr){
        return;
    }

    if(!fData.binaryCalibration.empty()){
        frame->insert_data_buffer(DCDataBufferType::Calibration, fData.binaryCalibration);
    }
}


auto DCDeviceImpl::update_frame_audio() -> void{

    auto t = TimeDiffGuard(timeM, "UPDATE_FRAME_AUDIO"sv);
    if(frame == nullptr){
        return;
    }

    if(settings.data.generation.audio && (fData.audioChannels.first != 0) && !fData.audioChannels.second.empty()){
        // size_t nbFrames = fData.audioChannels.second.size() / fData.audioChannels.first;
        // frame->audioFrames.resize(nbFrames);
        // auto audioFrom = fData.audioChannels.second.data();
        // std::copy(audioFrom, audioFrom + fData.audioChannels.second.size(), reinterpret_cast<float*>(frame->audioFrames.data()));
    }
}

auto DCDeviceImpl::update_data_frame_color() -> void{

    auto t = TimeDiffGuard(timeM, "UPDATE_DATA_FRAME_COLOR"sv);
    if(dFrame == nullptr){
        return;
    }

    if(!fData.originalSizeColor.empty() && settings.data.sending.addOriginalSizeColor){
        if(settings.data.sending.originalSizeColorCM == DCCompressionMode::JPEG){
            
            if(auto imageB = dFrame->insert_image_buffer(DCImageBufferType::OriginalColorRGBA8, DCCompressionMode::JPEG)){
                if(!jpegColorEncoder.encode(
                        mInfos.color_width(),
                        mInfos.color_height(),
                        fData.originalSizeColor,
                        *imageB,
                        settings.data.sending.originalSizeColorJPEGCQ
                    )){
                    dFrame->imagesB.erase(DCImageBufferType::OriginalColorRGBA8);
                }
            }

        }else if(settings.data.sending.originalSizeColorCM == DCCompressionMode::None){
            
            dFrame->insert_image_buffer(DCImageBufferType::OriginalColorRGBA8, DCCompressionMode::None,
                mInfos.color_width(), mInfos.color_height(), 4,
                std::span<std::byte>(reinterpret_cast<std::byte*>(fData.originalSizeColor.data()), fData.originalSizeColor.size_bytes()));
        }
    }
}

auto DCDeviceImpl::update_data_frame_depth_sized_color() -> void{

    auto t = TimeDiffGuard(timeM, "UPDATE_DATA_FRAME_DEPH_SIZED_COLOR"sv);
    if(dFrame == nullptr){
        return;
    }

    if(!fData.depthSizedColor.empty() && settings.data.sending.addDepthSizedColor){

        if(settings.data.sending.depthSizedColorCM == DCCompressionMode::JPEG){
            
            if(auto imageB = dFrame->insert_image_buffer(DCImageBufferType::DepthSizedColorRGBA8, DCCompressionMode::JPEG)){
                if(!jpegDepthSizedColorEncoder.encode(
                        mInfos.depth_width(),
                        mInfos.depth_height(),
                        fData.depthSizedColor,
                        *imageB,
                        settings.data.sending.depthSizedColorJPEGCQ
                    )){
                    dFrame->imagesB.erase(DCImageBufferType::DepthSizedColorRGBA8);
                }
            }

        }else if(settings.data.sending.depthSizedColorCM == DCCompressionMode::None){
            
            dFrame->insert_image_buffer(DCImageBufferType::DepthSizedColorRGBA8, DCCompressionMode::None,
                mInfos.depth_width(), mInfos.depth_height(), 4,
                std::span<std::byte>(reinterpret_cast<std::byte*>(fData.depthSizedColor.data()), fData.depthSizedColor.size_bytes()));
        }
    }
}

auto DCDeviceImpl::update_data_frame_depth() -> void{

    auto t = TimeDiffGuard(timeM, "UPDATE_DATA_FRAME_DEPTH"sv);
    if(dFrame == nullptr){
        return;
    }

    if(!fData.depth.empty() && settings.data.sending.addDepth){

        if(settings.data.sending.depthCM == DCCompressionMode::FastPFor){

            if(auto imageB = dFrame->insert_image_buffer(DCImageBufferType::Depth16, DCCompressionMode::FastPFor)){
                if(!fastPForDepthEncoder.encode(
                    mInfos.depth_width(),
                    mInfos.depth_height(),
                    fData.depth,
                    *imageB
                )){
                    dFrame->imagesB.erase(DCImageBufferType::Depth16);
                }
            }

        }else if(settings.data.sending.depthCM == DCCompressionMode::None){

            dFrame->insert_image_buffer(DCImageBufferType::Depth16, DCCompressionMode::None,
                mInfos.depth_width(), mInfos.depth_height(), 2,
                std::span<std::byte>(reinterpret_cast<std::byte*>(fData.depth.data()), fData.depth.size_bytes()));
        }
    }
}

auto DCDeviceImpl::update_data_frame_infra() -> void{

    auto t = TimeDiffGuard(timeM, "UPDATE_DATA_FRAME_INFRA"sv);
    if(dFrame == nullptr){
        return;
    }

    if(!fData.infra.empty() && settings.data.sending.addInfra){

        if(settings.data.sending.infraCM == DCCompressionMode::FastPFor){

            if(auto imageB = dFrame->insert_image_buffer(DCImageBufferType::Infrared16, DCCompressionMode::FastPFor)){
                if(!fastPForInfraEncoder.encode(
                        mInfos.infra_width(),
                        mInfos.infra_height(),
                        fData.infra,
                        *imageB
                    )){
                    dFrame->imagesB.erase(DCImageBufferType::Infrared16);
                }
            }

        }else if(settings.data.sending.infraCM == DCCompressionMode::None){

            dFrame->insert_image_buffer(DCImageBufferType::Infrared16, DCCompressionMode::None,
                mInfos.infra_width(), mInfos.infra_height(), 2,
                std::span<std::byte>(reinterpret_cast<std::byte*>(fData.infra.data()), fData.infra.size_bytes()));
        }
    }
}

auto DCDeviceImpl::update_data_frame_body_tracking() -> void{

    auto t = TimeDiffGuard(timeM, "UPDATE_DATA_FRAME_BODIES"sv);
    if(dFrame == nullptr){
        return;
    }

    // skeleon
    if(!fData.bodies.empty() && settings.data.sending.addBodiesSkeleton){
        dFrame->insert_data_buffer(DCDataBufferType::BodiesSkeleton, DCCompressionMode::None,
            std::span<std::byte>(reinterpret_cast<std::byte*>(fData.bodies.data()), fData.bodies.size_bytes()));
    }

    // bodies id map
    if(!fData.bodiesId.empty() && settings.data.sending.addBodiesId){

        dFrame->insert_image_buffer(DCImageBufferType::BodiesIdMap8, DCCompressionMode::None,
            mInfos.depth_width(), mInfos.depth_height(), 1,
            std::span<std::byte>(reinterpret_cast<std::byte*>(fData.bodiesId.data()), fData.bodiesId.size_bytes()));
    }
}


auto DCDeviceImpl::update_data_frame_calibration() -> void{

    auto t = TimeDiffGuard(timeM, "UPDATE_DATA_FRAME_CALIBRATION"sv);
    if(dFrame == nullptr){
        return;
    }

    if(!fData.binaryCalibration.empty()){        
        dFrame->insert_data_buffer(DCDataBufferType::Calibration, DCCompressionMode::None, fData.binaryCalibration);
    }
}

auto DCDeviceImpl::update_data_frame_cloud() -> void{

    auto t = TimeDiffGuard(timeM, "UPDATE_DATA_FRAME_CLOUD"sv);
    if(dFrame == nullptr){
        return;
    }

    if(!fData.depthCloud.empty() && !fData.depth.empty() && settings.data.sending.addCloud){

        if(settings.data.sending.cloudCM == DCCompressionMode::FastPFor){


            // auto [minX, maxX] = std::minmax_element(dIndices.depthVertexCorrrespondance.cbegin(), dIndices.depthVertexCorrrespondance.cend(), [](const Pt3f &a, const Pt3f &b){
            //     return a.x() < b.x();
            // });
            // auto [minY, maxY] = std::minmax_element(dIndices.depthVertexCorrrespondance.cbegin(), dIndices.depthVertexCorrrespondance.cend(), [](const Pt3f &a, const Pt3f &b){
            //     return a.y() < b.y();
            // });
            // auto [minZ, maxZ] = std::minmax_element(dIndices.depthVertexCorrrespondance.cbegin(), dIndices.depthVertexCorrrespondance.cend(), [](const Pt3f &a, const Pt3f &b){
            //     return a.z() < b.z();
            // });

            // // x y z r g b
            // // retrieve origin
            // std::for_each(std::execution::par_unseq, std::begin(dIndices.depthVertexCorrrespondance), std::end(dIndices.depthVertexCorrrespondance), [&](const auto &idC){

            //     auto idD = std::get<0>(idC);
            //     if(fData.depth[idD] == dc_invalid_depth_value){
            //         return;
            //     }

            //     Pt3f currentP = fData.depthCloud[idD].template conv<float>();
            // });

            // Pt3f currentP = fData.depthCloud[idD].template conv<float>();
            // frame->cloud.vertices[idV] = currentP * 0.001f;

            // if(addColors){
            //     frame->cloud.colors[idV] = (fData.depthSizedColor[idD].template conv<float>()/255.f).xyz();
            // fData.depthCloud;


            Buffer<std::uint16_t> processedCloudData;

            // resize cloud data
            // data mapping: XXYYZZRGB0
            size_t cloudVerticesBufferSize              = fData.validDepthValues*5;
            size_t rest                                 = cloudVerticesBufferSize % 128;
            size_t paddeCloudVerticesBufferPaddedSize   = rest == 0 ? cloudVerticesBufferSize : (cloudVerticesBufferSize + 128 - rest);
            if(processedCloudData.size() < paddeCloudVerticesBufferPaddedSize){
                processedCloudData.resize(paddeCloudVerticesBufferPaddedSize);
            }
            std::fill(processedCloudData.begin(), processedCloudData.end(), 0);

            auto processedCloudData8 = reinterpret_cast<std::uint8_t*>(processedCloudData.get_data());

            bool addColors = !fData.depthSizedColor.empty() && settings.data.sending.cloudColorMode == CloudColorMode::FromDepthSizedColorImage;

            const auto dRange = mInfos.depth_range_mm();
            const auto diff   = dRange.y() - dRange.x();
            static constexpr std::array<Pt3f,5> depthGradient ={
                Pt3f{0.f,0.f,1.f},
                {0.f,1.f,1.f},
                {0.f,1.f,0.f},
                {1.f,1.f,0.f},
                {1.f,0.f,0.f},
            };

            // fill data
            std::for_each(std::execution::par_unseq, std::begin(dIndices.depthVertexCorrrespondance), std::end(dIndices.depthVertexCorrrespondance), [&](const auto &idC){

                auto idD = std::get<0>(idC);
                if(fData.depth[idD] == dc_invalid_depth_value){
                    return;
                }
                auto idV = std::get<1>(idC);

                processedCloudData[idV]                             = static_cast<std::uint16_t>(static_cast<std::int32_t>(fData.depthCloud[idV].x())+4096); // X
                processedCloudData[fData.validDepthValues   + idV]  = static_cast<std::uint16_t>(static_cast<std::int32_t>(fData.depthCloud[idV].y())+4096); // Y
                processedCloudData[2*fData.validDepthValues + idV]  = static_cast<std::uint16_t>(fData.depthCloud[idV].z());                                 // Z
                if(addColors){
                    processedCloudData8[6*fData.validDepthValues + idV] = fData.depthSizedColor[idV].r();
                    processedCloudData8[7*fData.validDepthValues + idV] = fData.depthSizedColor[idV].g();
                    processedCloudData8[8*fData.validDepthValues + idV] = fData.depthSizedColor[idV].b();
                }else{
                    float vF = (static_cast<float>(fData.depth[idD]) - dRange.x())/diff;
                    float intPart;
                    float decPart = std::modf((vF*(depthGradient.size()-1)), &intPart);
                    size_t idG = static_cast<size_t>(intPart);
                    auto colorF = depthGradient[idG]*(1.f-decPart) + depthGradient[idG+1]*decPart;
                    processedCloudData8[6*fData.validDepthValues + idV] = static_cast<std::uint8_t>(255.f * colorF.x());
                    processedCloudData8[7*fData.validDepthValues + idV] = static_cast<std::uint8_t>(255.f * colorF.y());
                    processedCloudData8[8*fData.validDepthValues + idV] = static_cast<std::uint8_t>(255.f * colorF.z());
                }
            });

            if(auto volumeB = dFrame->insert_volume_buffer(DCVolumeBufferType::CloudXYZ16RGB8, DCCompressionMode::FastPFor)){
                if(!fastPForCloudEncoder.encode(processedCloudData.byte_span(), *volumeB)){
                    dFrame->volumesB.erase(DCVolumeBufferType::CloudXYZ16RGB8);
                }
            }
        }else if(settings.data.sending.cloudCM == DCCompressionMode::None){
            // ...
        }
    }
}

auto DCDeviceImpl::update_data_frame_audio() -> void{

    auto t = TimeDiffGuard(timeM, "UPDATE_DATA_FRAME_AUDIO"sv);
    if(dFrame == nullptr){
        return;
    }

    // if((fData.audioChannels.first != 0) && !fData.audioChannels.second.empty() && settings.data.compression.addAudio){

    //     size_t audioSize = fData.audioChannels.second.size() / fData.audioChannels.first;
    //     cFrame->audioFrames.resize(audioSize);
    //     std::copy(
    //         fData.audioChannels.second.begin(), fData.audioChannels.second.end(),
    //         reinterpret_cast<float*>(cFrame->audioFrames.data())
    //     );

    // }else{
    //     cFrame->audioFrames = {};
    // }
}

auto DCDeviceImpl::update_data_frame_imu() -> void{

    auto t = TimeDiffGuard(timeM, "UPDATE_DATA_FRAME_IMU"sv);
    if(dFrame == nullptr){
        return;
    }

    if(!fData.binaryIMU.empty() && settings.data.sending.addImu){
        dFrame->insert_data_buffer(DCDataBufferType::IMU, DCCompressionMode::None, fData.binaryIMU);
    }
}

auto DCDeviceImpl::check_data_validity() -> bool {

    if(mInfos.has_color() && settings.data.capture.color){
        if(fData.rawColor.empty()){
            Log::log("Invalid raw color, frame canceled.\n");
            return false;
        }
    }

    if(mInfos.has_depth() && settings.data.capture.depth){
        if(fData.depth.empty()){
            Log::log("Invalid depth, frame canceled.\n");
            return false;
        }
    }

    if(mInfos.has_infra() && settings.data.capture.infra){
        if(fData.infra.empty()){
            Log::log("Invalid infra, frame canceled.\n");
            return false;
        }
    }

    return true;
}

auto DCDeviceImpl::maximum_local_depth_difference(DCConnectivity connectivity, float max) -> void{

    // auto t1 = Time::nanoseconds_since_epoch();

    if(connectivity == DCConnectivity::Connectivity_4){

        std::for_each(std::execution::par_unseq, std::begin(dIndices.depths1DNoBorders), std::end(dIndices.depths1DNoBorders), [&](const auto &id){

            float currDepth = fData.depth[id];
            float meanDiff = 0.f;
            std::uint8_t count = 0;
            for(const auto &cId : dIndices.neighbours4Depth1D[id]){
                if(fData.depthMask[cId] == 1){
                    meanDiff += abs(fData.depth[cId]-currDepth);
                    ++count;
                }
            }
            fData.filteringMask[id] = (count == 0) ? 0 : ((1.*meanDiff/count < max) ? 1 : 0);
        });

    }else if(connectivity == DCConnectivity::Connectivity_8){

        std::for_each(std::execution::par_unseq, std::begin(dIndices.depths1DNoBorders), std::end(dIndices.depths1DNoBorders), [&](const auto &id){

            float currDepth = fData.depth[id];
            float meanDiff = 0.f;
            std::uint8_t count = 0;
            for(const auto &cId : dIndices.neighbours8Depth1D[id]){
                if(fData.depthMask[cId] == 1){
                    meanDiff += abs(fData.depth[cId]-currDepth);
                    ++count;
                }
            }
            fData.filteringMask[id] = (count == 0) ? 0 : ((1.*meanDiff/count < max) ? 1 : 0);
        });

    }else if(connectivity == DCConnectivity::Connectivity_2H){

        std::for_each(std::execution::par_unseq, std::begin(dIndices.depths1DNoBorders), std::end(dIndices.depths1DNoBorders), [&](const auto &id){

            float currDepth = fData.depth[id];
            float meanDiff = 0.f;
            std::uint8_t count = 0;
            for(const auto &cId : dIndices.neighbours2HDepth1D[id]){
                if(fData.depthMask[cId] == 1){
                    meanDiff += abs(fData.depth[cId]-currDepth);
                    ++count;
                }
            }
            fData.filteringMask[id] = (count == 0) ? 0 : ((1.*meanDiff/count < max) ? 1 : 0);
        });


    }else if(connectivity == DCConnectivity::Connectivity_2V){

        std::for_each(std::execution::par_unseq, std::begin(dIndices.depths1DNoBorders), std::end(dIndices.depths1DNoBorders), [&](const auto &id){

            float currDepth = fData.depth[id];
            float meanDiff = 0.f;
            std::uint8_t count = 0;
            for(const auto &cId : dIndices.neighbours2VDepth1D[id]){
                if(fData.depthMask[cId] == 1){
                    meanDiff += abs(fData.depth[cId]-currDepth);
                    ++count;
                }
            }
            fData.filteringMask[id] = (count == 0) ? 0 : ((1.*meanDiff/count < max) ? 1 : 0);
        });
    }

    std::for_each(std::execution::par_unseq, std::begin(dIndices.depths1DNoBorders), std::end(dIndices.depths1DNoBorders), [&](size_t id){
        if(fData.filteringMask[id] == 0){
            fData.depthMask[id] = 0;
        }
    });

    // v.add_value(Time::now_difference_micro_s(t1).count());
    // Log::fmessage("maximum_local_depth_difference time {}\n", v.get());
}

auto DCDeviceImpl::keep_only_biggest_cluster() -> void{

    // empty zones
    std::fill(fData.zonesId.begin(), fData.zonesId.end(), 0);

    currentZoneId = 1;
    biggestZone = -1;
    sizeBiggestZone = 0;

    auto depthWidth  = mInfos.depth_width();
    // auto depthHeight = mInfos.depth_height();
    auto depthSize   = mInfos.depth_size();

    std::for_each(std::execution::unseq, std::begin(dIndices.depths1D), std::end(dIndices.depths1D), [&](size_t pt){

        if(fData.zonesId[pt] != 0){
            return;
        }

        if(fData.depthMask[pt] == 0){
            return;
        }

        size_t zoneId = currentZoneId;
        std::queue<size_t> ids;
        ids.push(pt);

        // count new zone
        size_t count = 0;
        while(ids.size() > 0){
            size_t id = ids.front();
            ids.pop();

            if(fData.zonesId[id] != 0){
                continue;
            }

            if(fData.depthMask[id] == 0){
                continue;
            }

            fData.zonesId[id] = static_cast<int>(zoneId);
            count++;

            if(id >= 1){
                ids.push(id-1);

                if(id >= depthWidth-1){

                    ids.push(id-depthWidth+1);

                    if(id >= depthWidth){

                        ids.push(id-depthWidth);

                        if(id >= depthWidth + 1){
                            ids.push(id-depthWidth-1);
                        }
                    }
                }
            }

            if(id < depthSize-1){

                ids.push(id+1);

                if(id < depthSize - depthWidth+1){
                    ids.push(id+depthWidth-1);

                    if(id < depthSize -depthWidth){
                        ids.push(id+depthWidth);

                        if(id < depthSize -depthWidth-1){
                            ids.push(id+depthWidth+1);
                        }
                    }
                }
            }
        }

        if(count != 0){
            if(count > sizeBiggestZone){
                sizeBiggestZone = count;
                biggestZone = static_cast<int>(currentZoneId);
            }
            currentZoneId++;
        }
    });

    if(biggestZone != -1){
        Pt2<size_t> meanBiggestZone = {0,0};
        size_t count = 0;
        std::for_each(std::execution::unseq, std::begin(dIndices.depths3D), std::end(dIndices.depths3D), [&](const auto &pt){
            if(fData.zonesId[pt.x()] != biggestZone){
                fData.depthMask[pt.x()] = 0;
            }

            if(fData.depthMask[pt.x()] == 1){
                meanBiggestZone.x() += pt.y();
                meanBiggestZone.y() += pt.z();
                ++count;
            }
        });

        meanBiggestZone /= count;
        fData.meanBiggestZoneId = meanBiggestZone.y() * depthWidth + meanBiggestZone.x();
    }

}

// auto DCDeviceImpl::mininum_neighbours(uint8_t nbLoops, uint8_t nbMinNeighbours, DCConnectivity connectivity) -> void{

//     auto t1 = Time::nanoseconds_since_epoch();

//     for(size_t numLoop = 0; numLoop < nbLoops; ++numLoop){

//         if(connectivity == DCConnectivity::Connectivity_4){

//             // std::for_each(std::execution::par_unseq, std::begin(dIndices.depths1DNoBorders), std::end(dIndices.depths1DNoBorders), [&](const auto &id){
//             //     fData.filteringMask[id] = (fData.depthMask[id] != 0) ? (std::accumulate(dIndices.neighbours4Depth1D[id].begin(), dIndices.neighbours4Depth1D[id].end(), 0, [&](int total, size_t cId){
//             //                                                                 return total + fData.depthMask[cId];
//             //                                                             }) >= nbMinValid ? 1 : 0) : 0;
//             // });
//         }else{
//             // std::for_each(std::execution::par_unseq, std::begin(dIndices.depths1DNoBorders), std::end(dIndices.depths1DNoBorders), [&](const auto &id){
//             //     fData.filteringMask[id] = (fData.depthMask[id] != 0) ? (std::accumulate(dIndices.neighbours8Depth1D[id].begin(), dIndices.neighbours8Depth1D[id].end(), 0, [&](int total, size_t cId){
//             //                                                                 return total + fData.depthMask[cId];
//             //                                                             }) >= nbMinValid ? 1 : 0) : 0;
//             // });
//         }

//         std::for_each(std::execution::par_unseq, std::begin(dIndices.depths1DNoBorders), std::end(dIndices.depths1DNoBorders), [&](size_t id){
//             if(fData.filteringMask[id] == 0){
//                 fData.depthMask[id] = 0;
//             }
//         });
//     }

//     for(std::uint8_t numLoop = 0; numLoop < nbLoops; ++numLoop){

//         // reset filtering mask
//         std::fill(fData.filteringMask.begin(), fData.filteringMask.end(), 0);

//         std::for_each(std::execution::par_unseq, std::begin(dIndices.depths1DNoBorders), std::end(dIndices.depths1DNoBorders), [&](size_t id){

//             if(fData.depthMask[id] == 0){
//                 return;
//             }

//             std::uint8_t count = 0;
//             if(connectivity == DCConnectivity::Connectivity_4){
//                 for(auto cId : dIndices.neighbours4Depth1D[id]){

//                     if(fData.depthMask[cId] == 1){
//                         ++count;
//                     }
//                 }
//                 // fData.filteringMask[id] = count == 4 ? 1 : 0;
//             }else{
//                 for(auto cId : dIndices.neighbours8Depth1D[id]){

//                     if(fData.depthMask[cId] == 1){
//                         ++count;
//                     }
//                 }
//                 // fData.filteringMask[id] = count == 8 ? 1 : 0;
//             }

//             fData.filteringMask[id] = (count < nbMinNeighbours) ? 1 : 0;
//         });

//         for(size_t ii = 0; ii < fData.filteringMask.size(); ++ii){
//             if(fData.filteringMask[ii] == 1){
//                 fData.depthMask[ii] = 0;
//             }
//         }
//     }

//     v.add_value(Time::now_difference_micro_s(t1).count());
//     // Log::fmessage("Erode time {}\n", Time::now_difference_micro_s(t1));
//     Log::fmessage("Erode time {}\n", v.get());

// }

auto DCDeviceImpl::erode(uint8_t nbLoops, DCConnectivity connectivity, std::uint8_t nbMinValid) -> void{

    for(size_t numLoop = 0; numLoop < nbLoops; ++numLoop){

        if(connectivity == DCConnectivity::Connectivity_4){

            std::for_each(std::execution::par_unseq, std::begin(dIndices.depths1DNoBorders), std::end(dIndices.depths1DNoBorders), [&](const auto &id){
                fData.filteringMask[id] = (fData.depthMask[id] != 0) ? (std::accumulate(dIndices.neighbours4Depth1D[id].begin(), dIndices.neighbours4Depth1D[id].end(), 0, [&](int total, size_t cId){
                    return total + fData.depthMask[cId];
                }) >= nbMinValid ? 1 : 0) : 0;
            });
        }else{
            std::for_each(std::execution::par_unseq, std::begin(dIndices.depths1DNoBorders), std::end(dIndices.depths1DNoBorders), [&](const auto &id){
                fData.filteringMask[id] = (fData.depthMask[id] != 0) ? (std::accumulate(dIndices.neighbours8Depth1D[id].begin(), dIndices.neighbours8Depth1D[id].end(), 0, [&](int total, size_t cId){
                    return total + fData.depthMask[cId];
                }) >= nbMinValid ? 1 : 0) : 0;
            });
        }

        std::for_each(std::execution::par_unseq, std::begin(dIndices.depths1DNoBorders), std::end(dIndices.depths1DNoBorders), [&](size_t id){
            if(fData.filteringMask[id] == 0){
                fData.depthMask[id] = 0;
            }
        });
    }
}

auto DCDeviceImpl::set_data_settings(const DCDeviceDataSettings &dataS) -> void {
    auto lg = LogG("DCDeviceImpl::set_data_settings"sv);
    settings.data = dataS;
}

auto DCDeviceImpl::set_filters_settings(const DCFiltersSettings &filtersS) -> void{
    auto lg = LogG("DCDeviceImpl::set_filters_settings"sv);
    settings.filters = filtersS;
}

auto DCDeviceImpl::set_color_settings(const DCColorSettings &colorS) -> void{
    auto lg = LogG("DCDeviceImpl::set_color_settings"sv);
    settings.color = colorS;
}

auto DCDeviceImpl::set_misc_settings(const DCMiscSettings &miscS) -> void{
    auto lg = LogG("DCDeviceImpl::set_delay_settings"sv);
    settings.misc = miscS;
}

auto DCDeviceImpl::get_duration_ms(std::string_view id) -> std::optional<std::chrono::milliseconds>{
    std::unique_lock<std::mutex> lock(timesLocker, std::try_to_lock);
    if(!lock.owns_lock()){
        return std::nullopt;
    }
    if(times.contains(id)){
        return tool::Time::to_ms(times[id].lastDiff);
    }
    return std::nullopt;
}

auto DCDeviceImpl::get_duration_micro_s(std::string_view id) -> std::optional<std::chrono::microseconds>{
    std::unique_lock<std::mutex> lock(timesLocker, std::try_to_lock);
    if(!lock.owns_lock()){
        return std::nullopt;
    }
    if(times.contains(id)){
        return tool::Time::to_micro_s(times[id].lastDiff);
    }
    return std::nullopt;
}

auto DCDeviceImpl::get_average_framerate() -> float{
    std::unique_lock<std::mutex> lock(timesLocker, std::try_to_lock);
    if(!lock.owns_lock()){
        return 0.f;
    }
    return framerateB.get_framerate();
}

auto DCDeviceImpl::read_frame() -> bool{

    auto tRF = TimeDiffGuard(timeM, "READ_FRAME"sv);

    fData.reset_spans();
    {
        auto tCF = TimeDiffGuard(timeM, "CAPTURE_FRAME"sv);

        auto t1 = Time::nanoseconds_since_epoch();
        captureSuccess = capture_frame(mInfos.timeout_ms());
        // Log::fmessage("C[{}] [{}] \n", captureSuccess, Time::now_difference_micro_s(t1));
        if(!captureSuccess){
            dataIsValid = false;
            return false;
        }
    }

    auto tRI = TimeDiffGuard(timeM, "READ_IMAGES"sv);
    {
        auto tRCI = TimeDiffGuard(timeM, "READ_COLOR_IMAGE"sv);
        read_color_image(mInfos.has_color() && settings.data.capture.color);
        // std::cout << "COLOR SIZE: " << fData.rawColor.size() << " " << mInfos.has_color()<< " " << settings.data.capture.color << "\n";
    }
    {
        auto tRDI = TimeDiffGuard(timeM, "READ_DEPTH_IMAGE"sv);
        read_depth_image(mInfos.has_depth() && settings.data.capture.depth);
        // std::cout << "DEPTH SIZE: " << fData.depth.size() <<  " " << mInfos.has_depth()<< " " << settings.data.capture.depth << "\n";
    }
    {
        auto tRII = TimeDiffGuard(timeM, "READ_INFRA_IMAGE"sv);
        read_infra_image(mInfos.has_infra() && settings.data.capture.infra);
        // std::cout << "INFRA SIZE: " << fData.infra.size() << " " << mInfos.has_infra()<< " " << settings.data.capture.infra << "\n";
    }
    {
        auto tBT = TimeDiffGuard(timeM, "READ_BODY_TRACKING"sv);
        read_body_tracking(settings.data.capture.bodyTracking && settings.config.btEnabled && mInfos.has_depth());        
    }
    {
        auto tRA = TimeDiffGuard(timeM, "READ_AUDIO"sv);
        read_audio(settings.data.capture.audio && mInfos.has_audio());
    }
    {
        auto tRI = TimeDiffGuard(timeM, "READ_IMU"sv);
        read_IMU(settings.data.capture.imu);
    }

    release_frame();

    dataIsValid = check_data_validity();
    return dataIsValid;
}


auto DCDeviceImpl::process_data() -> std::tuple<std::shared_ptr<DCFrame>, std::shared_ptr<DCDataFrame>> {

    auto tPD = TimeDiffGuard(timeM, "PROCESSING_DATA"sv);
    frame   = nullptr;
    dFrame  = nullptr;

    if(captureSuccess && dataIsValid){

        framerateB.add_frame();

        if(settings.data.generation.has_data()){
            frame = std::make_shared<DCFrame>();
        }

        if(settings.data.sending.has_data()){
            dFrame = std::make_shared<DCDataFrame>();
        }

        {
            auto tCCI = TimeDiffGuard(timeM, "CONVERT_COLOR_IMAGE"sv);
            convert_color_image();
        }
        {
            auto tRCI = TimeDiffGuard(timeM, "RESIZE_IMAGES"sv);
            resize_images();
        }
        // return {nullptr,nullptr};
        {
            auto tGC = TimeDiffGuard(timeM, "GENERATE_CLOUD"sv);
            generate_cloud(settings.data.capture_cloud() || settings.filters.filterDepthWithCloud);
        }
        {
            // auto tPP = TimeDiffGuard(timeM, "PREPROCESS"sv);
            // preprocess_color_image();
            // preprocess_depth_sized_color_image();
            // preprocess_depth_image();
            // preprocess_infra_image();
            // preprocess_cloud_image();
            // preprocess_body_tracking_image();
        }

        {
            auto tFD = TimeDiffGuard(timeM, "FILTER"sv);
            // simd_filter();
            filter_depth_basic();
            // filter_depth_from_depth_sized_color();
            // filter_depth_from_infra();
            filter_depth_from_cloud();
            // filter_depth_from_body_tracking();
            filter_depth_complex();
            update_valid_depth_values();

            filter_depth_sized_color_from_depth();
            // mix_depth_sized_color_with_body_tracking();
            filter_infra_from_depth();
            // mix_infra_with_body_tracking();
        }

        {
            auto tUCF = TimeDiffGuard(timeM, "UPDATE_DATA_FRAME"sv);
            update_data_frame_color();
            update_data_frame_depth_sized_color();
            update_data_frame_depth();
            update_data_frame_infra();
            update_data_frame_cloud();
            update_data_frame_audio();
            update_data_frame_imu();
            update_data_frame_body_tracking();
            update_data_frame_calibration();
        }

        {
            auto tFCF = TimeDiffGuard(timeM, "FINALIZE_DATA_FRAME"sv);
            if(dFrame != nullptr){
                // set infos
                dFrame->idDevice           = static_cast<std::uint8_t>(settings.config.idDevice);
                dFrame->idCapture          = static_cast<std::int32_t>(mInfos.id_capture());
                dFrame->afterCaptureTS     = timeM.get_end("CAPTURE_FRAME"sv).count();
                dFrame->receivedTS         = dFrame->afterCaptureTS; // default init received TS with after capture TS
                dFrame->mode               = settings.config.mode;
                dFrame->validVerticesCount = fData.validDepthValues;

                // add frame
                frames.add_data_frame(std::move(dFrame));
            }
        }

        {
            auto tSCF = TimeDiffGuard(timeM, "SEND_DATA_FRAME"sv);
            if(auto dataFrameToSend = frames.get_data_frame_with_delay(timeM.get_end("CAPTURE_FRAME"sv), settings.misc.delayMs)){
                dFrame =dataFrameToSend;
                new_data_frame_signal(std::move(dataFrameToSend));
            }
        }

        {
            auto tUF = TimeDiffGuard(timeM, "UPDATE_FRAME"sv);

            // auto t1 = Time::nanoseconds_since_epoch();
            update_frame_original_size_color();
            // auto t2 = Time::nanoseconds_since_epoch();
            update_frame_depth_sized_color();
            // auto t3 = Time::nanoseconds_since_epoch();
            update_frame_depth();
            // auto t4 = Time::nanoseconds_since_epoch();
            update_frame_infra();
            // auto t5 = Time::nanoseconds_since_epoch();
            update_frame_cloud();
            // auto t6 = Time::nanoseconds_since_epoch();
            update_frame_audio();
            // auto t7 = Time::nanoseconds_since_epoch();
            update_frame_imu();
            // auto t8 = Time::nanoseconds_since_epoch();
            update_frame_body_tracking();
            // auto t9 = Time::nanoseconds_since_epoch();
            update_frame_calibration();
            // auto t10 = Time::nanoseconds_since_epoch();
            // Log::fmessage("[{}][{}][{}][{}][{}][{}][{}][{}][{}][{}]\n",
            //     Time::difference_micro_s(t1,t2),
            //     Time::difference_micro_s(t2,t3),Time::difference_micro_s(t3,t4),Time::difference_micro_s(t4,t5),
            //     Time::difference_micro_s(t5,t6),Time::difference_micro_s(t6,t7),Time::difference_micro_s(t7,t8),
            //     Time::difference_micro_s(t8,t9),Time::difference_micro_s(t9,t10),Time::difference_micro_s(t1,t10));
        }

        {
            auto tFF = TimeDiffGuard(timeM, "FINALIZE_FRAME"sv);
            if(frame != nullptr){

                // set infos
                frame->idCapture       = static_cast<std::int32_t>(mInfos.id_capture());
                frame->afterCaptureTS  = timeM.get_end("CAPTURE_FRAME"sv).count();
                frame->receivedTS      = frame->afterCaptureTS;  // default init received TS with after capture TS

                frame->infosB[DCInfoType::DCMode] = static_cast<std::int64_t>(settings.config.mode);
                frame->mode            = settings.config.mode;

                frames.add_frame(std::move(frame));
            }
        }

        {
            auto tSF = TimeDiffGuard(timeM, "SEND_FRAME"sv);
            if(auto frameToSend = frames.take_frame_with_delay(timeM.get_end("CAPTURE_FRAME"sv), settings.misc.delayMs)){
                frame = frameToSend;
                new_frame_signal(std::move(frameToSend));
            }
        }
    }

    if(captureSuccess){
        mInfos.increment_capture_id();
    }

    return {std::move(frame),std::move(dFrame)};
}

auto DCFramesBuffer::add_frame(std::shared_ptr<DCFrame> frame) -> void{
    // store frame
    auto afterCaptureTS = std::chrono::nanoseconds(frame->afterCaptureTS);
    frames.push_back(std::make_tuple(afterCaptureTS, std::move(frame)));
}

auto DCFramesBuffer::add_data_frame(std::shared_ptr<DCDataFrame> dFrame) -> void{
    // store frame
    auto afterCaptureTS = std::chrono::nanoseconds(dFrame->afterCaptureTS);
    dataFrames.push_back(std::make_tuple(afterCaptureTS, std::move(dFrame)));
}

auto DCFramesBuffer::take_frame_with_delay(std::chrono::nanoseconds afterCaptureTS, int64_t delayMs) -> std::shared_ptr<DCFrame>{

    // check delay
    using namespace std::chrono;
    bool foundFrame = false;
    size_t idFrame = 0;
    for(size_t ii = 0; ii < frames.size(); ++ii){
        auto diff = duration_cast<milliseconds>(afterCaptureTS - std::get<0>(frames[ii]));
        if(diff.count() >= delayMs){
            foundFrame = true;
            idFrame = ii;
        }else{
            break;
        }
    }

    if(foundFrame){
        auto frame = std::move(frames[idFrame]);
        tool::erase_range(frames, 0, idFrame + 1);
        return std::get<1>(frame);
    }
    return nullptr;
}

auto DCFramesBuffer::get_data_frame_with_delay(std::chrono::nanoseconds afterCaptureTS, int64_t delayMs) -> std::shared_ptr<DCDataFrame>{

    // check delay
    using namespace std::chrono;
    bool foundFrame = false;
    size_t idFrame = 0;
    for(size_t ii = 0; ii < dataFrames.size(); ++ii){
        auto diff = duration_cast<milliseconds>(afterCaptureTS - std::get<0>(dataFrames[ii]));
        if(diff.count() >= delayMs){
            foundFrame = true;
            idFrame = ii;
        }else{
            break;
        }
    }

    if(foundFrame){
        auto dFrame = std::move(dataFrames[idFrame]);
        tool::erase_range(dataFrames, 0, idFrame + 1);
        return std::get<1>(dFrame);
    }
    return nullptr;
}


// auto DCDeviceImpl::start_reading_thread() -> void{
//     auto lg = LogGuard("DCDeviceImpl::start_reading_thread"sv);

//     timeM.start("START_READING_THREAD"sv);
//     loopT = std::make_unique<std::thread>([&](){
//         auto t = TimeDiffGuard(timeM, "LOOP"sv);
//         loop_initialization();
//         while(readFramesFromCameras){
//             auto tf = TimeDiffGuard(timeM, "LOOP_ITERATION"sv);
//             // LogGuard g("Read frame.\n"sv);
//             read_settings();
//             read_images();
//             process_data();
//         }
//     });
// }

// auto DCDeviceImpl::stop_reading_thread() -> void{
//     auto lg = LogGuard("DCDeviceImpl::stop_reading_thread"sv);
//     if(readFramesFromCameras){
//         readFramesFromCameras = false;

//         if(loopT != nullptr){
//             if(loopT->joinable()){
//                 loopT->join();
//             }
//             loopT = nullptr;
//         }
//     }

//     timeM.end("END_READING_THREAD"sv);
// }


// auto DCDeviceImpl::init_frames() -> void{

//     if(captureSuccess && dataIsValid){

//         if(settings.data.generation.has_data_to_generate()){
//             frame = std::make_shared<DCFrame>();
//         }else{
//             frame = nullptr;
//         }
//         if(settings.data.compression.has_data_to_compress()){
//             cFrame = std::make_shared<DCCompressedFrame>();
//         }else{
//             cFrame = nullptr;
//         }
//     }
// }

// auto DCDeviceImpl::resize_and_convert() -> void{

//     if(captureSuccess && dataIsValid){
//         {
//             auto tCCI = TimeDiffGuard(timeM, "CONVERT_COLOR_IMAGE"sv);
//             convert_color_image();
//         }
//         {
//             auto tRCI = TimeDiffGuard(timeM, "RESIZE_COLOR_IMAGE"sv);
//             resize_color_image_to_depth_size();
//         }
//         {
//             auto tGC = TimeDiffGuard(timeM, "GENERATE_CLOUD"sv);
//             generate_cloud(settings.data.capture_cloud() || settings.filters.filterDepthWithCloud);
//         }
//     }
// }

// auto DCDeviceImpl::preprocess() -> void{
//     if(captureSuccess && dataIsValid){
//         auto tPP = TimeDiffGuard(timeM, "PREPROCESS"sv);
//         preprocess_color_image();
//         preprocess_depth_sized_color_image();
//         preprocess_depth_image();
//         preprocess_infra_image();
//         preprocess_cloud_image();
//         preprocess_body_tracking_image();
//     }
// }

// auto DCDeviceImpl::filter() -> void{
//     if(captureSuccess && dataIsValid){
//         {
//             auto tFD = TimeDiffGuard(timeM, "FILTER_DEPTH"sv);
//             filter_depth_basic();
//             filter_depth_from_depth_sized_color();
//             filter_depth_from_infra();
//             filter_depth_from_cloud();
//             filter_depth_from_body_tracking();
//             filter_depth_complex();
//             update_valid_depth_values();
//         }

//         {
//             auto tFO = TimeDiffGuard(timeM, "FILTER_OTHER"sv);
//             filter_depth_sized_color_from_depth();
//             mix_depth_sized_color_with_body_tracking();
//             filter_infra_from_depth();
//             mix_infra_with_body_tracking();
//         }
//     }
// }

// auto DCDeviceImpl::update_compressed_frame() -> void{
//     if(captureSuccess && dataIsValid){
//         auto tUCF = TimeDiffGuard(timeM, "UPDATE_COMPRESSED_FRAME"sv);
//         update_compressed_frame_color();
//         update_compressed_frame_depth_sized_color();
//         update_compressed_frame_depth();
//         update_compressed_frame_infra();
//         update_compressed_frame_cloud();
//         update_compressed_frame_audio();
//         update_compressed_frame_imu();
//         update_compressed_frame_bodies();
//         update_compressed_frame_calibration();
//     }
// }

// auto DCDeviceImpl::update_frame() -> void{
//     if(captureSuccess && dataIsValid){
//         auto tUF = TimeDiffGuard(timeM, "UPDATE_FRAME"sv);
//         update_frame_color();
//         update_frame_depth_sized_color();
//         update_frame_depth();
//         update_frame_infra();
//         update_frame_cloud();
//         update_frame_audio();
//         update_frame_imu();
//         update_frame_bodies();
//         update_frame_calibration();
//     }
// }

// auto DCDeviceImpl::read_data_taskflow() -> std::unique_ptr<tf::Taskflow>{

//     auto readDataTF = std::make_unique<tf::Taskflow>();

//     auto captureCT = readDataTF->emplace([&](){
//         {
//             auto t = TimeDiffGuard(timeM, "CAPTURE_FRAME"sv);
//             captureSuccess = capture_frame(mInfos.timeout_ms());
//             if(!captureSuccess){
//                 dataIsValid = false;
//                 return;
//             }
//         }
//         timeM.start("READ_IMAGES"sv);
//         {
//             auto t = TimeDiffGuard(timeM, "READ_COLOR_IMAGE"sv);
//             read_color_image(mInfos.has_color() && settings.data.capture_color());
//         }
//         {
//             auto t = TimeDiffGuard(timeM, "READ_DEPTH_IMAGE"sv);
//             read_depth_image(mInfos.has_depth() && settings.data.capture_depth());
//         }
//         {
//             auto t = TimeDiffGuard(timeM, "READ_INFRA_IMAGE"sv);
//             read_infra_image(mInfos.has_infra() && settings.data.capture_infra());
//         }
//         {
//             auto t = TimeDiffGuard(timeM, "READ_BODY_TRACKING"sv);
//             read_body_tracking(settings.data.capture_body_tracking() && settings.config.btEnabled && mInfos.has_depth());
//         }
//         {
//             auto t = TimeDiffGuard(timeM, "READ_AUDIO"sv);
//             read_audio(settings.data.capture_audio() && mInfos.has_audio());
//         }
//         {
//             auto t = TimeDiffGuard(timeM, "READ_IMU"sv);
//             read_IMU(settings.data.capture_imu());
//         }
//         timeM.end("READ_IMAGES"sv);
//         dataIsValid = check_data_validity();
//     }).name("capture_frame");

//     return readDataTF;
// }

// auto DCDeviceImpl::process_data_taskflow()  -> std::unique_ptr<tf::Taskflow>{


//     auto processDataTF = std::make_unique<tf::Taskflow>();
//     auto startProcessingDataT = processDataTF->emplace([&](){

//         // timeM.start("PROCESSING_DATA"sv);
//         // if(settings.data.client.generation.has_data_to_generate()){
//         //     frame = std::make_shared<DCFrame>();
//         // }else{
//         //     frame = nullptr;
//         // }
//         // if(settings.data.client.compression.has_data_to_compress()){
//         //     cFrame = std::make_shared<DCCompressedFrame>();
//         // }else{
//         //     cFrame = nullptr;
//         // }

//         // {
//         //     auto t = TimeDiffGuard(timeM, "CONVERT_COLOR_IMAGE"sv);
//         //     convert_color_image();
//         // }
//         // {
//         //     auto t = TimeDiffGuard(timeM, "RESIZE_COLOR_IMAGE"sv);
//         //     resize_color_image_to_depth_size();
//         // }
//         // {
//         //     auto t = TimeDiffGuard(timeM, "GENERATE_CLOUD"sv);
//         //     generate_cloud(settings.data.generate_cloud_from_client() || settings.filters.filterDepthWithCloud);
//         // }

//     }).name("start_processing_data");

//     auto preprocessColorImageT = processDataTF->emplace([&](){
//         preprocess_color_image();
//     }).name("preprocess_color_image");
//     preprocessColorImageT.priority(tf::TaskPriority::HIGH);

//     auto preprocessDepthSizedColorImageT = processDataTF->emplace([&](){
//         preprocess_depth_sized_color_image();
//     }).name("preprocess_depth_sized_color_image");
//     preprocessDepthSizedColorImageT.priority(tf::TaskPriority::HIGH);

//     auto preprocessDepthImageT = processDataTF->emplace([&](){
//         preprocess_depth_image();
//     }).name("preprocess_depth_image");
//     preprocessDepthImageT.priority(tf::TaskPriority::HIGH);

//     auto preprocessInfraImageT = processDataTF->emplace([&](){
//         preprocess_infra_image();
//     }).name("preprocess_infra_image");
//     preprocessInfraImageT.priority(tf::TaskPriority::NORMAL);

//     auto preprocessCloudImageT = processDataTF->emplace([&](){
//         preprocess_cloud_image();
//     }).name("preprocess_cloud_image");
//     preprocessCloudImageT.priority(tf::TaskPriority::HIGH);

//     auto preprocessBodytrackingImageT     = processDataTF->emplace([&](){
//         preprocess_body_tracking_image();
//     }).name("preprocess_body_tracking_image");
//     preprocessBodytrackingImageT.priority(tf::TaskPriority::NORMAL);

//     auto filterDepthBasicT = processDataTF->emplace([&](){
//         timeM.start("FILTER_DEPTH"sv);
//         filter_depth_basic();
//     }).name("filter_depth_basic");
//     filterDepthBasicT.priority(tf::TaskPriority::HIGH);

//     auto filterDepthFromDepthSizedColorT = processDataTF->emplace([&](){
//         filter_depth_from_depth_sized_color();
//     }).name("filter_depth_from_depth_sized_color");
//     filterDepthFromDepthSizedColorT.priority(tf::TaskPriority::HIGH);

//     auto filterDepthFromInfraT = processDataTF->emplace([&](){
//         filter_depth_from_infra();
//     }).name("filter_depth_from_infra");
//     filterDepthFromInfraT.priority(tf::TaskPriority::HIGH);

//     auto filterDepthFromBodyTrackingT = processDataTF->emplace([&](){
//         filter_depth_from_body_tracking();
//     }).name("filter_depth_from_body_tracking");
//     filterDepthFromBodyTrackingT.priority(tf::TaskPriority::HIGH);

//     auto filterDepthFromCloudT = processDataTF->emplace([&](){
//         filter_depth_from_cloud();
//     }).name("filter_depth_from_cloud");
//     filterDepthFromCloudT.priority(tf::TaskPriority::HIGH);

//     auto filterDepthComplexT = processDataTF->emplace([&](){
//         filter_depth_complex();
//         update_valid_depth_values();
//         timeM.end("FILTER_DEPTH"sv);
//     }).name("filter_depth_complex");
//     filterDepthComplexT.priority(tf::TaskPriority::HIGH);

//     startProcessingDataT.precede(
//         preprocessColorImageT,
//         preprocessDepthImageT,
//         preprocessDepthSizedColorImageT,
//         preprocessCloudImageT,
//         preprocessInfraImageT,
//         preprocessBodytrackingImageT
//     );

//     filterDepthBasicT.succeed(
//         preprocessDepthImageT
//     );

//     filterDepthFromBodyTrackingT.succeed(
//         filterDepthBasicT,
//         preprocessBodytrackingImageT
//     );
//     filterDepthFromInfraT.succeed(
//         filterDepthFromBodyTrackingT,
//         preprocessInfraImageT
//     );
//     filterDepthFromCloudT.succeed(
//         filterDepthFromInfraT,
//         preprocessCloudImageT
//     );

//     filterDepthFromDepthSizedColorT.succeed(
//         filterDepthFromCloudT,
//         preprocessDepthSizedColorImageT
//     );

//     filterDepthComplexT.succeed(
//         filterDepthFromDepthSizedColorT
//     );

//     auto filterDepthSizedColorFromDepthT = processDataTF->emplace([&](){
//         filter_depth_sized_color_from_depth();
//     }).name("filter_depth_sized_color_from_depth");
//     filterDepthSizedColorFromDepthT.priority(tf::TaskPriority::NORMAL);

//     auto filterInfraFromDepthT = processDataTF->emplace([&](){
//         filter_infra_from_depth();
//     }).name("filter_infra_from_depth");
//     filterInfraFromDepthT.priority(tf::TaskPriority::NORMAL);

//     auto mixDepthSizedColorWithBodyTrackingT = processDataTF->emplace([&](){
//         mix_depth_sized_color_with_body_tracking();
//     }).name("mix_depth_sized_color_with_body_tracking");
//     mixDepthSizedColorWithBodyTrackingT.priority(tf::TaskPriority::NORMAL);

//     auto mixInfraWithBodyTrackingT = processDataTF->emplace([&](){
//         mix_infra_with_body_tracking();
//     }).name("mix_infra_with_body_tracking");
//     mixInfraWithBodyTrackingT.priority(tf::TaskPriority::NORMAL);

//     filterDepthSizedColorFromDepthT.succeed(filterDepthComplexT);
//     filterDepthSizedColorFromDepthT.precede(mixDepthSizedColorWithBodyTrackingT);

//     filterInfraFromDepthT.succeed(filterDepthComplexT);
//     filterInfraFromDepthT.precede(mixInfraWithBodyTrackingT);

//     auto updateCompressedFrameColorT = processDataTF->emplace([&](){
//         update_compressed_frame_color();
//     }).name("update_compressed_frame_color");
//     updateCompressedFrameColorT.priority(tf::TaskPriority::HIGH);

//     auto updateCompressedFrameDepthSizedColorT = processDataTF->emplace([&](){
//         update_compressed_frame_depth_sized_color();
//     }).name("update_compressed_frame_depth_sized_color");
//     updateCompressedFrameDepthSizedColorT.priority(tf::TaskPriority::HIGH);

//     auto updateCompressedFrameDepthT = processDataTF->emplace([&](){
//         update_compressed_frame_depth();
//     }).name("update_compressed_frame_depth");
//     updateCompressedFrameDepthT.priority(tf::TaskPriority::HIGH);

//     auto updateCompressedFrameInfraT = processDataTF->emplace([&](){
//         update_compressed_frame_infra();
//     }).name("update_compressed_frame_infra");
//     updateCompressedFrameInfraT.priority(tf::TaskPriority::HIGH);

//     auto updateCompressedFrameCloudT = processDataTF->emplace([&](){
//         update_compressed_frame_cloud();
//     }).name("update_compressed_frame_cloud");
//     updateCompressedFrameCloudT.priority(tf::TaskPriority::HIGH);

//     auto updateCompressedFrameAudioT = processDataTF->emplace([&](){
//         update_compressed_frame_audio();
//     }).name("update_compressed_frame_audio");
//     updateCompressedFrameAudioT.priority(tf::TaskPriority::HIGH);

//     auto updateCompressedFrameIMUT = processDataTF->emplace([&](){
//           update_compressed_frame_imu();
//       }).name("update_compressed_frame_imu");
//     updateCompressedFrameIMUT.priority(tf::TaskPriority::HIGH);

//     auto updateCompressedFrameBodiesT = processDataTF->emplace([&](){
//         update_compressed_frame_bodies();
//     }).name("update_compressed_frame_bodies");
//     updateCompressedFrameBodiesT.priority(tf::TaskPriority::HIGH);

//     auto updateCompressedFrameCalibrationT = processDataTF->emplace([&](){
//         update_compressed_frame_calibration();
//     }).name("update_compressed_frame_calibration");
//     updateCompressedFrameCalibrationT.priority(tf::TaskPriority::HIGH);

//     auto finalizedCompressedFrameT = processDataTF->emplace([&](){
//         auto t = TimeDiffGuard(timeM, "FINALIZE_COMPRESSED_FRAME"sv);
//         if(cFrame != nullptr){

//             // set infos
//             cFrame->idDevice           = static_cast<std::uint8_t>(settings.config.idDevice);
//             cFrame->idCapture          = static_cast<std::int32_t>(mInfos.id_capture());
//             cFrame->afterCaptureTS     = timeM.get_end("CAPTURE_FRAME"sv).count();
//             cFrame->receivedTS         = cFrame->afterCaptureTS; // default init received TS with after capture TS
//             cFrame->mode               = settings.config.mode;
//             cFrame->validVerticesCount = fData.validDepthValues;

//             // add frame
//             frames.add_compressed_frame(std::move(cFrame));
//             cFrame = nullptr;
//         }
//     }).name("finalize_compressed_frame");
//     finalizedCompressedFrameT.priority(tf::TaskPriority::HIGH);

//     auto sendCompressedFrameT  = processDataTF->emplace([&](){
//         auto t = TimeDiffGuard(timeM, "SEND_COMPRESSED_FRAME"sv);
//         if(auto compressedFrameToSend = frames.get_compressed_frame_with_delay(timeM.get_end("CAPTURE_FRAME"sv), cDelayS.delayMs)){
//             if(dcDevice){
//                 dcDevice->new_compressed_frame_signal(std::move(compressedFrameToSend));
//             }else{
//                 new_compressed_frame_signal(std::move(compressedFrameToSend));
//             }

//             // auto t1 = Time::nanoseconds_since_epoch();
//             // auto t2 = timeM.get_end("CAPTURE_FRAME"sv);
//             // std::cout << "-> " << Time::to_micro_s(t1-t2) << "\n";
//         }

//     }).name("send_compressed_frame");
//     sendCompressedFrameT.priority(tf::TaskPriority::HIGH);

//     updateCompressedFrameColorT.succeed(startProcessingDataT);
//     updateCompressedFrameDepthSizedColorT.succeed(mixDepthSizedColorWithBodyTrackingT);
//     updateCompressedFrameDepthT.succeed(filterDepthComplexT);
//     updateCompressedFrameInfraT.succeed(mixInfraWithBodyTrackingT);
//     updateCompressedFrameCloudT.succeed(filterDepthComplexT);

//     updateCompressedFrameAudioT.succeed(startProcessingDataT);
//     updateCompressedFrameCalibrationT.succeed(startProcessingDataT);
//     updateCompressedFrameIMUT.succeed(startProcessingDataT);
//     updateCompressedFrameBodiesT.succeed(preprocessBodytrackingImageT);

//     finalizedCompressedFrameT.succeed(
//         updateCompressedFrameColorT, updateCompressedFrameDepthSizedColorT, updateCompressedFrameDepthT,
//         updateCompressedFrameInfraT,  updateCompressedFrameCloudT, updateCompressedFrameCalibrationT,
//         updateCompressedFrameAudioT, updateCompressedFrameIMUT, updateCompressedFrameBodiesT
//     );
//     sendCompressedFrameT.succeed(finalizedCompressedFrameT);

//     auto updateFrameColorT = processDataTF->emplace([&](){
//         update_frame_color();
//     }).name("update_frame_color");
//     updateFrameColorT.priority(tf::TaskPriority::NORMAL);

//     auto updateFrameDepthSizedColorT = processDataTF->emplace([&](){
//         update_frame_depth_sized_color();
//     }).name("update_frame_depth_sized_color");
//     updateFrameDepthSizedColorT.priority(tf::TaskPriority::NORMAL);

//     auto updateFrameDepthT = processDataTF->emplace([&](){
//         update_frame_depth();
//     }).name("update_frame_depth");
//     updateFrameDepthT.priority(tf::TaskPriority::NORMAL);

//     auto updateFrameInfraT = processDataTF->emplace([&](){
//         update_frame_infra();
//     }).name("update_frame_infra");
//     updateFrameInfraT.priority(tf::TaskPriority::NORMAL);

//     auto updateFrameCloudT = processDataTF->emplace([&](){
//         update_frame_cloud();
//     }).name("update_frame_cloud");
//     updateFrameCloudT.priority(tf::TaskPriority::NORMAL);

//     auto updateFrameAudioT = processDataTF->emplace([&](){
//         update_frame_audio();
//     }).name("update_compressed_frame_audio");
//     updateFrameAudioT.priority(tf::TaskPriority::NORMAL);

//     auto updateFrameIMUT = processDataTF->emplace([&](){
//         update_frame_imu();
//     }).name("update_frame_imu");
//     updateFrameIMUT.priority(tf::TaskPriority::NORMAL);

//     auto updateFrameBodiesT = processDataTF->emplace([&](){
//         update_frame_bodies();
//     }).name("update_frame_bodies");
//     updateFrameBodiesT.priority(tf::TaskPriority::NORMAL);

//     auto updateFrameCalibrationT = processDataTF->emplace([&](){
//         update_frame_calibration();
//         }).name("update_frame_calibration");
//     updateFrameCalibrationT.priority(tf::TaskPriority::NORMAL);

//     auto finalizedFrameT = processDataTF->emplace([&](){
//         auto t = TimeDiffGuard(timeM, "FINALIZE_FRAME"sv);
//         if(frame != nullptr){
//             // set infos
//             frame->idCapture       = static_cast<std::int32_t>(mInfos.id_capture());
//             frame->afterCaptureTS  = timeM.get_end("CAPTURE_FRAME"sv).count();
//             frame->receivedTS      = frame->afterCaptureTS;  // default init received TS with after capture TS
//             frame->mode            = settings.config.mode;

//             frames.add_frame(std::move(frame));
//             frame = nullptr;
//         }
//     }).name("finalize_frame");
//     finalizedFrameT.priority(tf::TaskPriority::NORMAL);

//     auto sendFrameT  = processDataTF->emplace([&](){
//         auto t = TimeDiffGuard(timeM, "SEND_FRAME"sv);
//         if(auto frameToSend = frames.take_frame_with_delay(timeM.get_end("CAPTURE_FRAME"sv), cDelayS.delayMs)){
//             if(dcDevice){
//                 dcDevice->new_frame_signal(std::move(frameToSend));
//             }else{
//                 new_frame_signal(std::move(frameToSend));
//             }
//         }
//     }).name("send_frame");
//     sendFrameT.priority(tf::TaskPriority::NORMAL);

//     updateFrameColorT.succeed(startProcessingDataT);
//     updateFrameDepthSizedColorT.succeed(mixDepthSizedColorWithBodyTrackingT);
//     updateFrameDepthT.succeed(filterDepthComplexT);
//     updateFrameInfraT.succeed(mixInfraWithBodyTrackingT);
//     updateFrameCloudT.succeed(filterDepthComplexT);

//     updateFrameAudioT.succeed(startProcessingDataT);
//     updateFrameCalibrationT.succeed(startProcessingDataT);
//     updateFrameIMUT.succeed(startProcessingDataT);
//     updateFrameBodiesT.succeed(preprocessBodytrackingImageT);

//     finalizedFrameT.succeed(
//         updateFrameColorT, updateFrameDepthSizedColorT, updateFrameDepthT,
//         updateFrameInfraT,  updateFrameCloudT, updateFrameCalibrationT,
//         updateFrameAudioT, updateFrameIMUT, updateFrameBodiesT
//     );
//     sendFrameT.succeed(finalizedFrameT);

//     auto endProcessingDataT = processDataTF->emplace([&](){
//     timeM.end("PROCESSING_DATA"sv);
//     // std::cout << "pd]";
//     }).name("end_processing_data");

//     endProcessingDataT.succeed(sendCompressedFrameT, sendFrameT);

//     return processDataTF;
// }

// auto DCDeviceImpl::process_frame_taskflow(tf::Taskflow &readDataTF, tf::Taskflow &processDataTF) -> std::unique_ptr<tf::Taskflow>{

//     auto processFrameTF = std::make_unique<tf::Taskflow>();

//     auto startProcessingFrameT       = processFrameTF->emplace([&](){
//         // std::cout << "[pf";
//         timeM.start("PROCESS_FRAME"sv);
//         fData.reset_spans();
//     }).name("start_processing_frame");

//     auto updateSettingsT             = processFrameTF->emplace([&](){
//         parametersM.lock();
//         {
//             settings.filters = settings.filters;
//             settings.data    = settings.data;
//             cDelayS   = settings.delay;
//             times     = timeM.times;
//         }
//         parametersM.unlock();
//         update_from_data_settings();

//     }).name("update_settings");

//     auto readingDataModuleT = processFrameTF->composed_of(readDataTF).name("reading_data_module");

//     auto checkDataCT = processFrameTF->emplace([&](){
//         return captureSuccess && dataIsValid;
//     }).name("check_data");

//     auto failureDataT = processFrameTF->emplace([&](){
//         if(captureSuccess){
//             mInfos.increment_capture_id();
//         }
//         timeM.end("PROCESS_FRAME"sv);
//     }).name("failure_data");

//     auto startProcessingDataModuleT  = processFrameTF->composed_of(processDataTF).name("processing_data_module");

//     auto endProcessingFrameT       = processFrameTF->emplace([&](){
//         mInfos.increment_capture_id();
//         // std::cout << "pf]";
//     timeM.end("PROCESS_FRAME"sv);
//     }).name("end_processing_frame");

//     startProcessingFrameT.precede(updateSettingsT);
//     updateSettingsT.precede(readingDataModuleT);
//     readingDataModuleT.precede(checkDataCT);

//     checkDataCT.precede(failureDataT, startProcessingDataModuleT);

//     endProcessingFrameT.succeed(startProcessingDataModuleT);




//     return processFrameTF;
// }


// #include "thirdparty/stb/stb_image_write.h"

// auto DCDeviceImpl::debug_save_images(std::string parentPath) -> void{

//     parentPath += std::format("/dev_{}_",mInfos.id_capture()%10);

//     if(!fData.color.empty()){
//         std::cout << std::format("write color image [{},{},{}] [{},{}]\n",mInfos.color_width(),mInfos.color_height(),mInfos.color_size(), fData.color.size(), fData.color.size_bytes()) << "\n";
//         stbi_write_png(std::format("{}_color.png", parentPath).c_str(), mInfos.color_width(), mInfos.color_height(), 4, fData.color.data(), mInfos.color_width()*4);
//     }

//     if(!fData.dephtSizedColor.empty()){
//         std::cout << std::format("write depth sized color image [{},{},{}] [{},{}]\n",mInfos.depth_width(),mInfos.depth_height(),mInfos.depth_size(), fData.dephtSizedColor.size(), fData.dephtSizedColor.size_bytes()) << "\n";
//         stbi_write_png(std::format("{}_depth_sized_color.png", parentPath).c_str(), mInfos.depth_width(), mInfos.depth_height(), 4, fData.dephtSizedColor.data(), mInfos.depth_width()*4);
//     }

// }



// executor = std::make_unique<tf::Executor>(1);

// tf::Taskflow taskf;

// auto preprocessColorImageT              = taskf.emplace([&](){preprocess_color_image();}).name("preprocess_color_image");
// auto preprocessDepthSizedColorImageT    = taskf.emplace([&](){preprocess_depth_sized_color_image();}).name("preprocess_depth_sized_color_image");
// auto preprocessDepthImageT              = taskf.emplace([&](){preprocess_depth_image();}).name("preprocess_depth_image");
// auto preprocessInfraImageT              = taskf.emplace([&](){preprocess_infra_image();}).name("preprocess_infra_image");
// auto preprocessCloudImageT              = taskf.emplace([&](){preprocess_cloud_image();}).name("preprocess_cloud_image");
// auto preprocessBodyTrackingImageT       = taskf.emplace([&](){preprocess_body_tracking_image();}).name("preprocess_body_tracking_image");

// auto filterDepthT = taskf.emplace([&](){
//     auto t = TimeDiffGuard(timeM, "FILTER_DEPTH"sv);
//     filter_depth_basic();
//     filter_depth_from_depth_sized_color();
//     filter_depth_from_infra();
//     filter_depth_from_cloud();
//     filter_depth_from_body_tracking();
//     filter_depth_complex();
//     update_valid_depth_values();
// }).name("filter_depth");
// filterDepthT.priority(tf::TaskPriority::MAX);

// auto filterDepthSizedColorFromDepthT = taskf.emplace([&](){filter_depth_sized_color_from_depth();}).name("filter_depth_sized_color_from_depth");
// auto mixDepthSizedColorWithBodyTrackingT = taskf.emplace([&](){mix_depth_sized_color_with_body_tracking();}).name("mix_depth_sized_color_with_body_tracking");

// auto filterInfraFromDepthT = taskf.emplace([&](){filter_infra_from_depth();}).name("filter_infra_from_depth");
// auto mixInfraWithBodyTrackingT = taskf.emplace([&](){mix_infra_with_body_tracking();}).name("mix_infra_with_body_tracking");

// auto startUpdateCompressedFrameT = taskf.emplace([&](){

// }).name("start_update_compressed_frame");
// auto updateCompressedFrameColorT = taskf.emplace([&](){update_compressed_frame_color();}).name("update_compressed_frame_color");
// auto updateCompressedFrameDepthSizedColorT = taskf.emplace([&](){update_compressed_frame_depth_sized_color();}).name("update_compressed_frame_depth_sized_color");
// auto updateCompressedFrameDepthT = taskf.emplace([&](){update_compressed_frame_depth();}).name("update_compressed_frame_depth");
// auto updateCompressedFrameInfraT = taskf.emplace([&](){update_compressed_frame_infra();}).name("update_compressed_frame_infra");
// auto updateCompressedFrameCloudT = taskf.emplace([&](){update_compressed_frame_cloud();}).name("update_compressed_frame_cloud");
// auto updateCompressedFrameAudioT = taskf.emplace([&](){update_compressed_frame_audio();}).name("update_compressed_frame_audio");
// auto updateCompressedFrameIMUT = taskf.emplace([&](){update_compressed_frame_imu();}).name("update_compressed_frame_imu");
// auto updateCompressedFrameBodiesT = taskf.emplace([&](){update_compressed_frame_bodies();}).name("update_compressed_frame_bodies");
// auto updateCompressedFrameCalibrationT = taskf.emplace([&](){update_compressed_frame_calibration();}).name("update_compressed_frame_calibration");
// auto tCompressedFrameFinalize = taskf.emplace([&](){
//     auto t = TimeDiffGuard(timeM, "FINALIZE_COMPRESSED_FRAME"sv);
//     if(cFrame != nullptr){
//         // set infos
//         cFrame->idDevice           = static_cast<std::uint8_t>(settings.config.idDevice);
//         cFrame->idCapture          = static_cast<std::int32_t>(mInfos.id_capture());
//         cFrame->afterCaptureTS     = timeM.get_end("CAPTURE_FRAME"sv).count();
//         cFrame->receivedTS         = cFrame->afterCaptureTS; // default init received TS with after capture TS
//         cFrame->mode               = settings.config.mode;
//         cFrame->validVerticesCount = fData.validDepthValues;

//         // add frame
//         frames.add_compressed_frame(std::move(cFrame));
//         cFrame = nullptr;
//     }
// }).name("finalize_compressed_frame");

// auto sendCompressedFrameT = taskf.emplace([&](){
//     auto t = TimeDiffGuard(timeM, "SEND_COMPRESSED_FRAME"sv);
//     if(auto compressedFrameToSend = frames.get_compressed_frame_with_delay(timeM.get_end("CAPTURE_FRAME"sv), cDelayS.delayMs)){
//         dcDevice->new_compressed_frame_signal(std::move(compressedFrameToSend));
//     }
// }).name("send_compressed_frame");


// auto startUpdateFrameT = taskf.emplace([&](){}).name("start_update_rame");
// auto updateFrameColorT = taskf.emplace([&](){update_frame_color();}).name("update_frame_color");
// auto updateFrameDepthSizedColorT = taskf.emplace([&](){update_frame_depth_sized_color();}).name("update_frame_depth_sized_color");
// auto updateFrameDepthT = taskf.emplace([&](){update_frame_depth();}).name("update_frame_depth");
// auto updateFrameInfraT = taskf.emplace([&](){update_frame_infra();}).name("update_frame_infra");
// auto updateFrameCloudT = taskf.emplace([&](){update_frame_cloud();}).name("update_frame_cloud");
// auto updateFrameAudioT = taskf.emplace([&](){update_frame_audio();}).name("update_frame_audio");
// auto updateFrameIMUT = taskf.emplace([&](){update_frame_imu();}).name("update_frame_imu");
// auto updateFrameBodiesT = taskf.emplace([&](){update_frame_bodies();}).name("update_frame_bodies");
// auto updateFrameCalibrationT = taskf.emplace([&](){update_frame_calibration();}).name("update_frame_calibration");
// auto tFrameFinalize = taskf.emplace([&](){
//     auto t = TimeDiffGuard(timeM, "FINALIZE_FRAME"sv);
//     if(frame != nullptr){

//         // set infos
//         frame->idCapture       = static_cast<std::int32_t>(mInfos.id_capture());
//         frame->afterCaptureTS  = timeM.get_end("CAPTURE_FRAME"sv).count();
//         frame->receivedTS      = frame->afterCaptureTS;  // default init received TS with after capture TS
//         frame->mode            = settings.config.mode;

//         frames.add_frame(std::move(frame));
//         frame = nullptr;
//     }
// }).name("finalize_frame");

// auto sendFrameT = taskf.emplace([&](){
//     auto tsf = TimeDiffGuard(timeM, "SEND_FRAME"sv);
//     if(auto frameToSend = frames.take_frame_with_delay(timeM.get_end("CAPTURE_FRAME"sv), cDelayS.delayMs)){
//         dcDevice->new_frame_signal(std::move(frameToSend));
//     }
// }).name("send_frame");

// filterDepthT.succeed(
//     preprocessColorImageT, preprocessDepthSizedColorImageT, preprocessDepthImageT,
//     preprocessInfraImageT, preprocessCloudImageT, preprocessBodyTrackingImageT
// );

// filterDepthT.precede(filterDepthSizedColorFromDepthT, filterInfraFromDepthT);

// mixDepthSizedColorWithBodyTrackingT.succeed(filterDepthSizedColorFromDepthT);
// mixInfraWithBodyTrackingT.succeed(filterInfraFromDepthT);

// startUpdateCompressedFrameT.succeed(mixDepthSizedColorWithBodyTrackingT, mixInfraWithBodyTrackingT);
// startUpdateCompressedFrameT.precede(
//     updateCompressedFrameColorT, updateCompressedFrameDepthSizedColorT, updateCompressedFrameDepthT,
//     updateCompressedFrameInfraT, updateCompressedFrameCloudT, updateCompressedFrameAudioT,
//     updateCompressedFrameIMUT, updateCompressedFrameDepthT, updateCompressedFrameBodiesT,
//     updateCompressedFrameCalibrationT
// );

// tCompressedFrameFinalize.succeed(
//     updateCompressedFrameColorT, updateCompressedFrameDepthSizedColorT, updateCompressedFrameDepthT,
//     updateCompressedFrameInfraT, updateCompressedFrameCloudT, updateCompressedFrameAudioT,
//     updateCompressedFrameIMUT, updateCompressedFrameDepthT, updateCompressedFrameBodiesT,
//     updateCompressedFrameCalibrationT
// );
// sendCompressedFrameT.succeed(tCompressedFrameFinalize);

// startUpdateFrameT.succeed(sendCompressedFrameT);
// startUpdateFrameT.precede(
//     updateFrameColorT, updateFrameDepthSizedColorT, updateFrameDepthT,
//     updateFrameInfraT, updateFrameCloudT, updateFrameAudioT,
//     updateFrameIMUT, updateFrameDepthT, updateFrameBodiesT,
//     updateFrameCalibrationT
// );
// tFrameFinalize.succeed(
//     updateFrameColorT, updateFrameDepthSizedColorT, updateFrameDepthT,
//     updateFrameInfraT, updateFrameCloudT, updateFrameAudioT,
//     updateFrameIMUT, updateFrameDepthT, updateFrameBodiesT,
//     updateFrameCalibrationT
// );
// sendFrameT.succeed(tFrameFinalize);
// auto DCDeviceImpl::do_loop() -> void{

//     auto t = TimeDiffGuard(timeM, "LOOP"sv);
//     loop_initialization();
//     while(readFramesFromCameras){
//         loop_iteration();

//     }

// }


// if(idNH != -1 && idNG != -1){
//     normal += normalize(cross(vec(currentP, fData.depthCloud[idNG].template conv<float>()), vec(currentP, fData.depthCloud[idNH].template conv<float>())));
//     ++count;
// }
// if(idNG != -1 && idNF != -1){
//     normal += normalize(cross(vec(currentP, fData.depthCloud[idNF].template conv<float>()), vec(currentP, fData.depthCloud[idNG].template conv<float>())));
//     ++count;
// }
// if(idNF != -1 && idND != -1){
//     normal += normalize(cross(vec(currentP, fData.depthCloud[idND].template conv<float>()), vec(currentP, fData.depthCloud[idNF].template conv<float>())));
//     ++count;
// }
// if(idND != -1 && idNA != -1){
//     normal += normalize(cross(vec(currentP, fData.depthCloud[idNA].template conv<float>()), vec(currentP, fData.depthCloud[idND].template conv<float>())));
//     ++count;
// }


// Vec3f normal{0,0,0};
// int count = 0;
// if(idNA != -1 && idNB != -1){
//     normal += normalize(cross(vec(currentP, fData.depthCloud[idNB].template conv<float>()), vec(currentP, fData.depthCloud[idNA].template conv<float>())));
//     ++count;
// }
// if(idNB != -1 && idNC != -1){
//     normal += normalize(cross(vec(currentP, fData.depthCloud[idNC].template conv<float>()), vec(currentP, fData.depthCloud[idNB].template conv<float>())));
//     ++count;
// }
// if(idNC != -1 && idNE != -1){
//     normal += normalize(cross(vec(currentP, fData.depthCloud[idNE].template conv<float>()), vec(currentP, fData.depthCloud[idNC].template conv<float>())));
//     ++count;
// }
// if(idNE != -1 && idNH != -1){
//     normal += normalize(cross(vec(currentP, fData.depthCloud[idNH].template conv<float>()), vec(currentP, fData.depthCloud[idNE].template conv<float>())));
//     ++count;
// }
// if(idNH != -1 && idNG != -1){
//     normal += normalize(cross(vec(currentP, fData.depthCloud[idNG].template conv<float>()), vec(currentP, fData.depthCloud[idNH].template conv<float>())));
//     ++count;
// }
// if(idNG != -1 && idNF != -1){
//     normal += normalize(cross(vec(currentP, fData.depthCloud[idNF].template conv<float>()), vec(currentP, fData.depthCloud[idNG].template conv<float>())));
//     ++count;
// }
// if(idNF != -1 && idND != -1){
//     normal += normalize(cross(vec(currentP, fData.depthCloud[idND].template conv<float>()), vec(currentP, fData.depthCloud[idNF].template conv<float>())));
//     ++count;
// }
// if(idND != -1 && idNA != -1){
//     normal += normalize(cross(vec(currentP, fData.depthCloud[idNA].template conv<float>()), vec(currentP, fData.depthCloud[idND].template conv<float>())));
//     ++count;
// }
