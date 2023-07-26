
/*******************************************************************************
** tool-test                                                                  **
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

// std
#include <random>

// local
#include "thirdparty/catch/catch.hpp"
#include "geometry/cloud.hpp"
#include "utility/logger.hpp"
#include "files/cloud_io.hpp"
#include "utility/constants.hpp"

using namespace tool;
using namespace geo;

TEST_CASE("Cloud"){

    Logger::message("start Cloud\n");


    std::mt19937 gen(0);
    std::uniform_real_distribution<> dist(-10.f, 10.f);

    size_t nbPts = 100000;
    geo::ColoredCloudData cloud;
    cloud.validVerticesCount = nbPts;
    cloud.vertices.reserve(nbPts);
    cloud.colors.reserve(nbPts);
    for(size_t ii = 0; ii < nbPts; ++ii){
        cloud.vertices.emplace_back(dist(gen),dist(gen),dist(gen));
        cloud.colors.emplace_back(1.f,0.f,0.f);
    }

    auto mean = cloud.mean_position();

    const std::string directoryPath = "D:/tool-tests";
    files::CloudIO::save_cloud(directoryPath + "/mean.obj", geo::ColoredCloudData (mean, {1.f,1.f,1.f}));
    files::CloudIO::save_cloud(directoryPath + "/original.obj", cloud);
    cloud.remove_outliers({0,0,0}, 5.f);
    cloud.fill_colors({0.f, 1.f, 0.f});
    files::CloudIO::save_cloud(directoryPath + "/without_outliers.obj", cloud);



    geo::ColoredCloudData sphereCloud;
    sphereCloud.validVerticesCount = 100000;
    sphereCloud.vertices.reserve(sphereCloud.validVerticesCount);
    sphereCloud.colors.resize(sphereCloud.validVerticesCount);

    std::uniform_real_distribution<float> uniform01(0.f, 1.f);


    Pt3f center = {1,1,0};
    float ray = 5.f;
//    float squareRay = ray*ray;
    for (size_t ii = 0; ii < sphereCloud.validVerticesCount; ++ii) {
        float theta = 2 * PI<float> * uniform01(gen);
        float phi = acos(1 - 2 * uniform01(gen));
        float x = sin(phi) * cos(theta);
        float y = sin(phi) * sin(theta);
        float z = cos(phi);
        sphereCloud.vertices.push_back(center + Pt3f{x,y,z}*ray);
    }
    sphereCloud.fill_colors({0.5f,0.5f,0.f});
    files::CloudIO::save_cloud(directoryPath + "/sphere_cloud.obj", sphereCloud);


    auto sphereCenter = sphereCloud.sphere().position;
    files::CloudIO::save_cloud(directoryPath + "/sphere_center.obj", geo::ColoredCloudData(sphereCenter, {1.f,1.f,1.f}));


    auto c1 = sphereCloud;
    auto c2 = sphereCloud;
    auto c3 = sphereCloud;
    auto c4 = sphereCloud;
    c1.cut(Plane3f{{1,0,0}, 2.f}, true);
    c2.cut(Plane3f{{1,0,0}, 2.f}, false);
    c3.cut(Plane3f{{1,0,0}, 4.f}, false);
    c4.cut(Plane3f{{1,0,0}, 4.7f}, false);

    files::CloudIO::save_cloud(directoryPath + "/cut1.obj", c1);
    files::CloudIO::save_cloud(directoryPath + "/cut2.obj", c2);
    files::CloudIO::save_cloud(directoryPath + "/cut3.obj", c3);
    files::CloudIO::save_cloud(directoryPath + "/cut4.obj", c4);

    files::CloudIO::save_cloud(directoryPath + "/cut1_center.obj", geo::ColoredCloudData(c1.sphere().position, {1.f,0.f,1.f}));
    files::CloudIO::save_cloud(directoryPath + "/cut2_center.obj", geo::ColoredCloudData(c2.sphere().position, {0.f,1.f,1.f}));
    files::CloudIO::save_cloud(directoryPath + "/cut3_center.obj", geo::ColoredCloudData(c3.sphere().position, {0.f,0.f,1.f}));
    files::CloudIO::save_cloud(directoryPath + "/cut4_center.obj", geo::ColoredCloudData(c4.sphere().position, {0.f,0.f,1.f}));

    SECTION("cloud init"){


        //        tool::Logger::message("");
    }
}
