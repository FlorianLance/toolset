
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
#include "camera/settings/dc_model_settings.hpp"

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

    auto process_cloud(const geo::ColoredCloudData &cloud) -> geo::ColoredCloudData;
    auto set_model_cloud(size_t idModel, const DCModelSettings &modelTr) -> bool;
    auto set_source_cloud(size_t idSource, const DCModelSettings &sourceTr) -> bool;
    auto do_RANSAC(unsigned int seed) const -> open3d::pipelines::registration::RegistrationResult;
    auto do_ICP() const -> open3d::pipelines::registration::RegistrationResult;

    static auto best_registration_results(const std::vector<std::tuple<open3d::pipelines::registration::RegistrationResult, double>> &results) -> std::tuple<open3d::pipelines::registration::RegistrationResult, double>;
    static auto compute_new_model(const open3d::pipelines::registration::RegistrationResult &result, const geo::Mat4f &model) -> geo::Mat4f;
    static auto convert_to_opend3d_pc(const ColoredCloudData &cloud) -> std::shared_ptr<open3d::geometry::PointCloud>;

    DCCalibratorSettings settings;
    std::vector<DCCalibratorGrabberData> grabbersData;
    geo::ColoredCloudData modelCloud;
    geo::ColoredCloudData sourceCloud;

    auto compute_charmfer_distance_between_with_current_source(const geo::Mat4f &model){

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

    std::mt19937 gen;

private:

    auto preprocess_features(const ColoredCloudData &cloud) -> std::tuple<std::shared_ptr<open3d::geometry::PointCloud>, std::shared_ptr<open3d::pipelines::registration::Feature>>;

    std::shared_ptr<open3d::geometry::PointCloud> m_modelPcCloud = nullptr;
    std::shared_ptr<open3d::pipelines::registration::Feature> m_modelFpfh = nullptr;
    std::shared_ptr<open3d::geometry::PointCloud> m_sourcePcCloud = nullptr;
    std::shared_ptr<open3d::pipelines::registration::Feature> m_sourceFpfh = nullptr;
};

auto DCCalibrator::Impl::set_model_cloud(size_t idModel, const DCModelSettings &modelTr) -> bool{

    // retrieve model cloud
    modelCloud = settings.useProcessed ?
        grabbersData[idModel].processedCloud :
        grabbersData[idModel].calibrationCloud;

    // apply transformation
    modelCloud.vertices.apply_transformation(modelTr.compute_full_transformation());

    // process features
    m_modelPcCloud = nullptr;
    m_modelFpfh    = nullptr;
    std::tie(m_modelPcCloud, m_modelFpfh) = preprocess_features(modelCloud);
    if(!m_modelPcCloud->HasPoints()){
        return false;
    }

    return true;
}

auto DCCalibrator::Impl::set_source_cloud(size_t idSource, const DCModelSettings &sourceTr) -> bool{

    // retrieve source cloud
    sourceCloud = settings.useProcessed ?
        grabbersData[idSource].processedCloud :
        grabbersData[idSource].calibrationCloud;

    // apply transformation
    sourceCloud.vertices.apply_transformation(sourceTr.compute_full_transformation());

    // process features
    m_sourcePcCloud = nullptr;
    m_sourceFpfh    = nullptr;
    std::tie(m_sourcePcCloud, m_sourceFpfh) = preprocess_features(sourceCloud);

    if(!m_sourcePcCloud->HasPoints()){
        return false;
    }

    return true;
}

auto DCCalibrator::Impl::process_cloud(const geo::ColoredCloudData &cloud) -> ColoredCloudData{

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
        processedCloud = ColoredCloudData(mean, processedCloud.colors[0]);
    }

    return processedCloud;
}


auto DCCalibrator::Impl::preprocess_features(const ColoredCloudData &cloud) ->

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


auto DCCalibrator::Impl::do_RANSAC(unsigned int seed) const -> open3d::pipelines::registration::RegistrationResult{

    // set registration parameters
    auto correspondence_checker_edge_length = open3d::pipelines::registration::CorrespondenceCheckerBasedOnEdgeLength(settings.ransac.similaritiesThreshold);
    auto correspondence_checker_distance    = open3d::pipelines::registration::CorrespondenceCheckerBasedOnDistance(settings.ransac.distanceThreshold);
    std::vector<std::reference_wrapper<const open3d::pipelines::registration::CorrespondenceChecker>> correspondence_checker ={
        correspondence_checker_edge_length,
        correspondence_checker_distance
    };

    return open3d::pipelines::registration::RegistrationRANSACBasedOnFeatureMatching(
        *m_sourcePcCloud, *m_modelPcCloud, *m_sourceFpfh, *m_modelFpfh,
        settings.ransac.mutualFilter,
        settings.ransac.maxDistanceCorr,
        open3d::pipelines::registration::TransformationEstimationPointToPoint(false),
        settings.ransac.N,
        correspondence_checker,
        open3d::pipelines::registration::RANSACConvergenceCriteria(settings.ransac.maxIteration, settings.ransac.confidence),
        seed
    );
}

