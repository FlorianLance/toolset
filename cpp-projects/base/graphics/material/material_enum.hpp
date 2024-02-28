

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

// std
#include <cstdint>

namespace tool::graphics {

enum class MaterialBlendMode : std::int8_t{
    default_blend,  // SourceColor*SourceAlpha + DestColor*(1-SourceAlpha)
    additive_blend, // SourceColor*1 + DestColor*1
    SizeEnum
};

enum class MaterialShadingModel : std::int8_t{
    flat, gouraud, phong, phong_blinn, toon, oren_nayar, minnarert, coook_torrance, no_shadding, fresnel,
    SizeEnum
};

enum class MaterialProperty : std::int8_t{
    name,twosided,shading_model,enable_wireframe,blend_func,opacity, bumpscaling, shininess, reflectivity,
    shininess_strength, refacti, color_diffuse, color_ambient, color_specular, color_emissive, color_transparent,
    color_reflective, global_background_image,
    text_blend, text_mapping, text_operation, text_uvw_source,
    text_mapping_mode_u, text_mapping_mode_v,
    text_texmap_axis, text_flags,
    SizeEnum
};


}
