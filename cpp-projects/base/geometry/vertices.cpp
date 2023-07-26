

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

//// std
//#include <algorithm>

using namespace tool::geo;

auto Vertices3D::min_id() const noexcept -> size_t{
    if(empty()){
        return {};
    }
    return std::distance(values.cbegin(), std::min_element(values.cbegin(), values.cend()));
}

auto Vertices3D::min() const noexcept -> Pt3f{
    if(empty()){
        return {};
    }
    return *std::min_element(values.cbegin(), values.cend());
}

auto Vertices3D::max_id() const noexcept -> size_t{
    if(empty()){
        return {};
    }
    return std::distance(values.cbegin(), std::max_element(values.cbegin(), values.cend()));
}

auto Vertices3D::max() const noexcept -> Pt3f{
    if(empty()){
        return {};
    }
    return *std::max_element(values.cbegin(), values.cend());
}

auto Vertices3D::min_x_id() const noexcept -> size_t{
    if(empty()){
        return {};
    }
    auto minX = std::min_element(values.cbegin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
        return a.x() < b.x();
    });
    return std::distance(values.cbegin(), minX);
}

auto Vertices3D::min_x() const noexcept -> float{
    if(empty()){
        return {};
    }
    auto minX = std::min_element(values.cbegin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
        return a.x() < b.x();
    });
    return minX->x();
}

auto Vertices3D::max_x_id() const noexcept -> size_t{
    if(empty()){
        return {};
    }
    auto maxX = std::max_element(values.cbegin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
        return a.x() < b.x();
    });
    return std::distance(values.cbegin(), maxX);
}

auto Vertices3D::max_x() const noexcept -> float{
    if(empty()){
        return {};
    }
    auto maxX = std::max_element(values.cbegin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
        return a.x() < b.x();
    });
    return maxX->x();
}

auto Vertices3D::min_y_id() const noexcept -> size_t{
    if(empty()){
        return {};
    }
    auto minY = std::min_element(values.cbegin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
        return a.y() < b.y();
    });
    return std::distance(values.cbegin(), minY);
}

auto Vertices3D::min_y() const noexcept -> float{
    if(empty()){
        return {};
    }
    auto minY = std::min_element(values.cbegin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
        return a.y() < b.y();
    });
    return minY->y();
}

auto Vertices3D::max_y_id() const noexcept -> size_t{
    if(empty()){
        return {};
    }
    auto maxY = std::max_element(values.cbegin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
        return a.y() < b.y();
    });
    return std::distance(values.cbegin(), maxY);
}

auto Vertices3D::max_y() const noexcept -> float{
    if(empty()){
        return {};
    }
    auto maxY = std::max_element(values.cbegin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
        return a.y() < b.y();
    });
    return maxY->y();
}

auto Vertices3D::min_z_id() const noexcept -> size_t{
    if(empty()){
        return {};
    }
    auto minZ = std::min_element(values.begin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
        return a.z() < b.z();
    });
    return std::distance(values.begin(), minZ);
}

auto Vertices3D::min_z() const noexcept -> float{
    if(empty()){
        return {};
    }
    auto minZ = std::min_element(values.begin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
        return a.z() < b.z();
    });
    return minZ->z();
}

auto Vertices3D::max_z_id() const noexcept -> size_t{
    if(empty()){
        return {};
    }
    auto maxZ = std::max_element(values.cbegin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
        return a.z() < b.z();
    });
    return std::distance(values.cbegin(), maxZ);
}

auto Vertices3D::max_z() const noexcept -> float{
    if(empty()){
        return {};
    }
    auto maxZ = std::max_element(values.cbegin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
        return a.z() < b.z();
    });
    return maxZ->z();
}

auto Vertices3D::sum() const noexcept -> Pt3f {
    if(empty()){
        return {};
    }
    return std::accumulate(values.cbegin(), values.cend(), geo::Pt3f{0,0,0});
}

auto Vertices3D::mean_position() const noexcept -> Pt3f{
    if(empty()){
        return {};
    }
    return sum()/static_cast<float>(size());
}

