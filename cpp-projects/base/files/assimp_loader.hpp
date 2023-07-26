

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
#include <optional>

// assimp
#include <assimp/material.h>
#include <assimp/scene.h>

// local
#include "graphics/model.hpp"
#include "utility/tuple_array.hpp"

namespace tool::files {

using Texture2D     = graphics::Texture2D;
using TextureInfo   = graphics::TextureInfo;
using GMesh         = graphics::GMesh<float>;
using Model         = graphics::Model;
using Mesh          = geo::Mesh;

using namespace std::literals::string_view_literals;

class AiLoader{

public:
    static auto load_model(std::string_view path, bool verbose = false) -> std::shared_ptr<Model>;

private:

    using TextureOperation  = graphics::TextureOperation;
    using TTexOp = std::tuple<TextureOperation,aiTextureOp>;
    static constexpr TupleArray<TextureOperation::SizeEnum, TTexOp> textureOperations ={{
        TTexOp{TextureOperation::add,         aiTextureOp::aiTextureOp_Add},
        TTexOp{TextureOperation::divide,      aiTextureOp::aiTextureOp_Divide},
        TTexOp{TextureOperation::multiply,    aiTextureOp::aiTextureOp_Multiply},
        TTexOp{TextureOperation::substract,   aiTextureOp::aiTextureOp_Subtract},
        TTexOp{TextureOperation::singed_add,  aiTextureOp::aiTextureOp_SignedAdd},
        TTexOp{TextureOperation::smooth_add,  aiTextureOp::aiTextureOp_SmoothAdd},
    }};
    static TextureOperation get_texture_operation(aiTextureOp t){
        return textureOperations.at<1,0>(t);
    }
    static aiTextureOp get_assimp_texture_operation(TextureOperation t){
        return textureOperations.at<0,1>(t);
    }

    using TextureType  = graphics::TextureType;
    using TTexType = std::tuple<TextureType,aiTextureType>;
    static constexpr TupleArray<TextureType::SizeEnum, TTexType> textureTypes ={{
        TTexType{TextureType::diffuse,      aiTextureType_DIFFUSE},
        TTexType{TextureType::ambiant,      aiTextureType_AMBIENT},
        TTexType{TextureType::specular,     aiTextureType_SPECULAR},
        TTexType{TextureType::normal,       aiTextureType_NORMALS},
        TTexType{TextureType::height,       aiTextureType_HEIGHT},
        TTexType{TextureType::light_map,    aiTextureType_LIGHTMAP},
        TTexType{TextureType::opacity,      aiTextureType_OPACITY},
        TTexType{TextureType::emissive,     aiTextureType_EMISSIVE},
        TTexType{TextureType::shininess,    aiTextureType_SHININESS},
        TTexType{TextureType::reflection,   aiTextureType_REFLECTION},
        TTexType{TextureType::displacement, aiTextureType_DISPLACEMENT},
    }};

    static TextureType get_texture_type(aiTextureType t){
        return textureTypes.at<1,0>(t);
    }
    static aiTextureType get_assimp_texture_type(TextureType t){
        return textureTypes.at<0,1>(t);
    }

    using TextureMapping    = graphics::TextureMapping;
    using TTexMap = std::tuple<TextureMapping,aiTextureMapping>;
    static constexpr TupleArray<TextureMapping::SizeEnum,TTexMap> textureMappings ={{
        TTexMap{TextureMapping::UV,        aiTextureMapping::aiTextureMapping_UV},
        TTexMap{TextureMapping::box,       aiTextureMapping::aiTextureMapping_BOX},
        TTexMap{TextureMapping::plane,     aiTextureMapping::aiTextureMapping_PLANE},
        TTexMap{TextureMapping::sphere,    aiTextureMapping::aiTextureMapping_SPHERE},
        TTexMap{TextureMapping::cylinder,  aiTextureMapping::aiTextureMapping_CYLINDER},
        TTexMap{TextureMapping::other,     aiTextureMapping::aiTextureMapping_OTHER},
    }};

    static TextureMapping get_texture_mapping(aiTextureMapping t){
        return textureMappings.at<1,0>(t);
    }
    static aiTextureMapping get_assimp_texture_mapping(TextureMapping t){
        return textureMappings.at<0,1>(t);
    }    

    using TextureMapMode    = graphics::TextureMapMode;
    using TTexMapMode = std::tuple<TextureMapMode,aiTextureMapMode>;
    static constexpr TupleArray<TextureMapMode::SizeEnum,TTexMapMode> textureMapModes ={{
        TTexMapMode{TextureMapMode::wrap,      aiTextureMapMode::aiTextureMapMode_Wrap},
        TTexMapMode{TextureMapMode::clamp,     aiTextureMapMode::aiTextureMapMode_Clamp},
        TTexMapMode{TextureMapMode::decal,     aiTextureMapMode::aiTextureMapMode_Decal},
        TTexMapMode{TextureMapMode::mirro,     aiTextureMapMode::aiTextureMapMode_Mirror},
    }};

