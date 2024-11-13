
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

#pragma once

// local
#include "plane3.hpp"
#include "point4.hpp"
#include "sphere.hpp"
#include "aabb3.hpp"
#include "matrix4.hpp"
#include "utility/buffer.hpp"

namespace tool::geo {


struct Vertices3D : public Buffer<Pt3f>{

    // get
    // # geometry
    [[nodiscard]] constexpr auto min_id() const noexcept -> size_t{
        if(!empty()){
            return std::distance(values.cbegin(), std::min_element(values.cbegin(), values.cend()));
        }
        return {};
    }
    [[nodiscard]] constexpr auto max_id() const noexcept -> size_t{
        if(!empty()){
            return std::distance(values.cbegin(), std::max_element(values.cbegin(), values.cend()));
        }
        return {};
    }

    [[nodiscard]] constexpr auto min_x_id() const noexcept -> size_t{
        if(!empty()){
            auto minX = std::min_element(values.cbegin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
                return a.x() < b.x();
            });
            return std::distance(values.cbegin(), minX);
        }
        return {};
    }
    [[nodiscard]] constexpr auto max_x_id() const noexcept -> size_t{
        if(!empty()){
            auto maxX = std::max_element(values.cbegin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
                return a.x() < b.x();
            });
            return std::distance(values.cbegin(), maxX);
        }
        return {};
    }

    [[nodiscard]] constexpr auto min_y_id() const noexcept -> size_t{
        if(!empty()){
            auto minY = std::min_element(values.cbegin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
                return a.y() < b.y();
            });
            return std::distance(values.cbegin(), minY);
        }
        return {};
    }
    [[nodiscard]] constexpr auto max_y_id() const noexcept -> size_t{
        if(!empty()){
            auto maxY = std::max_element(values.cbegin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
                return a.y() < b.y();
            });
            return std::distance(values.cbegin(), maxY);
        }
        return {};
    }

    [[nodiscard]] constexpr auto min_z_id() const noexcept -> size_t{
        if(!empty()){
            auto minZ = std::min_element(values.begin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
                return a.z() < b.z();
            });
            return std::distance(values.begin(), minZ);
        }
        return {};
    }
    [[nodiscard]] constexpr auto max_z_id() const noexcept -> size_t{
        if(!empty()){
            auto maxZ = std::max_element(values.cbegin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
                return a.z() < b.z();
            });
            return std::distance(values.cbegin(), maxZ);
        }
        return {};
    }

    [[nodiscard]] constexpr auto min() const noexcept -> geo::Pt3f{
        if(!empty()){
            return *std::min_element(values.cbegin(), values.cend());
        }
        return {};
    }
    [[nodiscard]] constexpr auto max() const noexcept -> geo::Pt3f{
        if(!empty()){
            return *std::max_element(values.cbegin(), values.cend());
        }
        return {};
    }

    [[nodiscard]] constexpr auto min_x() const noexcept -> float{
        if(!empty()){
            auto minX = std::min_element(values.cbegin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
                return a.x() < b.x();
            });
            return minX->x();
        }
        return {};
    }
    [[nodiscard]] constexpr auto max_x() const noexcept -> float{
        if(!empty()){
            auto maxX = std::max_element(values.cbegin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
                return a.x() < b.x();
            });
            return maxX->x();
        }
        return {};
    }

    [[nodiscard]] constexpr auto min_y() const noexcept -> float{
        if(!empty()){
            auto minY = std::min_element(values.cbegin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
                return a.y() < b.y();
            });
            return minY->y();
        }
        return {};
    }
    [[nodiscard]] constexpr auto max_y() const noexcept -> float{
        if(!empty()){
            auto maxY = std::max_element(values.cbegin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
                return a.y() < b.y();
            });
            return maxY->y();
        }
        return {};
    }

    [[nodiscard]] constexpr auto min_z() const noexcept -> float{
        if(!empty()){
            auto minZ = std::min_element(values.begin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
                return a.z() < b.z();
            });
            return minZ->z();
        }
        return {};
    }
    [[nodiscard]] constexpr auto max_z() const noexcept -> float{
        if(!empty()){
            auto maxZ = std::max_element(values.cbegin(), values.cend(), [](const Pt3f &a, const Pt3f &b){
                return a.z() < b.z();
            });
            return maxZ->z();
        }
        return {};
    }

    [[nodiscard]] constexpr auto sum() const noexcept -> geo::Pt3f{
        if(!empty()){
            return std::accumulate(values.cbegin(), values.cend(), geo::Pt3f{0,0,0});
        }
        return {};
    }
    [[nodiscard]] constexpr auto mean() const noexcept -> geo::Pt3f{
        if(!empty()){
            return sum()/static_cast<float>(size());
        }
        return {};
    }
    [[nodiscard]] constexpr auto aabb() const noexcept -> AABB3<float>{
        if(!empty()){
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
        return {};
    }
    [[nodiscard]] auto sphere() const -> Sphere<float>;

    // id
    [[nodiscard]] auto get_outliers_id(const Pt3f &target, float maxDistance) noexcept -> std::vector<size_t>;
    [[nodiscard]] auto get_from_plane_id(const Plane3f &plane, bool keepAbove) noexcept -> std::vector<size_t>;  // TO CHECK

    // remove
    constexpr auto remove_outliers(const Pt3f &target, float maxDistance) noexcept -> void{
        if(!empty()){
            auto squareMaxDistance = maxDistance*maxDistance;
            size_t currIdToKeep = 0;
            for(auto &v : values){
                if(square_norm(vec(v, target)) < squareMaxDistance){
                    values[currIdToKeep++] = std::move(v);
                }
            }
            resize(currIdToKeep);
        }
    }
    constexpr auto remove_from_plane(const Plane3f &plane, bool keepAbove) noexcept -> void{
        // TODO: add test
        if(!empty()){
            size_t currIdToKeep = 0;
            for(auto &v : values){
                auto pe = plane_equation(v, plane);
                if((!keepAbove && pe < 0.f) || (keepAbove && pe >= 0.f) ){
                    values[currIdToKeep++] = std::move(v);
                }
            }
            resize(currIdToKeep);
        }
    }

    // modify
    constexpr auto apply_transformation(const Mat4f &transformation) noexcept -> void{
        std::transform(values.begin(), values.end(), values.begin(),
           [&](const geo::Pt3f &vertex) {
               return (to_pt4(vertex,1.f)*transformation).xyz();
           }
       );
    }
    constexpr auto sort_ascendant() noexcept -> void{
        std::sort(values.begin(), values.end(), std::less<geo::Pt3f>());
    }
    constexpr auto sort_descendant() noexcept -> void{
        std::sort(values.begin(), values.end(), std::greater<geo::Pt3f>());
    }
};
}