auto Vertices3D::aabb() const noexcept -> AABB3<float>{

    if(empty()){
        return {};
    }

    auto [minX, maxX] = std::minmax_element(values.cbegin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
        return a.x() < b.x();
    });
    auto [minY, maxY] = std::minmax_element(values.cbegin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
        return a.y() < b.y();
    });
    auto [minZ, maxZ] = std::minmax_element(values.cbegin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
        return a.z() < b.z();
    });

    return {
        {minX->x(),minY->y(),minZ->z()},
        {maxX->x(),maxY->y(),maxZ->z()}
    };
}

auto Vertices3D::sphere() const -> Sphere<float>{

    if(empty()){
        return {{},0.f};
    }

    Pt3f mean = mean_position();

    // compute A/B
    Mat3f A;
    Vec3f B;
    auto n = static_cast<float>(size());
    for(const auto &v : values){

        Vec3f diff = v-mean;

        A.array[0] += (v.x() * diff.x());
        A.array[1] += (v.x() * diff.y());
        A.array[2] += (v.x() * diff.z());

        A.array[3] += (v.y() * diff.x());
        A.array[4] += (v.y() * diff.y());
        A.array[5] += (v.y() * diff.z());

        A.array[6] += (v.z() * diff.x());
        A.array[7] += (v.z() * diff.y());
        A.array[8] += (v.z() * diff.z());
        A /= n;

        auto d = dot(v, v);
        B += (diff*d)/n;
    }
    A *= 2.;

    Mat3f trA = transpose(A);
    Mat3f bb  = inverse((trA*A)) * trA;

    Pt3f center =  transpose(bb * transpose(B));
    float r = 0.f;
    for(const auto &v : values){
        r += square_norm(v - center);
    }
    r = sqrt(r/n);

    return {center,r};
}

auto Vertices3D::apply_transformation(const Mat4f &transformation) noexcept -> void{

    std::transform(values.begin(), values.end(), values.begin(),
        [&](geo::Pt3f vertex) {
            return (to_pt4(vertex,1.f)*transformation).xyz();
        }
    );
}

auto Vertices3D::sort_ascendant() noexcept -> void{
    std::sort(values.begin(), values.end(), std::less<geo::Pt3f>());
}

auto Vertices3D::sort_descendant() noexcept -> void{
    std::sort(values.begin(), values.end(), std::greater<geo::Pt3f>());
}

auto Vertices3D::get_outliers_id(const Pt3f &target, float maxDistance) noexcept -> std::vector<size_t>{

    if(empty()){
        return {};
    }

    std::vector<size_t> ids;
    auto squareMaxDistance = maxDistance*maxDistance;
    for(size_t ii = 0; ii < size(); ++ii){
        if(square_norm(vec(values[ii], target)) < squareMaxDistance){
            ids.push_back(ii);
        }
    }

    return ids;
}

auto Vertices3D::get_from_plane_id(const Plane3f &plane, bool keepAbove) noexcept -> std::vector<size_t>{

    if(empty()){
        return {};
    }

    std::vector<size_t> ids;
    for(size_t ii = 0; ii < size(); ++ii){
        auto v = plane_equation(values[ii], plane);
        if((!keepAbove && v < 0.f) || (keepAbove && v >= 0.f) ){
            ids.push_back(ii);
        }
    }
    return ids;
}

auto Vertices3D::remove_outliers(const Pt3f &target, float maxDistance) noexcept -> void {

    if(empty()){
        return;
    }

    auto squareMaxDistance = maxDistance*maxDistance;
    size_t currIdToKeep = 0;
    for(auto &v : values){
        if(square_norm(vec(v, target)) < squareMaxDistance){
            values[currIdToKeep++] = std::move(v);
        }
    }
    resize(currIdToKeep);
}

auto Vertices3D::remove_from_plane(const Plane3f &plane, bool keepAbove) noexcept -> void{

    if(empty()){
        return;
    }

    size_t currIdToKeep = 0;
    for(auto &v : values){
        auto pe = plane_equation(v, plane);
        if((!keepAbove && pe < 0.f) || (keepAbove && pe >= 0.f) ){
            values[currIdToKeep++] = std::move(v);
        }
    }
    resize(currIdToKeep);
}