    static TextureMapMode get_texture_map_mode(aiTextureMapMode t){
        return textureMapModes.at<1,0>(t);
    }
    static aiTextureMapMode get_assimp_texture_map_mode(TextureMapMode t){
        return textureMapModes.at<0,1>(t);
    }

    using Material = graphics::Material;
    using TMatProp = std::tuple<Material::Property,std::string_view>;
    static constexpr TupleArray<Material::Property::SizeEnum, TMatProp> materialProperties ={{
        TMatProp{Material::Property::name,                      "?mat.name"sv},
        TMatProp{Material::Property::twosided,                  "$mat.twosided"sv},
        TMatProp{Material::Property::shading_model,             "$mat.shadingm"sv},
        TMatProp{Material::Property::enable_wireframe,          "$mat.wireframe"sv},
        TMatProp{Material::Property::blend_func,                "$mat.blend"sv},
        TMatProp{Material::Property::opacity,                   "$mat.opacity"sv},
        TMatProp{Material::Property::bumpscaling,               "$mat.bumpscaling"sv},
        TMatProp{Material::Property::shininess,                 "$mat.shininess"sv},
        TMatProp{Material::Property::reflectivity,              "$mat.reflectivity"sv},
        TMatProp{Material::Property::shininess_strength,        "$mat.shinpercent"sv},
        TMatProp{Material::Property::refacti,                   "$mat.refracti"sv},
        TMatProp{Material::Property::color_diffuse,             "$mat.diffuse"sv},
        TMatProp{Material::Property::color_ambient,             "$mat.ambient"sv},
        TMatProp{Material::Property::color_specular,            "$mat.specular"sv},
        TMatProp{Material::Property::color_emissive,            "$mat.emissive"sv},
        TMatProp{Material::Property::color_transparent,         "$mat.transparent"sv},
        TMatProp{Material::Property::color_reflective,          "$mat.reflective"sv},
        TMatProp{Material::Property::global_background_image,   "$mat.global"sv},
        TMatProp{Material::Property::text_blend,                "$tex.blend"sv},
        TMatProp{Material::Property::text_mapping,              "$tex.mapping"sv},
        TMatProp{Material::Property::text_operation,            "$tex.op"sv},
        TMatProp{Material::Property::text_uvw_source,           "$tex.uvwsrc"sv},
        TMatProp{Material::Property::text_mapping_mode_u,       "$tex.mapmodeu"sv},
        TMatProp{Material::Property::text_mapping_mode_v,       "$tex.mapmodev"sv},
        TMatProp{Material::Property::text_texmap_axis,          "$tex.mapaxis"sv},
        TMatProp{Material::Property::text_flags,                "$tex.flags"sv},
        // _AI_MATKEY_UVTRANSFORM_BASE
    }};

    static std::string_view get_assimp_str_code(Material::Property p) {
        return materialProperties.at<0,1>(p);
    }

    template<typename T>
    static std::optional<T> read_property(Material::Property property, aiMaterial *assimpMaterial){
        T value;
        if(AI_SUCCESS == assimpMaterial->Get(std::string(get_assimp_str_code(property)).c_str(),0,0, value)){
            return {value};
        }
        return {T{}};
    }

    static geo::Col3f to_color(std::optional<aiColor3D> assimpColor){
        if(assimpColor.has_value()){
            return geo::Col3f{assimpColor.value().r, assimpColor.value().g, assimpColor.value().b};
        }
        return {};
    }

    static const char* to_string(std::optional<aiString> assimpStr){
        if(assimpStr.has_value()){
            return assimpStr.value().C_Str();
        }
        return {};
    }

    template<typename T>
    static std::optional<T> read_texture_property(Material::Property property, aiMaterial *assimpMaterial, TextureType type, unsigned int index){
        T value;
        if(AI_SUCCESS == assimpMaterial->Get(std::string(get_assimp_str_code(property)).c_str(),get_assimp_texture_type(type),index, value)){
            return {value};
        }
        return {T{}};
    }

    static std::optional<std::string> retrieve_texture_path(Model *model, const aiString &aiPath);
    static void read_mesh(Model *model, aiMesh *aiMesh);
    static void read_material(Model *model, aiMaterial *aiMat);    
    static void read_bones_hierarchy(graphics::BonesHierarchy *bones, aiNode *node);
};
}

