
/*******************************************************************************
** Toolset-ts-graphics                                                        **
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

#include "animation.hpp"

using namespace tool::graphics;
using namespace tool::geo;

auto AnimationKeys::find_position(float time) const noexcept -> std::size_t{
    for(size_t ii = 0; ii < positionTimes.size()-1; ++ii){
        if(time < static_cast<float>(positionTimes[ii+1])){
            return ii;
        }
    }
    return 0;
}

auto AnimationKeys::find_rotation(float time) const noexcept -> std::size_t{
    for(size_t ii = 0; ii < rotationTimes.size()-1; ++ii){
        if(time < static_cast<float>(rotationTimes[ii+1])){
            return ii;
        }
    }
    return 0;
}

auto AnimationKeys::find_scaling(float time) const noexcept -> std::size_t{
    for(size_t ii = 0; ii < scalingTimes.size()-1; ++ii){
        if(time < static_cast<float>(scalingTimes[ii+1])){
            return ii;
        }
    }
    return 0;
}

auto AnimationKeys::calculate_interpolated_scaling(float time) const noexcept -> Pt3f{

    if(scalingKeys.empty()){
        return {};
    }

    if(scalingKeys.size() == 1){
        return scalingKeys[0];
    }

    size_t scalingId =find_scaling(time);
    size_t nextScalingIndex = scalingId +1;

    float deltaTime = static_cast<float>(scalingTimes[nextScalingIndex] - scalingTimes[scalingId]);
    float factor = (time - static_cast<float>(scalingTimes[scalingId]))/deltaTime;

    Pt3f start = scalingKeys[scalingId];
    Pt3f end   = scalingKeys[nextScalingIndex];
    Pt3f delta = end - start;

    return start + delta * factor;
}

auto AnimationKeys::calculate_interpolated_position(float time) const noexcept -> Pt3f{

    if(positionKeys.empty()){
        return {};
    }

    if(positionKeys.size() == 1){
        return positionKeys[0];
    }

    size_t positionId = find_position(time);
    size_t nextPositionIndex = positionId +1;

    float deltaTime = static_cast<float>(positionTimes[nextPositionIndex] - positionTimes[positionId]);
    float factor = (time - static_cast<float>(positionTimes[positionId]))/deltaTime;

    Pt3f start = positionKeys[positionId];
    Pt3f end   = positionKeys[nextPositionIndex];
    Pt3f delta = end - start;

    return start + delta * factor;
}

auto AnimationKeys::calculate_interpolated_rotation(float time) -> Quatf{

    if(rotationKeys.empty()){
        return {};
    }

    if(rotationKeys.size() == 1){
        return rotationKeys[0];
    }

    size_t rotationId = find_rotation(time);
    size_t nextRotationIndex = rotationId +1;

    float deltaTime = static_cast<float>(rotationTimes[nextRotationIndex] - rotationTimes[rotationId]);
    float factor = (time - static_cast<float>(rotationTimes[rotationId]))/deltaTime;

    Quatf start = rotationKeys[rotationId];
    Quatf end   = rotationKeys[nextRotationIndex];
    return normalize(slerp(start, end, factor));
}
