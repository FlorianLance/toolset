

/*******************************************************************************
** Toolset-ts-mesh                                                            **
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
#include <string_view>

// assimp
#include <assimp/material.h>
#include <assimp/scene.h>

// base
#include "utility/tuple_array.hpp"

// local
#include "model_mesh.hpp"

namespace tool::mesh {

using namespace std::literals::string_view_literals;

class AiLoader{

public:
    static auto load_model(std::string_view path, bool verbose = false) -> std::shared_ptr<ModelMesh>;

private:

    using TextureOperation  = img::TextureOperation;
    using TTexOp = std::tuple<
        TextureOperation,               aiTextureOp>;
    static constexpr TupleArray<TextureOperation::SizeEnum, TTexOp> textureOperations ={{
        TTexOp
        {TextureOperation::add,         aiTextureOp::aiTextureOp_Add},
        {TextureOperation::divide,      aiTextureOp::aiTextureOp_Divide},
        {TextureOperation::multiply,    aiTextureOp::aiTextureOp_Multiply},
        {TextureOperation::substract,   aiTextureOp::aiTextureOp_Subtract},
        {TextureOperation::singed_add,  aiTextureOp::aiTextureOp_SignedAdd},
        {TextureOperation::smooth_add,  aiTextureOp::aiTextureOp_SmoothAdd},
    }};
    static TextureOperation get_texture_operation(aiTextureOp t){
        return textureOperations.at<1,0>(t);
    }
    static aiTextureOp get_assimp_texture_operation(TextureOperation t){
        return textureOperations.at<0,1>(t);
    }

    using TextureType  = img::TextureType;
    using TTexType = std::tuple<
        TextureType,                aiTextureType>;
    static constexpr TupleArray<TextureType::SizeEnum, TTexType> textureTypes ={{
        TTexType
        {TextureType::diffuse,      aiTextureType_DIFFUSE},
        {TextureType::ambiant,      aiTextureType_AMBIENT},
        {TextureType::specular,     aiTextureType_SPECULAR},
        {TextureType::normal,       aiTextureType_NORMALS},
        {TextureType::height,       aiTextureType_HEIGHT},
        {TextureType::light_map,    aiTextureType_LIGHTMAP},
        {TextureType::opacity,      aiTextureType_OPACITY},
        {TextureType::emissive,     aiTextureType_EMISSIVE},
        {TextureType::shininess,    aiTextureType_SHININESS},
        {TextureType::reflection,   aiTextureType_REFLECTION},
        {TextureType::displacement, aiTextureType_DISPLACEMENT},
    }};

    static TextureType get_texture_type(aiTextureType t){
        return textureTypes.at<1,0>(t);
    }
    static aiTextureType get_assimp_texture_type(TextureType t){
        return textureTypes.at<0,1>(t);
    }

    using TextureMapping    = img::TextureMapping;
    using TTexMap = std::tuple<
        TextureMapping,             aiTextureMapping>;
    static constexpr TupleArray<TextureMapping::SizeEnum,TTexMap> textureMappings ={{
        TTexMap
        {TextureMapping::UV,        aiTextureMapping::aiTextureMapping_UV},
        {TextureMapping::box,       aiTextureMapping::aiTextureMapping_BOX},
        {TextureMapping::plane,     aiTextureMapping::aiTextureMapping_PLANE},
        {TextureMapping::sphere,    aiTextureMapping::aiTextureMapping_SPHERE},
        {TextureMapping::cylinder,  aiTextureMapping::aiTextureMapping_CYLINDER},
        {TextureMapping::other,     aiTextureMapping::aiTextureMapping_OTHER},
    }};

    static TextureMapping get_texture_mapping(aiTextureMapping t){
        return textureMappings.at<1,0>(t);
    }
    static aiTextureMapping get_assimp_texture_mapping(TextureMapping t){
        return textureMappings.at<0,1>(t);
    }    

    using TextureMapMode    = img::TextureMapMode;
    using TTexMapMode = std::tuple<
        TextureMapMode,             aiTextureMapMode>;
    static constexpr TupleArray<TextureMapMode::SizeEnum,TTexMapMode> textureMapModes ={{
        TTexMapMode
        {TextureMapMode::wrap,      aiTextureMapMode::aiTextureMapMode_Wrap},
        {TextureMapMode::clamp,     aiTextureMapMode::aiTextureMapMode_Clamp},
        {TextureMapMode::decal,     aiTextureMapMode::aiTextureMapMode_Decal},
        {TextureMapMode::mirro,     aiTextureMapMode::aiTextureMapMode_Mirror},
    }};

    static TextureMapMode get_texture_map_mode(aiTextureMapMode t){
        return textureMapModes.at<1,0>(t);
    }
    static aiTextureMapMode get_assimp_texture_map_mode(TextureMapMode t){
        return textureMapModes.at<0,1>(t);
    }

    using MatProperty = img::MaterialProperty;
    using TMatProp = std::tuple<MatProperty,std::string_view>;
    static constexpr TupleArray<MatProperty::SizeEnum, TMatProp> materialProperties ={{
        TMatProp
        {MatProperty::name,                      "?mat.name"sv},
        {MatProperty::twosided,                  "$mat.twosided"sv},
        {MatProperty::shading_model,             "$mat.shadingm"sv},
        {MatProperty::enable_wireframe,          "$mat.wireframe"sv},
        {MatProperty::blend_func,                "$mat.blend"sv},
        {MatProperty::opacity,                   "$mat.opacity"sv},
        {MatProperty::bumpscaling,               "$mat.bumpscaling"sv},
        {MatProperty::shininess,                 "$mat.shininess"sv},
        {MatProperty::reflectivity,              "$mat.reflectivity"sv},
        {MatProperty::shininess_strength,        "$mat.shinpercent"sv},
        {MatProperty::refacti,                   "$mat.refracti"sv},
        {MatProperty::color_diffuse,             "$mat.diffuse"sv},
        {MatProperty::color_ambient,             "$mat.ambient"sv},
        {MatProperty::color_specular,            "$mat.specular"sv},
        {MatProperty::color_emissive,            "$mat.emissive"sv},
        {MatProperty::color_transparent,         "$mat.transparent"sv},
        {MatProperty::color_reflective,          "$mat.reflective"sv},
        {MatProperty::global_background_image,   "$mat.global"sv},
        {MatProperty::text_blend,                "$tex.blend"sv},
        {MatProperty::text_mapping,              "$tex.mapping"sv},
        {MatProperty::text_operation,            "$tex.op"sv},
        {MatProperty::text_uvw_source,           "$tex.uvwsrc"sv},
        {MatProperty::text_mapping_mode_u,       "$tex.mapmodeu"sv},
        {MatProperty::text_mapping_mode_v,       "$tex.mapmodev"sv},
        {MatProperty::text_texmap_axis,          "$tex.mapaxis"sv},
        {MatProperty::text_flags,                "$tex.flags"sv},
        // _AI_MATKEY_UVTRANSFORM_BASE
    }};
    
    static constexpr auto get_assimp_str_code(MatProperty p) -> std::string_view{
        return materialProperties.at<0,1>(p);
    }

    template<typename T>
    static auto read_property(MatProperty property, aiMaterial *assimpMaterial) -> std::optional<T>{
        T value;
        if(AI_SUCCESS == assimpMaterial->Get(std::string(get_assimp_str_code(property)).c_str(),0,0, value)){
            return {value};
        }
        return {T{}};
    }

    template<typename T>
    static auto read_texture_property(MatProperty property, aiMaterial *assimpMaterial, TextureType type, unsigned int index) -> std::optional<T>{
        T value;
        if(AI_SUCCESS == assimpMaterial->Get(std::string(get_assimp_str_code(property)).c_str(),get_assimp_texture_type(type),index, value)){
            return {value};
        }
        return {T{}};
    }

    static auto to_color(std::optional<aiColor3D> assimpColor) -> geo::Pt3f;
    static auto to_string(std::optional<aiString> assimpStr) -> std::string;

    static auto retrieve_texture_path(ModelMesh *model, const aiString &aiPath) -> std::optional<std::string>;
    static auto read_mesh(ModelMesh *model, aiMesh *aiMesh) -> void;
    static auto read_material(ModelMesh *model, aiMaterial *aiMat) -> void;
    static auto read_bones_hierarchy(BonesHierarchy *bones, aiNode *node) -> void;
};
}

