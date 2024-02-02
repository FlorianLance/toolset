
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
#include "sphere.hpp"
#include "aabb3.hpp"
#include "matrix4.hpp"
#include "utility/buffer.hpp"

namespace tool::geo {

struct Vertices3D : public Buffer<Pt3f>{

    // get
    // # geometry
    [[nodiscard]] auto min_id() const noexcept -> size_t;
    [[nodiscard]] auto max_id() const noexcept -> size_t;
    [[nodiscard]] auto min_x_id() const noexcept -> size_t;
    [[nodiscard]] auto max_x_id() const noexcept -> size_t;
    [[nodiscard]] auto min_y_id() const noexcept -> size_t;
    [[nodiscard]] auto max_y_id() const noexcept -> size_t;
    [[nodiscard]] auto min_z_id() const noexcept -> size_t;
    [[nodiscard]] auto max_z_id() const noexcept -> size_t;
    [[nodiscard]] auto min() const noexcept -> geo::Pt3f;
    [[nodiscard]] auto max() const noexcept -> geo::Pt3f;
    [[nodiscard]] auto min_x() const noexcept -> float;
    [[nodiscard]] auto max_x() const noexcept -> float;
    [[nodiscard]] auto min_y() const noexcept -> float;
    [[nodiscard]] auto max_y() const noexcept -> float;
    [[nodiscard]] auto min_z() const noexcept -> float;
    [[nodiscard]] auto max_z() const noexcept -> float;
    [[nodiscard]] auto sum() const noexcept -> geo::Pt3f;
    [[nodiscard]] auto mean_position() const noexcept -> geo::Pt3f;
    [[nodiscard]] auto aabb() const noexcept -> AABB3<float>;
    [[nodiscard]] auto sphere() const -> Sphere<float>; // TO CHECK

    // id
    [[nodiscard]] auto get_outliers_id(const Pt3f &target, float maxDistance) noexcept -> std::vector<size_t>;
    [[nodiscard]] auto get_from_plane_id(const Plane3f &plane, bool keepAbove) noexcept -> std::vector<size_t>;  // TO CHECK
    // remove
    auto remove_outliers(const Pt3f &target, float maxDistance) noexcept -> void;
    auto remove_from_plane(const Plane3f &plane, bool keepAbove) noexcept -> void;  // TO CHECK

    // modify
    auto apply_transformation(const Mat4f &transformation) noexcept -> void;
    auto sort_ascendant() noexcept -> void;
    auto sort_descendant() noexcept -> void;    
};
}
