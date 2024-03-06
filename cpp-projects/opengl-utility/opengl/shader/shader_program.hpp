

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

// base
#include "geometry/point2.hpp"
#include "geometry/point4.hpp"
#include "geometry/matrix4.hpp"
#include "graphics/camera/camera_matrices.hpp"

// local
#include "shader.hpp"
#include "opengl/utility/gl_utility.hpp"

namespace tool::gl {

struct UniformBlockInfo{

    UniformBlockSize size;
    UniformBlockLocation location;
    UniformBlockElementInfo info;

    std::unordered_map<std::string,UniformBlockElementInfo> elements;

    auto get_offset(const std::string &elementName) const -> GLint{
        if(elements.count(elementName) == 0){
            // std::cerr << "[GL] Error " << elementName << " not found in block uniform info.\n";
            return {};
        }
        return elements.at(elementName).offset;
    }

    auto get_offsets(const std::vector<std::string> &elementsNames) const -> std::vector<GLint>{

        std::vector<GLint> offsets;
        offsets.reserve(elementsNames.size());
        for(const auto &name : elementsNames){
            if(elements.count(name) == 0){
                // std::cerr << "[GL] Error " << name << " not found in block uniform info.\n";
                return {};
            }
            offsets.emplace_back(elements.at(name).offset);
        }
        return offsets;
    }

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

    auto link() -> bool;
    auto find_uniforms_location() -> void;

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
