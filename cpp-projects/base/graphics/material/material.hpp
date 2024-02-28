

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
#include <vector>

// base
#include "geometry/point3.hpp"
#include "utility/unordered_map.hpp"

// local
#include "graphics/texture/texture_2d.hpp"
#include "material_enum.hpp"

namespace tool::graphics {

struct Material { // http://assimp.sourceforge.net/lib_html/materials.html

    //  TODO: add default values
    bool backfaceCulling = true;
    bool wireframe = false;

    float opacity;
    float shininess;
    float shininessStrength;
    float refraction;
    float reflectivity;

    geo::Pt3f ambiantColor;
    geo::Pt3f diffuseColor;
    geo::Pt3f specularColor;
    geo::Pt3f emissiveColor;
    geo::Pt3f transparentColor;
    geo::Pt3f reflectiveColor;

    MaterialShadingModel shadingModel;
    MaterialBlendMode blendMode;

    std::string name;

    umap<TextureType, std::vector<Texture2D*>> textures;
    umap<TextureType, std::vector<TextureInfo>> texturesInfo;
};


struct MaterialInfo {

    // [size: 48]
    geo::Vec3f Ka;      // Ambient reflectivity         [offset: 0]
    geo::Vec3f Kd;      // Diffuse reflectivity         [offset: 16]
    geo::Vec3f Ks;      // Specular reflectivity        [offset: 32]
    float Shininess;    // Specular shininess factor    [offset: 44]
};

struct RefractMaterialInfo {
    float eta;              // ration of indices of refraction
    float reflectionFactor; // percentage of reflected light
};

}

