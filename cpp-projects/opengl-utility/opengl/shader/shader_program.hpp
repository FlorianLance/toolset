

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
#include <span>

// base
#include "geometry/point2.hpp"
#include "geometry/point4.hpp"
#include "geometry/matrix4.hpp"
#include "graphics/camera/camera_matrices.hpp"
#include "utility/string_unordered_map.hpp"

// local
#include "shader_object.hpp"
#include "opengl/utility/gl_utility.hpp"


namespace tool::gl {

struct AttribInfo{
    std::string_view name;
    GlType type;
    GLint location;
};

struct UniformInfo{
    std::string_view name;
    GlType type;
    GLint location;
    GLint offset;
    GLint arrayStride;
};

struct UniformBlockElementInfo{
    std::string_view name;
    GLuint index;
    GLint offset;
};

struct UniformBlockInfo{

    std::string_view name;
    GLint size;
    GLuint location;    
    s_umap<std::string, UniformBlockElementInfo> elements;

    auto get_offset(const std::string_view elementName) const -> GLint;
    auto get_offsets(std::span<const std::string_view> elementsNames) const -> std::vector<GLint>;
};


class ShaderObject{
public:
};


class ShaderProgram {

public:

    ShaderProgram() = default;
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram(ShaderProgram&& other) = default;
    ShaderProgram& operator=(ShaderProgram&& other) = default;
    ~ShaderProgram();

    [[nodiscard]] constexpr auto id()               const noexcept -> GLuint    {return m_handle;}
    [[nodiscard]] constexpr auto is_initialized()   const noexcept -> bool      {return id() != 0;}
    [[nodiscard]] constexpr auto is_linked()        const noexcept -> bool      {return m_linked;}

    auto load_from_files(std::span<const std::string> shadersPaths) -> bool;
    auto load_from_source_code(std::span<const std::tuple<ShaderType, std::string>> shadersSourceCode) -> bool;
    auto clean() -> void;

    auto use() -> void;
    static auto unbind() -> void;

    inline auto shaders_file_paths() const noexcept -> std::span<const std::string>{ return m_shadersFilePaths;}
    auto loaded_files_names_to_str() const -> std::string;

    // uniforms
    auto is_uniform_valid(std::string_view name) const -> bool;
    auto get_uniform_info(std::string_view name) const -> std::optional<std::reference_wrapper<const UniformInfo>>;
    auto get_uniform_info(std::string_view name, GlType type) const -> std::optional<std::reference_wrapper<const UniformInfo>>;
    // # set
    // ## basic types
    auto set_uniform(std::string_view name, bool value) -> bool;
    auto set_uniform(std::string_view name, int value) -> bool;
    auto set_uniform(std::string_view name, float value) -> bool;
    auto set_uniform(std::string_view name, unsigned int value) -> bool;
    auto set_uniform(std::string_view name, std::span<float> values) -> bool;
    auto set_uniform(std::string_view name, std::span<geo::Vec3f> values) -> bool;
    auto set_uniform(std::string_view name, std::span<geo::Mat4f> values, bool transpose=false) -> bool;
    auto set_uniform(std::string_view name, const geo::Vec2f &values) -> bool;
    auto set_uniform(std::string_view name, const geo::Vec3f &values) -> bool;
    auto set_uniform(std::string_view name, const geo::Vec4f &values) -> bool;
    auto set_uniform_matrix(std::string_view name, const geo::Mat3f &values, bool transpose=false) -> bool;
    auto set_uniform_matrix(std::string_view name, const geo::Mat4f &values, bool transpose=false) -> bool;
    auto set_uniform_matrix(std::string_view name, std::span<float> values, bool transpose=false) -> bool;
    // ## samplers
    auto set_uniform_sampler_2d(std::string_view name, int value) -> bool;
    auto set_uniform_sampler_2d_shadow(std::string_view name, int value) -> bool;
    // ## matrices
    auto set_model_matrix_uniform(const geo::Mat4d &model) -> bool;
    auto set_camera_matrices_uniforms(const graphics::CameraMatrices &cameraM) -> bool;

    // introspection
    s_umap<std::string, AttribInfo> attribs;
    s_umap<std::string, UniformInfo> uniforms;
    s_umap<std::string, UniformBlockInfo> uniformBlocks;

    // debug
    auto debug_display() -> void;

private:

    auto initialize(const std::vector<GLuint> &shadersObjects) -> bool;
    auto create_shader_from_source_code(GLuint &shader, ShaderType shaderType, const std::string &sourceCode) -> bool;

    auto link_program() -> bool;

    auto retrieve_attribs_info() -> void;
    auto retrieve_uniforms_info() -> void;

    auto detach_and_delete_shaders() -> void;


    bool m_linked = false;
    GLuint m_handle = 0;
    std::array<char, 512> m_infoLog;

    // file paths
    std::vector<std::string> m_loadedShadersFileNames;
    std::vector<std::string> m_shadersFilePaths;



    //    struct FileInfo{
    //        Shader::Type type;
    //        std::string path;
    //        std::string name;
    //    };
    //    umap<Shader::Type,FileInfo> m_files;

    static constexpr std::string_view MVM = "ModelViewMatrix"sv;
    static constexpr std::string_view NM  = "NormalMatrix"sv;
    static constexpr std::string_view MVP = "MVP"sv;
    static constexpr std::string_view MM  = "ModelMatrix"sv;
};


}
