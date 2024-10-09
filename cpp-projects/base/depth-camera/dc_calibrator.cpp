
/*******************************************************************************
** Toolset-k4-scaner-manager                                                  **
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

#include "dc_calibrator.hpp"

// std
#include <random>

// open3D
#include "open3d/pipelines/registration/Registration.h"
#include "open3d/pipelines/registration/Feature.h"
#include "open3d/geometry/PointCloud.h"

// local
#include "utility/time.hpp"
#include "utility/logger.hpp"
#include "utility/string_geo.hpp"
#include "depth-camera/settings/dc_model_settings.hpp"

using namespace tool::cam;
using namespace tool::geo;

auto from_eigen_mat(const Eigen::Matrix4d_u &eMat) -> Mat4f {
    // apply transformation to source
    Mat4f mat = Mat4f::identity();
    for(int ii = 0; ii < 4; ++ii){
        for(int jj = 0; jj < 4; ++jj){
            mat.at(ii,jj) = static_cast<float>(eMat(jj,ii));
        }
    }
    return mat;
}

struct DCCalibrator::Impl{
    
    auto process_cloud(const DCCalibratorSettings &settings, const geo::ColorCloud &cloud) -> geo::ColorCloud;
    auto set_model_cloud(const DCCalibratorSettings &settings, size_t idModel, const DCModelSettings &modelTr) -> bool;
    auto set_source_cloud(const DCCalibratorSettings &settings, size_t idSource, const DCModelSettings &sourceTr) -> bool;
    auto do_RANSAC(const DCCalibratorSettings &settings, unsigned int seed) const -> open3d::pipelines::registration::RegistrationResult;
    auto do_ICP(const DCCalibratorSettings &settings) const -> open3d::pipelines::registration::RegistrationResult;
    auto compute_charmfer_distance_between_with_current_source(const geo::Mat4f &model) -> double;

    static auto best_registration_results(const std::vector<std::tuple<open3d::pipelines::registration::RegistrationResult, double>> &results) -> std::tuple<open3d::pipelines::registration::RegistrationResult, double>;
    static auto compute_new_model(const open3d::pipelines::registration::RegistrationResult &result, const geo::Mat4f &model) -> geo::Mat4f;
    static auto convert_to_opend3d_pc(const ColorCloud &cloud) -> std::shared_ptr<open3d::geometry::PointCloud>;

    std::mt19937 gen;
    std::vector<DCCalibratorClientData> clientsData;
    geo::ColorCloud modelCloud;
    geo::ColorCloud sourceCloud;

    std::vector<std::optional<DCModelSettings>> calibrations;

private:
    
    auto preprocess_features(const DCCalibratorSettings &settings, const ColorCloud &cloud) -> std::tuple<std::shared_ptr<open3d::geometry::PointCloud>, std::shared_ptr<open3d::pipelines::registration::Feature>>;

    std::shared_ptr<open3d::geometry::PointCloud> m_modelPcCloud = nullptr;
    std::shared_ptr<open3d::pipelines::registration::Feature> m_modelFpfh = nullptr;
    std::shared_ptr<open3d::geometry::PointCloud> m_sourcePcCloud = nullptr;
    std::shared_ptr<open3d::pipelines::registration::Feature> m_sourceFpfh = nullptr;
};

auto DCCalibrator::Impl::set_model_cloud(const DCCalibratorSettings &settings, size_t idModel, const DCModelSettings &modelTr) -> bool{

    // retrieve model cloud
    modelCloud = settings.useProcessed ?
                     clientsData[idModel].processedCloud :
                     clientsData[idModel].calibrationCloud;

    // apply transformation
    modelCloud.vertices.apply_transformation(modelTr.compute_full_transformation());

    // process features
    m_modelPcCloud = nullptr;
    m_modelFpfh    = nullptr;
    std::tie(m_modelPcCloud, m_modelFpfh) = preprocess_features(settings, modelCloud);
    if(!m_modelPcCloud->HasPoints()){
        return false;
    }

    return true;
}

auto DCCalibrator::Impl::set_source_cloud(const DCCalibratorSettings &settings, size_t idSource, const DCModelSettings &sourceTr) -> bool{

    // retrieve source cloud
    sourceCloud = settings.useProcessed ?
                      clientsData[idSource].processedCloud :
                      clientsData[idSource].calibrationCloud;

    // apply transformation
    sourceCloud.vertices.apply_transformation(sourceTr.compute_full_transformation());

    // process features
    m_sourcePcCloud = nullptr;
    m_sourceFpfh    = nullptr;
    std::tie(m_sourcePcCloud, m_sourceFpfh) = preprocess_features(settings, sourceCloud);

    if(!m_sourcePcCloud->HasPoints()){
        return false;
    }

    return true;
}

auto DCCalibrator::Impl::process_cloud(const DCCalibratorSettings &settings, const geo::ColorCloud &cloud) -> ColorCloud{

    if(!cloud.has_vertices()){
        return {};
    }

    // process cloud frame
    auto processedCloud = cloud;
    if(settings.removeOutliers){
        processedCloud.remove_outliers(processedCloud.vertices.mean_position(), settings.maxDistanceOutlier);
    }

    if(settings.downSample){

        auto pcdDown = convert_to_opend3d_pc(processedCloud)->VoxelDownSample(settings.downSampleVoxelSize);
        processedCloud.resize(pcdDown->points_.size());
        for(size_t ii = 0; ii < pcdDown->points_.size(); ++ii){
            processedCloud.vertices[ii] = {
                static_cast<float>(pcdDown->points_[ii].x()),
                static_cast<float>(pcdDown->points_[ii].y()),
                static_cast<float>(pcdDown->points_[ii].z())
            };
        }
    }

    if(settings.computeSphereCenter){
        auto mean = processedCloud.vertices.mean_position();
        mean.z() = processedCloud.vertices.min_z() + settings.ballRay;
        processedCloud = ColorCloud(mean, processedCloud.colors[0]);
    }

    return processedCloud;
}


auto DCCalibrator::Impl::preprocess_features(const DCCalibratorSettings &settings, const ColorCloud &cloud) ->

    std::tuple<std::shared_ptr<open3d::geometry::PointCloud>, std::shared_ptr<open3d::pipelines::registration::Feature>>{

    // create point cloud
    auto pcd = Impl::convert_to_opend3d_pc(cloud);

    // normals
    pcd->EstimateNormals(open3d::geometry::KDTreeSearchParamHybrid(settings.normalRadius, settings.normalNeighbours));

    // compute features
    auto pcd_fpfh = open3d::pipelines::registration::ComputeFPFHFeature(
        *pcd,
        open3d::geometry::KDTreeSearchParamHybrid(
            settings.ransac.FPFHFeatureRadius,
            settings.ransac.FPFHFeatureMaxNeghbours
        )
    );

    return std::make_tuple(pcd, pcd_fpfh);
}


auto DCCalibrator::Impl::do_RANSAC(const DCCalibratorSettings &settings, unsigned int seed) const -> open3d::pipelines::registration::RegistrationResult{

    // set registration parameters
    auto correspondence_checker_edge_length = open3d::pipelines::registration::CorrespondenceCheckerBasedOnEdgeLength(settings.ransac.similaritiesThreshold);
    auto correspondence_checker_distance    = open3d::pipelines::registration::CorrespondenceCheckerBasedOnDistance(settings.ransac.distanceThreshold);
    std::vector<std::reference_wrapper<const open3d::pipelines::registration::CorrespondenceChecker>> correspondence_checker ={
        correspondence_checker_edge_length,
        correspondence_checker_distance
    };

    return open3d::pipelines::registration::RegistrationRANSACBasedOnFeatureMatching(
        *m_sourcePcCloud,   // source
        *m_modelPcCloud,    // target
        *m_sourceFpfh,      // source feature
        *m_modelFpfh,       // target feature
        settings.ransac.mutualFilter,   // mutual filter
        settings.ransac.maxDistanceCorr, // max correspondance distance
        open3d::pipelines::registration::TransformationEstimationPointToPoint(false),
        settings.ransac.N,      // ransac number
        correspondence_checker, //
        open3d::pipelines::registration::RANSACConvergenceCriteria(settings.ransac.maxIteration, settings.ransac.confidence)//,
        // seed
    );
}

auto DCCalibrator::Impl::do_ICP(const DCCalibratorSettings &settings) const -> open3d::pipelines::registration::RegistrationResult{
    return open3d::pipelines::registration::RegistrationICP(
        *m_sourcePcCloud,
        *m_modelPcCloud,
        settings.icpMaxDistanceCorr,
        Eigen::Matrix4d::Identity(),
        open3d::pipelines::registration::TransformationEstimationPointToPoint(false),
        open3d::pipelines::registration::ICPConvergenceCriteria(settings.icpRelativeFitness, settings.icpRelativeRmse, settings.icpMaxIteration)
    );
}

auto DCCalibrator::Impl::best_registration_results(const std::vector<std::tuple<open3d::pipelines::registration::RegistrationResult, double>> &results) -> std::tuple<open3d::pipelines::registration::RegistrationResult, double>{

    auto bestResult = results[0];
    double rmseMin     = 10000.0;
    double distanceMin = 10000.0;

    for(const auto &result : results){
        auto regRes   = std::get<0>(result);
        auto distance = std::get<1>(result);

        if(regRes.inlier_rmse_ > 0.0){
            if(almost_equal(regRes.inlier_rmse_, rmseMin)){
                if(distance < distanceMin){
                    distanceMin = distance;
                    bestResult  = result;
                }
            }else if(regRes.inlier_rmse_ < rmseMin){
                rmseMin     = regRes.inlier_rmse_;
                distanceMin = distance;
                bestResult  = result;
            }
        }
    }
    return bestResult;
}

auto DCCalibrator::Impl::compute_new_model(const open3d::pipelines::registration::RegistrationResult &result, const Mat4f &model) -> Mat4f{
    return from_eigen_mat(result.transformation_) * model;
}

auto DCCalibrator::Impl::convert_to_opend3d_pc(const ColorCloud &cloud) -> std::shared_ptr<open3d::geometry::PointCloud>{

    auto pcd = std::make_shared<open3d::geometry::PointCloud>();
    pcd->points_.reserve(cloud.size());
    for(size_t ii = 0; ii < cloud.size(); ++ii){
        pcd->points_.emplace_back(
            static_cast<double>(cloud.vertices[ii].x()),
            static_cast<double>(cloud.vertices[ii].y()),
            static_cast<double>(cloud.vertices[ii].z())
        );
    }
    return pcd;
}

auto DCCalibrator::Impl::compute_charmfer_distance_between_with_current_source(const Mat4f &model) -> double {

    // The chamfer command is used to compute the Chamfer distance between two point clouds.
    // The Chamfer distance is computed by summing the squared distances between nearest neighbor correspondences of two point clouds.
    auto sc = sourceCloud;
    sc.vertices.apply_transformation(model);

    auto pcSourceCloud = convert_to_opend3d_pc(sc);
    double sum = 0.0;
    auto distances = m_modelPcCloud->ComputePointCloudDistance(*pcSourceCloud);
    for(auto distance : distances){
        sum += distance*distance;
    }
    return sum;
}

DCCalibrator::DCCalibrator(): i(std::make_unique<Impl>()){}

DCCalibrator::~DCCalibrator(){}

auto DCCalibrator::initialize(size_t nbGrabbers) -> void{

    settings.initialize(nbGrabbers);

    i->clientsData.resize(nbGrabbers);
    for(size_t ii = 0; ii < i->clientsData.size(); ++ii){
        i->clientsData[ii].id = ii;
        i->calibrations.push_back(std::nullopt);
        states.nbFramesRegistered.push_back(0);
    }

}

auto DCCalibrator::nb_frames_registered(size_t idGrabber) const noexcept -> size_t{
    return i->clientsData[idGrabber].frames.size();
}

auto DCCalibrator::size_all_calibration_cloud() const noexcept -> size_t{
    size_t total = 0;
    for(const auto &clientData : i->clientsData){
        total += clientData.calibrationCloud.size();
    }
    return total;
}

auto DCCalibrator::calibration_client_data(size_t idGrabber) const -> const DCCalibratorClientData*{
    if(idGrabber < i->clientsData.size()){
        return &i->clientsData.at(idGrabber);
    }
    return nullptr;
}

auto DCCalibrator::add_frame(size_t idCloud, std::shared_ptr<cam::DCFrame> frame) -> void{

    if(!states.isRegistering){
        return;
    }

    if(states.elapsedTime.count() <= settings.timeToWaitBeforeRegisteringMs){
        return;
    }

    // retrieve cloud
    ColorCloud &cloud = frame->cloud;
    if(cloud.size() == 0){
        return;
    }

    auto &clientData = i->clientsData[idCloud];
    if(clientData.frames.size() > 0){
        if(clientData.frames.back()->idCapture == frame->idCapture){
            Logger::error("[DCCalibrator] Previous cloud already added.\n");
            return;
        }
    }

    states.nbFramesRegistered[idCloud]++;
    clientData.frames.push_back(frame);

    add_to_calibration_cloud(idCloud, cloud);
    add_to_proccessed_cloud(idCloud, i->process_cloud(settings, cloud));
    trigger_data_updated();
}

auto DCCalibrator::process_all_frames() -> void{

    for(auto &clientData : i->clientsData){
        clientData.calibrationCloud.clear();
        clientData.processedCloud.clear();

        for(auto &frame : clientData.frames){
            add_to_calibration_cloud(clientData.id, frame->cloud);
            add_to_proccessed_cloud(clientData.id, i->process_cloud(settings, frame->cloud));
        }
    }
    trigger_data_updated();
}

auto DCCalibrator::calibrate(const std::vector<DCModelSettings> &models) -> bool{

    size_t idModel = settings.modelSelectionId;
    if(states.nbFramesRegistered[idModel] == 0){
        Logger::error(std::format("[DCCalibrator] No frames registered for model with id [{}], calibration aborted.\n",idModel));
        return false;
    }

    std::vector<size_t> idSources;
    if(settings.sourceSelectionId == settings.models.size()){

        for(size_t ii = 0; ii < settings.models.size(); ++ii){
            if(ii != idModel){
                if(states.nbFramesRegistered[ii] == 0){
                    Logger::warning(std::format("[DCCalibrator] No frames registered for source with id [{}], source removed from calibration list.\n", ii));
                    continue;
                }
                idSources.push_back(ii);
                Logger::message(std::format("Add source [{}] to list: \n", ii));
            }
        }
    }else{

        if(settings.sourceSelectionId != idModel){
            idSources.push_back(settings.sourceSelectionId);
        }else{
            Logger::error("[DCCalibrator] Source must be different from model, calibration aborted.\n");
            return false;
        }
    }

    if(idSources.size() == 0){
        Logger::error("[DCCalibrator] No source set, calibration aborted.\n");
        return false;
    }

    // Logger::message(std::format("Model {}\n", idModel));
    // for(auto &s : idSources){
    //     Logger::message(std::format("source {}\n", s));
    // }

    if(!i->set_model_cloud(settings, idModel, models[idModel])){
        Logger::error("[DCCalibrator] Model cloud is empty, calibration aborted.\n");
        return false;
    }

    Logger::message("Start calibration.\n");
    for(auto &calibration : i->calibrations){
        calibration = std::nullopt;
    }

    for(auto idSource : idSources){

        Logger::message(std::format("Calibrate with source [{}] and model [{}].\n", idSource, idModel));

        if(idSource == idModel){
            Logger::error("[DCCalibrator] Id model and source are the same, source ignored.\n");
            continue;
        }

        if(!i->set_source_cloud(settings, idSource, models[idSource])){
            Logger::error("[DCCalibrator] Source cloud is empty, source ignored.\n");
            return false;
        }

        // do registration
        Logger::message(std::format("Register cloud [{}]\n",idSource));
        std::vector<std::tuple<open3d::pipelines::registration::RegistrationResult, double>> results;
        if(settings.doRansac){
            // RANSAC
            std::uniform_int_distribution<> dist(0, 100000);
            for(size_t ii = 0; ii < settings.ransac.nbTries; ++ii){
                auto result    = i->do_RANSAC(settings, dist(i->gen));
                auto resMat    = from_eigen_mat(result.transformation_);
                auto distance  = i->compute_charmfer_distance_between_with_current_source(resMat);
                results.push_back({result,distance});
                Logger::message(std::format("RANSAC [ID TRY:{}] [Fitness:{:.4f}] [RMSE{:.4f}] [Dist:{:.4f}] [Res:{}]\n", ii, result.fitness_, result.inlier_rmse_, distance, tool::to_string(resMat)));
            }
        }else{ // ICP
            auto result    = i->do_ICP(settings);
            auto resMat    = from_eigen_mat(result.transformation_);
            auto distance  = i->compute_charmfer_distance_between_with_current_source(resMat);
            results.push_back({result,distance});
            Logger::message(std::format("ICP [Fitness:{:.4f}] [RMSE{:.4f}] [Dist:{:.4f}] [Res:{}]\n", result.fitness_, result.inlier_rmse_, distance, tool::to_string(resMat)));
        }

        // retrieve best calibration
        auto bestResult =  Impl::best_registration_results(results);
        Logger::message(std::format("Best [Fitness:{:.4f}] [RMSE{:.4f}] [Dist:{:.4f}] [Res:{}]\n", std::get<0>(bestResult).fitness_, std::get<0>(bestResult).inlier_rmse_, std::get<1>(bestResult), tool::to_string(from_eigen_mat(std::get<0>(bestResult).transformation_))));

        // update models with new calibration
        DCModelSettings newM;
        newM.transformation = models[idSource].compute_full_transformation() * from_eigen_mat(std::get<0>(bestResult).transformation_);

        // send calibration
        i->calibrations[idSource] = newM;
        new_calibration_signal(idSource, i->calibrations[idSource].value());

        Logger::message(std::format("Calibration updated for cloud [{}]\n", idSource));
    }
    Logger::message("End calibration.\n");

    return true;
}

auto DCCalibrator::validate_calibration() -> void{

    for(size_t ii = 0; ii < i->calibrations.size(); ++ii){
        if(i->calibrations[ii].has_value()){
            validated_calibration_signal(ii, i->calibrations[ii].value());
        }
    }
}

auto DCCalibrator::update_settings(const DCCalibratorSettings &calibratorS) -> void{
    settings = calibratorS;
}

auto DCCalibrator::trigger_data_updated() -> void{
    data_updated_signal(settings.modelSelectionId, settings.sourceSelectionId, &i->clientsData);
}

auto DCCalibrator::start_registering() -> void{

    if((states.startTime == std::chrono::milliseconds(0)) || (states.elapsedTime.count() < settings.durationMs)){
        states.startTime      = tool::Time::milliseconds_since_epoch() - states.elapsedTime;
        states.isRegistering  = true;
    }
}

auto DCCalibrator::stop_registering() -> void{
    states.isRegistering = false;
}

auto DCCalibrator::reset_registering() -> void{

    // reset data
    for(auto &clientData : i->clientsData){
        clientData.clean();
    }
    for(auto &calibration : i->calibrations){
        calibration = std::nullopt;
    }
    trigger_data_updated();

    // reset states
    states.startTime   = tool::Time::milliseconds_since_epoch();
    states.elapsedTime = std::chrono::milliseconds(0);

    std::fill(std::begin(states.nbFramesRegistered), std::end(states.nbFramesRegistered), 0);
}

auto DCCalibrator::update() -> void{

    if(states.isRegistering){
        states.elapsedTime = (tool::Time::milliseconds_since_epoch() - states.startTime);
    }
    if(states.elapsedTime.count() > settings.durationMs){
        states.isRegistering = false;
    }

    if(states.isRegistering && size_all_calibration_cloud() > 5000000){
        states.isRegistering = false;
        Logger::message("Max calibration cloud size reached.\n");
    }
}


auto DCCalibrator::add_to_calibration_cloud(size_t idCloud, const geo::ColorCloud &cloud) -> void {
    i->clientsData[idCloud].calibrationCloud.merge(cloud);
}

auto DCCalibrator::add_to_proccessed_cloud(size_t idCloud, const geo::ColorCloud &cloud) -> void{
    i->clientsData[idCloud].processedCloud.merge(cloud);
}

