

/*******************************************************************************
** Toolset-opengl-utility                                                     **
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
#include <string>
#include <memory>
#include <optional>

// glew
#include <GL/glew.h>

// glm
//#include <glm/glm.hpp>

// base
#include "utility/tuple_array.hpp"
#include "utility/unordered_map.hpp"
#include "geometry/point2.hpp"
#include "geometry/point4.hpp"
#include "geometry/matrix4.hpp"
#include "graphics/camera.hpp"

// local
#include "opengl/utility/gl_utility.hpp"

namespace tool::gl {

using namespace std::literals::string_view_literals;

class Shader{
public:

    enum class Type : std::uint8_t{
        vertex,fragment,geometry,tess_control,tess_eval,compute,
        SizeEnum
    };

    using Name = std::string_view;
    using Ext = std::string_view;
    using TShader = std::tuple<
        Type,                         GLenum,                      Name,             Ext>;
    static constexpr TupleArray<Type::SizeEnum, TShader> shaders = {{
        TShader{Type::vertex,          GL_VERTEX_SHADER,           "vertex"sv,       ".vs"sv},
        TShader{Type::fragment,        GL_FRAGMENT_SHADER,         "fragment"sv,     ".fs"sv},
        TShader{Type::geometry,        GL_GEOMETRY_SHADER,         "geometry"sv,     ".gs"sv},
        TShader{Type::tess_control,    GL_TESS_CONTROL_SHADER,     "tess_control"sv, ".tcs"sv},
        TShader{Type::tess_eval,       GL_TESS_EVALUATION_SHADER,  "tess_eval"sv,    ".tes"sv},
        TShader{Type::compute,         GL_COMPUTE_SHADER,          "compute"sv,      ".cs"sv},
    }};

    static constexpr auto get_gl_type(Type t) -> GLenum{
        return shaders.at<0,1>(t);
    }
    static constexpr auto get_name(Type t) -> std::string_view {
        return shaders.at<0,2>(t);
    }
    static constexpr auto get_ext(Type t) -> std::string_view{
        return shaders.at<0,3>(t);
    }

    static inline umap<std::string, Shader::Type> extensions = {
        {".vs",         Shader::Type::vertex},
        {".vert",       Shader::Type::vertex},
        {"_vert.glsl",  Shader::Type::vertex},
        {".vert.glsl",  Shader::Type::vertex},
        {".gs",         Shader::Type::geometry},
        {".geom",       Shader::Type::geometry},
        {".geom.glsl",  Shader::Type::geometry },
        {".tcs",        Shader::Type::tess_control},
        {".tcs.glsl",   Shader::Type::tess_control},
        {".tes",        Shader::Type::tess_eval},
        {".tes.glsl",   Shader::Type::tess_eval},
        {".fs",         Shader::Type::fragment},
        {".frag",       Shader::Type::fragment},
        {"_frag.glsl",  Shader::Type::fragment},
        {".frag.glsl",  Shader::Type::fragment},
        {".cs",         Shader::Type::compute},
        {".cs.glsl",    Shader::Type::compute}
    };
};

class ShaderProgram {

public:

    ShaderProgram() = default;
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram(ShaderProgram&& other) = default;
    ShaderProgram& operator=(ShaderProgram&& other) = default;

    auto load_from_files(const std::vector<std::string> &shadersPaths) -> bool;
    auto load_from_source_code(const std::vector<std::tuple<Shader::Type, std::string>> &shadersSourceCode) -> bool;
    auto clean() -> void;

    auto link() -> bool;
    auto find_uniforms_location() -> void;
    auto use() -> void;
    static auto unbind() -> void;

    constexpr auto id() const noexcept -> GLuint {return m_id;}
    constexpr auto is_linked() const noexcept -> bool{return m_linked;}
    constexpr auto get_handle() const noexcept -> unsigned int{ return m_id;}
    inline auto shaders_file_paths() const noexcept -> std::vector<std::string>{ return m_shadersFilePaths;}
    auto loaded_files_names_to_str() const -> std::string;

    // uniforms
    auto is_uniform_valid(const char* name) const -> bool;
    auto get_uniform_location(const char* name) const -> std::optional<UniformLocation>;
    auto check_uniform_type(const char *name, UniformType type) const -> bool;
    // # set
    // ## basic types
    auto set_uniform(const char *name, bool value) -> bool;
    auto set_uniform(const char *name, int value) -> bool;
    auto set_uniform(const char *name, float value) -> bool;
    auto set_uniform(const char *name, unsigned int value) -> bool;
    auto set_uniform(const char *name, const std::vector<float> &values) -> bool;
    auto set_uniform(const char *name, const std::vector<geo::Vec3f> &values) -> bool;
    auto set_uniform(const char *name, Sampler2D value) -> bool;
    auto set_uniform(const char *name, Sampler2DShadow value) -> bool;
    auto set_uniform(const char *name, geo::Vec2f values) -> bool;
    auto set_uniform(const char *name, geo::Vec3f values) -> bool;
    auto set_uniform(const char *name, geo::Vec4f values) -> bool;
    auto set_uniform(const char *name, geo::Mat3f values, bool transpose=false) -> bool;
    auto set_uniform(const char *name, geo::Mat4f values, bool transpose=false) -> bool;
    auto set_uniform(const char *name, float *values, bool transpose=false) -> bool;
    auto set_uniform(const char *name, const std::vector<geo::Mat4f> &values, bool transpose=false) -> bool;
    // auto set_uniform(const char *name, glm::vec3 values) -> bool;
    auto set_uniform_glm_mat4(const char *name, const float *values, bool transpose=false)  -> bool;
    // ## matrices
    auto set_model_matrix_uniform(const geo::Mat4d &model) -> bool;
    auto set_camera_matrices_uniforms(const graphics::CameraMatrices &cameraM) -> bool;

    // uniforms info
    std::vector<std::string> attribs;
    umap<std::string, std::tuple<UniformType, UniformLocation>> uniforms;
    umap<std::string, UniformBlockInfo> uniformBlocks;

    // debug
    auto debug_display() -> void;

private:

    auto detach_and_delete_shaders() -> void;
    auto init_shader(GLuint &shader, Shader::Type shaderType, const char *sourceCode) -> bool;
    auto init_shader_program(std::vector<GLuint> shaders) -> bool;

    bool m_linked = false;
    GLuint m_id = 0;
    char m_infoLog[512];

    std::vector<std::string> m_loadedShadersFileNames;
    std::vector<std::string> m_shadersFilePaths;

    //    struct FileInfo{
    //        Shader::Type type;
    //        std::string path;
    //        std::string name;
    //    };
    //    umap<Shader::Type,FileInfo> m_files;

    static constexpr const char* MVM = "ModelViewMatrix";
    static constexpr const char* NM  = "NormalMatrix";
    static constexpr const char* MVP = "MVP";
    static constexpr const char* MM  = "ModelMatrix";
};

}


