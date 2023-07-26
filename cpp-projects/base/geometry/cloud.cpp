

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

#include "cloud.hpp"

using namespace tool::geo;

auto ColoredCloudData::merge(const ColoredCloudData &cloud) -> void{

    if(!is_valid() || !cloud.is_valid()){
        return;
    }

    if(!empty()){

        vertices.merge(cloud.vertices);

        if(has_colors() && cloud.has_colors()){
            colors.merge(cloud.colors);
        }

        if(has_normals() && cloud.has_normals()){
            normals.merge(cloud.normals);
        }

    }else{
        *this = cloud;
    }
}

auto ColoredCloudData::fill_colors(const geo::Pt3f &color) -> void{
    colors.fill(color);
}

auto ColoredCloudData::remove_outliers(const Pt3f &target, float maxDistance) -> void {
    keep_from_ids(vertices.get_outliers_id(target, maxDistance));
}

auto ColoredCloudData::keep_from_ids(const std::vector<size_t> &ids) noexcept -> void{

    vertices.keep_from_ids(ids);

    if(has_colors()){
        colors.keep_from_ids(ids);
    }

    if(has_normals()){
        normals.keep_from_ids(ids);
    }
}



