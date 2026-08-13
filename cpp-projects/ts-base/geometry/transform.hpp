
/*******************************************************************************
** Toolset-ts-base                                                            **
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
// #include "geometry/matrix4.hpp"
#include "geometry/quaternion.hpp"
// #include "utility/vector.hpp"

namespace tool::geo {

template<class acc>
class Transform {
public:

    [[nodiscard]] constexpr auto forward()  const noexcept -> Vec3<acc> {return geo::rotate(m_rotation, Vec3<acc>{0, 0, -1});};
    [[nodiscard]] constexpr auto right()    const noexcept -> Vec3<acc> {return geo::rotate(m_rotation, Vec3<acc>{1, 0,  0});};
    [[nodiscard]] constexpr auto up()       const noexcept -> Vec3<acc> {return geo::rotate(m_rotation, Vec3<acc>{0, 1,  0});};

    [[nodiscard]] constexpr auto position()     const noexcept  {return m_position;}
    [[nodiscard]] constexpr auto rotation()     const noexcept  {return m_rotation;}
    [[nodiscard]] constexpr auto euler_angles() const           {return Quat<acc>::euler_angles(m_rotation);}
    [[nodiscard]] constexpr auto scale()        const noexcept  {return m_scale;}

    constexpr auto set_position(Pt3<acc> position)   noexcept -> void{m_position = position;}
    constexpr auto set_rotation(Quat<acc> rotation)  noexcept -> void{m_rotation = rotation;}
    constexpr auto set_scale(Pt3<acc> scale)         noexcept -> void{m_scale    = scale;}

    constexpr auto translate(Vec3<acc> vec)         noexcept -> void{m_position += vec;}
    constexpr auto rotate(Quat<acc> deltaRotation)  noexcept -> void{m_rotation = deltaRotation * m_rotation;}
    constexpr auto rotate(Pt3<acc> eulerAngles)     noexcept -> void{m_rotation = Quat<acc>(eulerAngles) * m_rotation;}
    constexpr auto scale(Vec3<acc> deltaScale)      noexcept -> void{m_scale *=deltaScale;}
    constexpr auto scale_uniform(acc scale)         noexcept -> void{m_scale *= scale;}

    constexpr auto reset() noexcept -> void{
        m_rotation = {};
        m_position = {};
        m_scale = {1,1,1};
    }

    auto look_at(Pt3<acc> target, Vec3<acc> up = {0, 1, 0}) -> void{
        m_rotation = Quat<acc>::from_look_at(geo::look_at(m_position, target, up));
    }

private:
    Quat<acc> m_rotation;
    Vec3<acc> m_position;
    Vec3<acc> m_scale = {1,1,1};
};
}

// localEulerAngles	The rotation as Euler angles in degrees relative to the parent transform's rotation.
// localPosition	Position of the transform relative to the parent transform.
// localRotation	The rotation of the transform relative to the transform rotation of the parent.
// localScale	The scale of the transform relative to the GameObjects parent.

// DetachChildren	Unparents all children.
// Find	Finds a child by n and returns it.
// GetChild	Returns a transform child by index.
// GetSiblingIndex	Gets the sibling index.
// InverseTransformDirection	Transforms a direction from world space to local space. The opposite of Transform.TransformDirection.
// InverseTransformPoint	Transforms position from world space to local space.
// InverseTransformVector	Transforms a vector from world space to local space. The opposite of Transform.TransformVector.
// IsChildOf	Is this transform a child of parent?
// LookAt	Rotates the transform so the forward vector points at /target/'s current position.
// Rotate	Use Transform.Rotate to rotate GameObjects in a variety of ways. The rotation is often provided as an Euler angle and not a Quaternion.
// RotateAround	Rotates the transform about axis passing through point in world coordinates by angle degrees.
// SetAsFirstSibling	Move the transform to the start of the local transform list.
// SetAsLastSibling	Move the transform to the end of the local transform list.
// SetParent	Set the parent of the transform.
// SetPositionAndRotation	Sets the world space position and rotation of the Transform component.
// SetSiblingIndex	Sets the sibling index.
// TransformDirection	Transforms direction from local space to world space.
// TransformPoint	Transforms position from local space to world space.
// TransformVector	Transforms vector from local space to world space.
// Translate	Moves the transform in the direction and distance of translation.