auto DCCalibrator::Impl::do_ICP() const -> open3d::pipelines::registration::RegistrationResult{
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

auto DCCalibrator::Impl::convert_to_opend3d_pc(const ColoredCloudData &cloud) -> std::shared_ptr<open3d::geometry::PointCloud>{

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

DCCalibrator::DCCalibrator(): m_p(std::make_unique<Impl>()){}

DCCalibrator::~DCCalibrator(){}

auto DCCalibrator::initialize(size_t nbGrabbers) -> void{

    m_p->grabbersData.resize(nbGrabbers);
    for(size_t ii = 0; ii < m_p->grabbersData.size(); ++ii){
        m_p->grabbersData[ii].id = ii;
        m_calibrations.push_back(std::nullopt);
        m_states.nbFramesRegistered.push_back(0);
    }

}

auto DCCalibrator::nb_frames_registered(size_t idGrabber) const noexcept -> size_t{
    return m_p->grabbersData[idGrabber].frames.size();
}

auto DCCalibrator::size_all_calibration_cloud() const noexcept -> size_t{
    size_t total = 0;
    for(const auto &grabberData : m_p->grabbersData){
        total += grabberData.calibrationCloud.size();
    }
    return total;
}

auto DCCalibrator::calibration_grabber_data(size_t idGrabber) const -> const DCCalibratorGrabberData*{
    if(idGrabber < m_p->grabbersData.size()){
        return &m_p->grabbersData.at(idGrabber);
    }
    return nullptr;
}

auto DCCalibrator::add_frame(size_t idCloud, std::shared_ptr<cam::DCFrame> frame) -> void{

    if(!m_states.isRegistering){
        return;
    }

    if(m_states.elapsedTime.count() <= m_p->settings.timeToWaitBeforeRegisteringMs){
        return;
    }

    // retrieve cloud
    ColoredCloudData &cloud = frame->cloud;
    if(cloud.size() == 0){
        return;
    }

    auto &grabberData = m_p->grabbersData[idCloud];
    if(grabberData.frames.size() > 0){
        if(grabberData.frames.back()->idCapture == frame->idCapture){
            Logger::error("[DCCalibrator] Previous cloud already added.\n");
            return;
        }
    }

    m_states.nbFramesRegistered[idCloud]++;
    grabberData.frames.push_back(frame);

    add_to_calibration_cloud(idCloud, cloud);
    add_to_proccessed_cloud(idCloud, m_p->process_cloud(cloud));
    send_data_updated_signal();
}

auto DCCalibrator::process_all_frames() -> void{

    for(auto &grabberData : m_p->grabbersData){
        grabberData.calibrationCloud.clear();
        grabberData.processedCloud.clear();

        for(auto &frame : grabberData.frames){
            add_to_calibration_cloud(grabberData.id, frame->cloud);
            add_to_proccessed_cloud(grabberData.id, m_p->process_cloud(frame->cloud));
        }
    }
    send_data_updated_signal();
}

auto DCCalibrator::calibrate(const std::vector<DCModelSettings> &models) -> bool{

    size_t idModel = m_p->settings.modelSelectionId;
    if(m_states.nbFramesRegistered[idModel] == 0){
        Logger::error(std::format("[DCCalibrator] No frames registered for model with id [{}], calibration aborted.\n",idModel));
        return false;
    }

    std::vector<size_t> idSources;
    if(m_p->settings.sourceSelectionId == m_p->settings.models.size()){

        for(size_t ii = 0; ii < m_p->settings.models.size(); ++ii){
            if(ii != idModel){
                if(m_states.nbFramesRegistered[ii] == 0){
                    Logger::warning(std::format("[DCCalibrator] No frames registered for source with id [{}], source removed from calibration list.\n", ii));
                    continue;
                }
                idSources.push_back(ii);
            }
        }
    }else{

        if(m_p->settings.sourceSelectionId != idModel){
            idSources.push_back(m_p->settings.sourceSelectionId);
        }else{
            Logger::error("[DCCalibrator] Source must be different from model, calibration aborted.\n");
            return false;
        }
    }

    if(idSources.size() == 0){
        Logger::error("[DCCalibrator] No source set, calibration aborted.\n");
        return false;
    }

    Logger::message(std::format("model {}\n", idModel));
    for(auto &s : idSources){
        Logger::message(std::format("source {}\n", s));
    }

    if(!m_p->set_model_cloud(idModel, models[idModel])){
        Logger::error("[DCCalibrator] Model cloud is empty, calibration aborted.\n");
        return false;
    }

    Logger::message("Start calibration.\n");
    for(auto &calibration : m_calibrations){
        calibration = std::nullopt;
    }

    for(auto idSource : idSources){

        if(idSource == idModel){
            Logger::error("[DCCalibrator] Id model and source are the same, source ignored.\n");
            continue;
        }

        if(!m_p->set_source_cloud(idSource, models[idSource])){
            Logger::error("[DCCalibrator] Source cloud is empty, source ignored.\n");
            return false;
        }

        // do registration
        Logger::message(std::format("Register cloud [{}]\n",idSource));
        std::vector<std::tuple<open3d::pipelines::registration::RegistrationResult, double>> results;
        if(m_p->settings.doRansac){
            // RANSAC
            std::uniform_int_distribution<> dist(0, 100000);
            for(size_t ii = 0; ii < m_p->settings.ransac.nbTries; ++ii){
                auto result    = m_p->do_RANSAC(dist(m_p->gen));
                auto resMat    = from_eigen_mat(result.transformation_);
                auto distance  = m_p->compute_charmfer_distance_between_with_current_source(resMat);
                results.push_back({result,distance});
                Logger::message(std::format("RANSAC [ID TRY:{}] [Fitness:{:.4f}] [RMSE{:.4f}] [Dist:{:.4f}] [Res:{}]\n", ii, result.fitness_, result.inlier_rmse_, distance, tool::to_string(resMat)));
            }
        }else{ // ICP
            auto result    = m_p->do_ICP();
            auto resMat    = from_eigen_mat(result.transformation_);
            auto distance  = m_p->compute_charmfer_distance_between_with_current_source(resMat);
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
        m_calibrations[idSource] = newM;
        new_calibration_signal(idSource, m_calibrations[idSource].value());

        Logger::message(std::format("Calibration updated for cloud [{}]\n", idSource));
    }
    Logger::message("End calibration.\n");

    return true;
}

auto DCCalibrator::validate_calibration() -> void{

    for(size_t ii = 0; ii < m_calibrations.size(); ++ii){
        if(m_calibrations[ii].has_value()){
            validated_calibration_signal(ii, m_calibrations[ii].value());
        }
    }
}

auto DCCalibrator::update_settings(const DCCalibratorSettings &calibrationS) -> void{
    m_p->settings = calibrationS;
}

auto DCCalibrator::send_data_updated_signal() -> void{
    data_updated_signal(m_p->settings.modelSelectionId, m_p->settings.sourceSelectionId, &m_p->grabbersData);
}

auto DCCalibrator::start_registering() -> void{

    if((m_states.startTime == std::chrono::milliseconds(0)) || (m_states.elapsedTime.count() < m_p->settings.durationMs)){
        m_states.startTime      = tool::Time::milliseconds_since_epoch() - m_states.elapsedTime;
        m_states.isRegistering  = true;
        states_updated_signal(m_states);
    }
}

auto DCCalibrator::stop_registering() -> void{
    m_states.isRegistering = false;
    states_updated_signal(m_states);
}

auto DCCalibrator::reset_registering() -> void{

    // reset data
    for(auto &grabberData : m_p->grabbersData){
        grabberData.clean();
    }
    for(auto &calibration : m_calibrations){
        calibration = std::nullopt;
    }
    send_data_updated_signal();

    // reset states
    m_states.startTime   = tool::Time::milliseconds_since_epoch();
    m_states.elapsedTime = std::chrono::milliseconds(0);

    std::fill(std::begin(m_states.nbFramesRegistered), std::end(m_states.nbFramesRegistered), 0);
    states_updated_signal(m_states);        
}

auto DCCalibrator::update_time() -> void{

    if(m_states.isRegistering){
        m_states.elapsedTime = (tool::Time::milliseconds_since_epoch() - m_states.startTime);
    }
    if(m_states.elapsedTime.count() > m_p->settings.durationMs){
        m_states.isRegistering = false;
    }

    if(m_states.isRegistering && size_all_calibration_cloud() > 5000000){
        m_states.isRegistering = false;
        Logger::message("Max calibration cloud size reached.\n");
    }

    states_updated_signal(m_states);
}


auto DCCalibrator::add_to_calibration_cloud(size_t idCloud, const geo::ColoredCloudData &cloud) -> void {
    m_p->grabbersData[idCloud].calibrationCloud.merge(cloud);
}

auto DCCalibrator::add_to_proccessed_cloud(size_t idCloud, const geo::ColoredCloudData &cloud) -> void{
    m_p->grabbersData[idCloud].processedCloud.merge(cloud);
}

