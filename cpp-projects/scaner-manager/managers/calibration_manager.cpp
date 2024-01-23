
/*******************************************************************************
** Toolset-scaner-manager                                                     **
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

#include "calibration_manager.hpp"

// open3D
#include "open3D/Open3D.h"

// local
#include "utility/vector.hpp"

// qt-utility
#include "qt_io.hpp"
#include "qt_logger.hpp"

using namespace tool;
using namespace tool::ui;
using namespace tool::geo;
using namespace tool::cam;
using namespace open3d;

struct CalibrationManager::Impl{
    std::vector<GrabberControllerUP> *grabbers = nullptr;
};


CalibrationManager::CalibrationManager(std::vector<GrabberControllerUP> *grabbers): m_p(std::make_unique<Impl>()){
    m_p->grabbers = grabbers;
}

CalibrationManager::~CalibrationManager(){}


std::tuple<std::shared_ptr<geometry::PointCloud>, std::shared_ptr<pipelines::registration::Feature>> preprocess_point_cloud(const std::vector<Pt3f> &registeredPoints) {

    auto pcd = std::make_shared<geometry::PointCloud>();
    pcd->points_.reserve(registeredPoints.size());
    for(const auto &pt : registeredPoints){
        pcd->points_.emplace_back(Eigen::Vector3d{
            static_cast<double>(pt.x()),
            static_cast<double>(pt.y()),
            static_cast<double>(pt.z())
        });
    }

    double voxelSize = 0.05; // resolution of the voxel grid, smaller values lead to denser output point cloud
    auto pcd_down = pcd->VoxelDownSample(voxelSize);
    double radius = 0.1; // radius of the search
    int maxNeighbours = 30; // max neighbours to be searched
    pcd_down->EstimateNormals(open3d::geometry::KDTreeSearchParamHybrid(radius, maxNeighbours));

    radius = 0.25;
    maxNeighbours = 100;
    auto pcd_fpfh = pipelines::registration::ComputeFPFHFeature(*pcd_down, open3d::geometry::KDTreeSearchParamHybrid(0.25, 100));

    return std::make_tuple(pcd_down, pcd_fpfh);
}

void save_cloud(QString path, std::shared_ptr<geometry::PointCloud> cloud){

    if(path.length() == 0){
        return;
    }

    std::ofstream file;
    std::string name(path.toStdString());
    file.open(name);
    if(file.is_open() && cloud->points_.size() > 0){
        for(size_t ii = 0; ii < cloud->points_.size(); ++ii){
            const auto &v = cloud->points_[ii];
            file << "v " << v.x() << " " << v.y() << " " << v.z() << "\n";
        }
        for(size_t ii = 0; ii < cloud->normals_.size(); ++ii){
            const auto &n = cloud->normals_[ii];
            file << "vn " << n.x() << " " << n.y() << " " << n.z() << "\n";
        }
    }
    file.close();
}


std::optional<geo::Mat4d> CalibrationManager::calibrate_open3D(bool useProcessed, size_t indexSource, size_t indexTarget){

    QtLogger::message("Calibrate opend3d");

    const bool removeOutliers = true;
    const size_t nbNeighborsOutliers = 20;
    const double stdRatioOutliers = 2.0;


    // retrieve grabbers
    auto sourceGrabber = (*m_p->grabbers)[indexSource].get();
    auto targetGrabber = (*m_p->grabbers)[indexTarget].get();

    std::vector<geo::Pt3f> *sourcePoints = nullptr, *sourceColors = nullptr;
    std::vector<geo::Pt3f> *targetPoints = nullptr, *targetColors = nullptr;
    if(useProcessed){
        sourcePoints = &sourceGrabber->processedPoints;
        targetPoints = &targetGrabber->processedPoints;
        sourceColors = &sourceGrabber->processedColors;
        targetColors = &targetGrabber->processedColors;
    }else{
        sourcePoints = &sourceGrabber->flattenRegisteredPoints;
        targetPoints = &targetGrabber->flattenRegisteredPoints;
        sourceColors = &sourceGrabber->flattenRegisteredColors;
        targetColors = &targetGrabber->flattenRegisteredColors;
    }

    if(sourcePoints->size() == 0){
        QtLogger::error(QSL("No source points defined for calibration from index ") % QString::number(indexSource) % QSL("\n"));
        return {};
    }
    if(targetPoints->size() == 0){
        QtLogger::error(QSL("No target points defined for calibration from index ") % QString::number(indexTarget) % QSL("\n"));
        return {};
    }

    // apply transformations
    std::vector<geo::Pt3f> trSourcePoints(sourcePoints->size());
    std::vector<geo::Pt3f> trTargetPoints(targetPoints->size());
    const auto trSource = sourceGrabber->get_model_matrix().conv<float>();
    for(size_t ii = 0; ii < sourcePoints->size(); ++ii){
        trSourcePoints[ii] = trSource.multiply_point(geo::to_pt4((*sourcePoints)[ii],1.f)).xyz();
    }
    const auto trTarget = targetGrabber->get_model_matrix().conv<float>();
    for(size_t ii = 0; ii < targetPoints->size(); ++ii){
        trTargetPoints[ii] = trTarget.multiply_point(geo::to_pt4((*targetPoints)[ii],1.f)).xyz();
    }

    // init Open3D data
    std::shared_ptr<geometry::PointCloud> source, target;
    std::shared_ptr<pipelines::registration::Feature> source_fpfh, target_fpfh;
    std::tie(source, source_fpfh) = preprocess_point_cloud(trSourcePoints);
    std::tie(target, target_fpfh) = preprocess_point_cloud(trTargetPoints);

    if(removeOutliers){
        const size_t sizePtsSource = source->points_.size();
        const size_t sizePtsTarget = target->points_.size();
        source = std::get<0>(source->RemoveStatisticalOutliers(nbNeighborsOutliers, stdRatioOutliers));
        target = std::get<0>(target->RemoveStatisticalOutliers(nbNeighborsOutliers, stdRatioOutliers));

        // RemoveRadiusOutliers

        QtLogger::message(QSL("Source points outliers removed number:") % QString::number((sizePtsSource-source->points_.size()))
                            % QSL(" off ") % QString::number(sizePtsSource) % QSL("\n"));
        QtLogger::message(QSL("Target points outliers removed number:") % QString::number((sizePtsTarget-target->points_.size()))
                            % QSL(" off ") % QString::number(sizePtsTarget) % QSL("\n"));
    }

    if(!source->HasPoints()){
        QtLogger::error(QSL("Source voxels downsampling is empty: ") % QString::number(indexSource) % QSL("\n"));
        return {};
    }
    if(!target->HasPoints()){
        QtLogger::error(QSL("Target voxels downsampling is empty: ") % QString::number(indexTarget) % QSL("\n"));
        return {};
    }

    // set registration parameters
    double similaritiesThreshold = 0.9; // specify the threshold within which 2 arbitrary edges are similar
    auto correspondence_checker_edge_length = pipelines::registration::CorrespondenceCheckerBasedOnEdgeLength(similaritiesThreshold);
    double distanceThreshold = 0.075;
    auto correspondence_checker_distance    = pipelines::registration::CorrespondenceCheckerBasedOnDistance(distanceThreshold);
    // auto correspondence_checker_normal      = registration::CorrespondenceCheckerBasedOnNormal(0.52359878);

    std::vector<std::reference_wrapper<const pipelines::registration::CorrespondenceChecker>> correspondence_checker;
    correspondence_checker.push_back(correspondence_checker_edge_length);
    correspondence_checker.push_back(correspondence_checker_distance);
    // correspondence_checker.push_back(correspondence_checker_normal);

    // do registration
    bool mutualFilter = true;
    auto registration_result = pipelines::registration::RegistrationRANSACBasedOnFeatureMatching(
        *source, *target, *source_fpfh, *target_fpfh,
        mutualFilter,
        sourceGrabber->maxDistanceCorr, //0.075,//0.075, // max_correspondence_distance
        pipelines::registration::TransformationEstimationPointToPoint(false),
        3, // 4 ransac_n
        correspondence_checker,
        pipelines::registration::RANSACConvergenceCriteria(100000, 0.999) // max_iter, max_validation
    );

    // apply transformation to source   
    Mat4d trD(true);
    for(int ii = 0; ii < 4; ++ii){
        for(int jj = 0; jj < 4; ++jj){
            trD.at(ii,jj) = static_cast<double>(registration_result.transformation_(ii,jj));
        }
    }
    trD = sourceGrabber->get_model_matrix() * transpose(trD);

    QtLogger::message(QSL("[fitness]: ") % QString::number(registration_result.fitness_) % QSL("\n"));
    QtLogger::message(QSL("[inlier_rmse]: ") % QString::number(registration_result. inlier_rmse_) % QSL("\n"));

    return trD;
}




//// super4pcss
//#include "super4pcs/algorithms/4pcs.h"
//#include "super4pcs/algorithms/super4pcs.h"
//#include "super4pcs/io/io.h"
//#include "super4pcs/utils/geometry.h"

//using namespace GlobalRegistration;
//struct TransformVisitor {
//    inline void operator()(float fraction,float best_LCP,Eigen::Ref<Match4PCSBase::MatrixType> /*transformation*/) const {
//        std::cout << fraction << " " << best_LCP <<" |";
//        if (fraction >= 0){
//            printf("done: %d%c best: %f                  \r",
//               static_cast<int>(fraction * 100), '%', best_LCP);
//            fflush(stdout);
//        }
//    }
//    constexpr bool needsGlobalTransformation() const { return false; }
//};

//void CalibrationManager::calibrate_super4pcs_full_cloud(size_t indexModel, size_t indexTarget){

//    auto modelGrabber = (*m_p->grabbers)[indexModel].get();
//    auto targetGrabber = (*m_p->grabbers)[indexTarget].get();


//    // init model
//    modelGrabber->data_locker()->lock();
//    auto model  = modelGrabber->cloud_data();
//    std::vector<Point3D> set1;
//    set1.reserve(model->sizePts);
//    for(size_t ii = 0; ii < model->sizePts; ++ii){
//        auto &p = (*model->points)[ii];
//        auto &n = (*model->normals)[ii];
//        Point3D point{p.x(), p.y(), p.z()};
//        point.set_normal(Point3D::VectorType(n.x(), n.y(), n.z()));
//        set1.emplace_back(std::move(point));
//    }
//    modelGrabber->data_locker()->unlock();

//    // init target
//    targetGrabber->data_locker()->lock();
//    auto target = targetGrabber->cloud_data();
//    std::vector<Point3D> set2;
//    set2.reserve(target->sizePts);
//    for(size_t ii = 0; ii < target->sizePts; ++ii){
//        auto &p = (*target->points)[ii];
//        auto &n = (*target->normals)[ii];
//        Point3D point{p.x(), p.y(), p.z()};
//        point.set_normal(Point3D::VectorType(n.x(), n.y(), n.z()));
//        set2.emplace_back(std::move(point));
//    }
//    targetGrabber->data_locker()->unlock();

//    // Match and return the score (estimated overlap or the LCP).
//    typename Point3D::Scalar score = 0;
//    using SamplerType   = GlobalRegistration::Sampling::UniformDistSampler;
//    SamplerType sampler;

//    constexpr Utils::LogLevel loglvl = Utils::Verbose;
//    using TrVisitorType = typename std::conditional <loglvl==Utils::NoLog, Match4PCSBase::DummyTransformVisitor, TransformVisitor>::type;

//    TrVisitorType visitor;
//    Match4PCSOptions options;
//    bool overlapOk = options.configureOverlap(static_cast<float>(0.20));
//     if(! overlapOk )  {
//         std::cerr << "invalid options\n";
//         return;
//     }

//    /// The delta for the LCP (see the paper).
//    options.delta = 5.0;
//    /// Maximum normal difference.
//    options.max_normal_difference = 90.0;
//    /// Maximum translation distance. Set negative to ignore
//    options.max_translation_distance = -1;
//    /// Maximum rotation angle. Set negative to ignore
//    options.max_angle = -1;
//    /// Maximum color RGB distance between corresponding vertices. Set negative to ignore
//    options.max_color_distance = -150;
//    /// The number of points in the sample. We sample this number of points
//    /// uniformly from P and Q.
//    options.sample_size = 200;
//    /// Maximum time we allow the computation to take. This makes the algorithm
//    /// an ANY TIME algorithm that can be stopped at any time, producing the best
//    /// solution so far.
//    options.max_time_seconds = 10;
//    /// use a constant default seed by default
//    options.randomSeed = std::mt19937::default_seed;

//    Match4PCSBase::MatrixType mat (Match4PCSBase::MatrixType::Identity());

//    Utils::Logger logger(loglvl);


//    // Sampled cloud 1
//    std::string outputSampled1 = "";

//    // Sampled cloud 2
//    std::string outputSampled2 = "";

//    std::cout << "Start Super4PCS aligning\n";
//    try {
//        MatchSuper4PCS matcher(options, logger);
//        logger.Log<Utils::Verbose>( "Use Super4PCS" );
//        score = matcher.ComputeTransformation(set1, &set2, mat, sampler, visitor);

//        std::cout << "Score: " << score << "\n";
//        std::cout << "Mat:\n" << mat << "\n";
//    }
//    catch (const std::exception& e) {
//        std::cerr << "[Error]: " << e.what() << "\n";
//    }catch(...){
//        std::cerr << "[Error]2: \n";
//    }
//}


//// pcl
//#include <pcl/io/pcd_io.h>
//#include <pcl/point_types.h>
//#include <pcl/registration/icp.h>

//std::optional<Mat4<double>> CalibrationManager::calibrate_pcl_full_cloud(PclIcpParameters p, size_t indexModel, size_t indexTarget){

//    std::cout << "pcl1\n";
//    auto modelGrabber  = (*m_p->grabbers)[indexModel].get();
//    auto targetGrabber = (*m_p->grabbers)[indexTarget].get();
//    auto modelTransfo = modelGrabber->displayParams.model;

//    // init model pcl cloud
//    pcl::PointCloud<pcl::PointXYZ>::Ptr pclModelCloud (new pcl::PointCloud<pcl::PointXYZ>);
//    pclModelCloud->width    = static_cast<std::uint32_t>(modelGrabber->cloud_data()->sizePts);
//    pclModelCloud->height   = 1; // set to 1
//    pclModelCloud->is_dense = true; // false to test
//    pclModelCloud->points.resize (pclModelCloud->width * pclModelCloud->height);

//    modelGrabber->data_locker()->lock();
//    auto pts  = modelGrabber->cloud_data()->points->data();
//    for (size_t ii = 0; ii < pclModelCloud->points.size (); ++ii){
//        const auto trPt = modelTransfo.multiply_point(Pt3d{
//            static_cast<double>(pts[ii].x()),
//            static_cast<double>(pts[ii].y()),
//            static_cast<double>(pts[ii].z())}
//        );
//        pclModelCloud->points[ii].x = static_cast<float>(trPt.x());
//        pclModelCloud->points[ii].y = static_cast<float>(trPt.y());
//        pclModelCloud->points[ii].z = static_cast<float>(trPt.z());
//    }
//    modelGrabber->data_locker()->unlock();

//    std::cout << "pc2\n";
//    // init target pcl cloud
//    pcl::PointCloud<pcl::PointXYZ>::Ptr pclTargetCloud (new pcl::PointCloud<pcl::PointXYZ>);
//    pclTargetCloud->width    = static_cast<std::uint32_t>(targetGrabber->cloud_data()->sizePts);
//    pclTargetCloud->height   = 1; // set to 1
//    pclTargetCloud->is_dense = true; // false to test
//    pclTargetCloud->points.resize (pclTargetCloud->width * pclTargetCloud->height);

//    targetGrabber->data_locker()->lock();
//    pts  = targetGrabber->cloud_data()->points->data();
//    for (size_t ii = 0; ii < pclTargetCloud->points.size (); ++ii){
//        pclTargetCloud->points[ii].x = static_cast<float>(pts[ii].x());
//        pclTargetCloud->points[ii].y = static_cast<float>(pts[ii].y());
//        pclTargetCloud->points[ii].z = static_cast<float>(pts[ii].z());
//    }
//    targetGrabber->data_locker()->unlock();

//    // pcl init
//    pcl::IterativeClosestPoint<pcl::PointXYZ, pcl::PointXYZ> pclIcp;
//    if(p.useMaxIter){
//        pclIcp.setMaximumIterations (p.maxIterations);
//    }
//    if(p.useMaxCorr){
//        pclIcp.setMaxCorrespondenceDistance (p.maxCorrespondenceDistance);
//    }
//    if(p.useTransfoEpsi){
//        pclIcp.setTransformationEpsilon (p.transformationEpsilon);
//    }
//    if(p.usEuclidianEpsi){
//        pclIcp.setEuclideanFitnessEpsilon  (p.euclidianEspilon);
//    }

//    std::cout << "pcl3\n";
//    // set model/target
//    pclIcp.setInputTarget(pclModelCloud);
//    pclIcp.setInputSource(pclTargetCloud);

//    // start aligning
//    std::cout << "Start full cloud ICP:\n";
//    pcl::PointCloud<pcl::PointXYZ> Final;
//    pclIcp.align(Final);

//    std::cout << "ICP has converged: " << pclIcp.hasConverged() << " with a score of " << pclIcp.getFitnessScore() << "\n";
//    Eigen::Matrix<float, 4, 4> pclTr = pclIcp.getFinalTransformation(); // eigen matrix4
//    Mat4<double> trD(true);
//    for(int ii = 0; ii < 3; ++ii){
//        for(int jj = 0; jj < 3; ++jj){
//            trD.at(ii,jj) = static_cast<double>(pclTr(ii,jj));
//        }
//    }
//    trD = trD.transpose();
//    trD.set_translation({
//        static_cast<double>(pclTr(0,3)),
//        static_cast<double>(pclTr(1,3)),
//        static_cast<double>(pclTr(2,3))}
//    );

//    std::cout << "pcl3\n";
//    std::cout << "Final pcl transformation:\n " << pclTr << "\n";
//    std::cout << "Final transformation:\n " << trD << "\n";

//    // apply transformation to target
////    targetGrabber->update_model_matrix(trD);
//    std::cout << "pcl4\n";
//    if(pclIcp.hasConverged()){
//        return trD;
//    }
//    return {};
//}

//void CalibrationManager::calibrate_pcl_registered(PclIcpParameters p, size_t indexModel, size_t indexTarget){

//    auto modelGrabber  = (*m_p->grabbers)[indexModel].get();
//    auto targetGrabber = (*m_p->grabbers)[indexTarget].get();
//    auto modelTransfo = modelGrabber->displayParams.model;

//    // init model pcl cloud
//    pcl::PointCloud<pcl::PointXYZ>::Ptr pclModelCloud (new pcl::PointCloud<pcl::PointXYZ>);
//    pclModelCloud->width    = static_cast<std::uint32_t>(modelGrabber->calibCloud.size());
//    pclModelCloud->height   = 1; // set to 1
//    pclModelCloud->is_dense = true; // false to test
//    pclModelCloud->points.resize (pclModelCloud->width * pclModelCloud->height);

//    modelGrabber->data_locker()->lock();
//    auto pts  = modelGrabber->calibCloud.data();
//    for (size_t ii = 0; ii < pclModelCloud->points.size (); ++ii){
//        const auto trPt = modelTransfo.multiply_point(Pt3d{
//            static_cast<double>(pts[ii].x()),
//            static_cast<double>(pts[ii].y()),
//            static_cast<double>(pts[ii].z())}
//        );
//        pclModelCloud->points[ii].x = static_cast<float>(trPt.x());
//        pclModelCloud->points[ii].y = static_cast<float>(trPt.y());
//        pclModelCloud->points[ii].z = static_cast<float>(trPt.z());
//    }
//    modelGrabber->data_locker()->unlock();

//    // init target pcl cloud
//    pcl::PointCloud<pcl::PointXYZ>::Ptr pclTargetCloud (new pcl::PointCloud<pcl::PointXYZ>);
//    pclTargetCloud->width    = static_cast<std::uint32_t>(targetGrabber->calibCloud.size());
//    pclTargetCloud->height   = 1; // set to 1
//    pclTargetCloud->is_dense = true; // false to test
//    pclTargetCloud->points.resize (pclTargetCloud->width * pclTargetCloud->height);

//    targetGrabber->data_locker()->lock();
//    pts  = targetGrabber->calibCloud.data();
//    for (size_t ii = 0; ii < pclTargetCloud->points.size (); ++ii){
//        pclTargetCloud->points[ii].x = static_cast<float>(pts[ii].x());
//        pclTargetCloud->points[ii].y = static_cast<float>(pts[ii].y());
//        pclTargetCloud->points[ii].z = static_cast<float>(pts[ii].z());
//    }
//    targetGrabber->data_locker()->unlock();

//    // pcl init
//    pcl::IterativeClosestPoint<pcl::PointXYZ, pcl::PointXYZ> pclIcp;
//    if(p.useMaxIter){
//        pclIcp.setMaximumIterations (p.maxIterations);
//    }
//    if(p.useMaxCorr){
//        pclIcp.setMaxCorrespondenceDistance (p.maxCorrespondenceDistance);
//    }
//    if(p.useTransfoEpsi){
//        pclIcp.setTransformationEpsilon (p.transformationEpsilon);
//    }
//    if(p.usEuclidianEpsi){
//        pclIcp.setEuclideanFitnessEpsilon  (p.euclidianEspilon);
//    }

//    // set model/target
//    pclIcp.setInputTarget(pclModelCloud);
//    pclIcp.setInputSource(pclTargetCloud);

//    // start aligning
//    std::cout << "Start registered cloud ICP: " << pclModelCloud->points.size() << " " << pclTargetCloud->points.size() << "\n";
//    pcl::PointCloud<pcl::PointXYZ> Final;
//    pclIcp.align(Final);

//    std::cout << "ICP has converged: " << pclIcp.hasConverged() << " with a score of " << pclIcp.getFitnessScore() << "\n";
//    Eigen::Matrix<float, 4, 4> pclTr = pclIcp.getFinalTransformation(); // eigen matrix4
//    Mat4<double> trD(true);
//    for(int ii = 0; ii < 3; ++ii){
//        for(int jj = 0; jj < 3; ++jj){
//            trD.at(ii,jj) = static_cast<double>(pclTr(ii,jj));
//        }
//    }
//    trD = trD.transpose();
//    trD.set_translation({
//        static_cast<double>(pclTr(0,3)),
//        static_cast<double>(pclTr(1,3)),
//        static_cast<double>(pclTr(2,3))}
//    );

//    std::cout << "Final pcl transformation:\n " << pclTr << "\n";
//    std::cout << "Final ransformation:\n " << trD << "\n";

//    // apply transformation to target
//    targetGrabber->update_model_matrix(trD);
//}
