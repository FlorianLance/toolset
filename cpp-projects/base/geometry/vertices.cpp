

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

#include "vertices.hpp"

// eigen
#include <Eigen/Dense>

using namespace tool::geo;

auto Vertices3D::sphere() const -> Sphere<float>{

    if(empty()){
        return {{},0.f};
    }

    Buffer<float> A;
    A.reserve(4*size());
    for(const auto &v : values){
        A.push_back(v.x()*2);
    }
    for(const auto &v : values){
        A.push_back(v.y()*2);
    }
    for(const auto &v : values){
        A.push_back(v.z()*2);
    }
    for(size_t id = 0; id < values.size(); ++id){
        A.push_back(1.f);
    }

    Buffer<float> F;
    F.reserve(size());
    for(const auto &v : values){
        F.push_back(geo::square_norm(v));
    }

    Eigen::MatrixXf aEigen = Eigen::MatrixXf::Map(reinterpret_cast<float*>(A.get_data()), size(), 4);
    Eigen::MatrixXf fEigen = Eigen::MatrixXf::Map(F.get_data(), F.size(), 1);
    Eigen::BDCSVD<Eigen::MatrixXf> svd(aEigen, Eigen::ComputeThinU | Eigen::ComputeThinV);

    auto C = svd.solve(fEigen);
    auto t = (C(0)*C(0)) + (C(1)*C(1)) + (C(2)*C(2)) + C(3);
    auto radius = std::sqrt(t);

    return {{C(0),C(1),C(2)},radius};
}

auto Vertices3D::sphere(float ray) const -> Sphere<float>{

    Pt3f center = mean();
    for (const auto& v : values) {
        Pt3f diff = v - center;
        auto f = ray/norm(diff);
        center += diff * f;
    }
    return {center,ray};
}

auto Vertices3D::get_outliers_id(const Pt3f &target, float maxDistance) noexcept -> std::vector<size_t>{

    if(!empty()){
        std::vector<size_t> ids;
        auto squareMaxDistance = maxDistance*maxDistance;
        for(size_t ii = 0; ii < size(); ++ii){
            if(square_norm(vec(values[ii], target)) < squareMaxDistance){
                ids.push_back(ii);
            }
        }

        return ids;
    }
    return {};
}

auto Vertices3D::get_from_plane_id(const Plane3f &plane, bool keepAbove) noexcept -> std::vector<size_t>{

    if(!empty()){
        std::vector<size_t> ids;
        for(size_t ii = 0; ii < size(); ++ii){
            auto v = plane_equation(values[ii], plane);
            if((!keepAbove && v < 0.f) || (keepAbove && v >= 0.f) ){
                ids.push_back(ii);
            }
        }
        return ids;
    }
    return {};
}



