

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

using namespace tool::geo;

auto Vertices3D::sphere() const -> Sphere<float>{

     // TODO: add test

    if(empty()){
        return {{},0.f};
    }
    
    Pt3f meanP = mean();

    // compute A/B
    Mat3f A;
    Vec3f B;
    auto n = static_cast<float>(size());
    for(const auto &v : values){

        Vec3f diff = v-meanP;

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



