

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

#include "shader_program.hpp"

// std
#include <filesystem>
#include <fstream>

// base
#include "utility/logger.hpp"
#include "utility/string.hpp"

// local
#include "opengl/gl_functions.hpp"

using namespace tool;
using namespace tool::gl;
using namespace tool::geo;


auto UniformBlockInfo::get_offset(const std::string_view elementName) const -> GLint{
    if(elements.count(elementName) != 0){
        return elements.at(elementName).offset;
    }
    Logger::error(std::format("[UniformBlockInfo::get_offset] Element [{}] not found in block uniform info [{}].\n"sv, elementName, name));
    return {};
}

auto UniformBlockInfo::get_offsets(std::span<const std::string_view> elementsNames) const -> std::vector<GLint>{

    std::vector<GLint> offsets;
    offsets.reserve(elementsNames.size());

    for(const auto &elementName : elementsNames){
        if(elements.count(elementName) != 0){
            offsets.push_back(elements.at(elementName).offset);
        }else{
            Logger::error(std::format("[UniformBlockInfo::get_offsets] Element [{}] not found in block uniform info [{}].\n"sv, elementName, name));
            return {};
        }
    }
    return offsets;
}

ShaderProgram::~ShaderProgram(){
    if(is_initialized()){
        Logger::error(std::format("[ShaderProgram::~ShaderProgram] ShaderProgram has not been cleaned (id:{}).\n", m_handle));
    }
}

auto ShaderProgram::use() -> void{

    if(m_handle <= 0){
        Logger::error("[ShaderProgram::use] Shader program has not been created.\n"sv);
        return;
    }
    if(!m_linked){
        Logger::error("[ShaderProgram::use] Shader program has not been linked.\n"sv);
        return;
    }
    GL::use_program(m_handle);
}

auto ShaderProgram::unbind() -> void{
    GL::use_program(0);
}

auto ShaderProgram::is_uniform_valid(std::string_view name) const -> bool{
    return uniforms.count(name) != 0;
}

auto ShaderProgram::get_uniform_info(std::string_view name) const -> std::optional<std::reference_wrapper<const UniformInfo>>{
    if(is_uniform_valid(name)){
        return uniforms.at(name);
    }
    return {};
}

auto ShaderProgram::get_uniform_info(std::string_view name, GlType type) const -> std::optional<std::reference_wrapper<const UniformInfo>>{
    if(is_uniform_valid(name)){
        const auto &info = uniforms.at(name);
        if(info.type == type){
            return uniforms.at(name);
        }
        Logger::error(std::format("[ShaderProgram::get_uniform_info] Invalid type for uniform [{}], type prodived: [{}], expected: [{}]. \n"sv, name, get_name(type), get_name(info.type)));
        return {};
    }
    // Logger::error(std::format("[ShaderProgram::get_uniform_info] Uniform with name [{}] doesn't exist in shader files [{}].\n"sv, name, String::join(m_shadersFilePaths)));
    return {};
}

auto ShaderProgram::set_uniform(std::string_view name, bool value) -> bool{
    if(const auto &info = get_uniform_info(name, GlType::bool_t); info.has_value()){
        GL::program_uniform_1i(m_handle, info->get().location, value);
        return true;
    }
    return false;
}

auto ShaderProgram::set_uniform(std::string_view name, int value) -> bool{
    if(const auto &info = get_uniform_info(name, GlType::int_t); info.has_value()){
        GL::program_uniform_1i(m_handle, info->get().location, value);
        return true;
    }
    return false;
}

auto ShaderProgram::set_uniform_sampler_2d(std::string_view name, int value) -> bool{
    if(const auto &info = get_uniform_info(name, GlType::sampler2d_t); info.has_value()){
        GL::program_uniform_1i(m_handle, info->get().location, value);
        return true;
    }
    return false;
}

auto ShaderProgram::set_uniform_sampler_2d_shadow(std::string_view name, int value) -> bool{
    if(const auto &info = get_uniform_info(name, GlType::sampler2dShadow_t); info.has_value()){
        GL::program_uniform_1i(m_handle, info->get().location, value);
        return true;
    }
    return false;
}

auto ShaderProgram::set_uniform(std::string_view name, float value) -> bool{
    if(const auto &info = get_uniform_info(name, GlType::float_t); info.has_value()){
        GL::program_uniform_1f(m_handle, info->get().location, value);
        return true;
    }
    return false;
}

auto ShaderProgram::set_uniform(std::string_view name, unsigned int value) -> bool{
    if(const auto &info = get_uniform_info(name, GlType::uint_t); info.has_value()){
        GL::program_uniform_1ui(m_handle, info->get().location, value);
        return true;
    }
    return false;
}

auto ShaderProgram::set_uniform(std::string_view name, std::span<float> values) -> bool{
    if(const auto &info = get_uniform_info(name, GlType::float_t); info.has_value()){
        GL::program_uniform_1fv(m_handle, info->get().location, static_cast<GLsizei>(values.size()), values.data());
        return true;
    }
    return false;
}

auto ShaderProgram::set_uniform(std::string_view name, std::span<geo::Vec3f> values) -> bool{
    if(const auto &info = get_uniform_info(name, GlType::fvec3_t); info.has_value()){
        GL::program_uniform_3fv(m_handle, info->get().location, static_cast<GLsizei>(values.size()), values.data()->array.data());
        return true;
    }
    return false;
}

auto ShaderProgram::set_uniform(std::string_view name, const Vec2f &values) -> bool{
    if(const auto &info = get_uniform_info(name, GlType::fvec2_t); info.has_value()){
        GL::program_uniform_2f(m_handle, info->get().location, values.x(), values.y());
        return true;
    }
    return false;
}

auto ShaderProgram::set_uniform(std::string_view name, const Vec3f &values) -> bool{
    if(const auto &info = get_uniform_info(name, GlType::fvec3_t); info.has_value()){
        GL::program_uniform_3f(m_handle, info->get().location, values.x(), values.y(), values.z());
        return true;
    }
    return false;
}

auto ShaderProgram::set_uniform(std::string_view name, const Vec4f &values) -> bool{
    if(const auto &info = get_uniform_info(name, GlType::fvec4_t); info.has_value()){
        GL::program_uniform_4f(m_handle, info->get().location, values.x(), values.y(), values.z(), values.w());
        return true;
    }
    return false;
}

auto ShaderProgram::set_uniform_matrix(std::string_view name, const Mat3f &values, bool transpose) -> bool{
    if(const auto &info = get_uniform_info(name, GlType::fmat3x3_t); info.has_value()){
        GL::program_uniform_matrix_3fv(m_handle, info->get().location, 1, transpose ? GL_TRUE : GL_FALSE, values.array.data());
        return true;
    }
    return false;
}

auto ShaderProgram::set_uniform_matrix(std::string_view name, const Mat4f &values, bool transpose) -> bool{
    if(const auto &info = get_uniform_info(name, GlType::fmat4x4_t); info.has_value()){
        GL::program_uniform_matrix_4fv(m_handle, info->get().location, 1, transpose ? GL_TRUE : GL_FALSE, values.array.data());
        return true;
    }
    return false;
}

auto ShaderProgram::set_uniform_matrix(std::string_view name, std::span<float> values, bool transpose) -> bool{

    GlType type;
    if(values.size() == 4){
        type = GlType::fmat2x2_t;
    }else if(values.size() == 9){
        type = GlType::fmat3x3_t;
    }else if(values.size() == 16){
        type = GlType::fmat4x4_t;
    }else{
        // ...
        return false;
    }

    if(const auto &info = get_uniform_info(name, type); info.has_value()){
        GL::program_uniform_matrix_4fv(m_handle, info->get().location, 1, transpose ? GL_TRUE : GL_FALSE, values.data());
        return true;
    }
    return false;
}

auto ShaderProgram::set_uniform(std::string_view name, std::span<geo::Mat4f> values, bool transpose) -> bool{
    if(const auto &info = get_uniform_info(name, GlType::fmat4x4_t); info.has_value()){
        GL::program_uniform_matrix_4fv(m_handle, info->get().location, static_cast<GLsizei>(values.size()), transpose ? GL_TRUE : GL_FALSE, values.data()->array.data());
        return true;
    }
    return false;
}

auto ShaderProgram::set_model_matrix_uniform(const Mat4d &model) -> bool{
    return set_uniform_matrix(MM, model.conv<float>());
}

auto ShaderProgram::set_camera_matrices_uniforms(const graphics::CameraMatrices &camM) -> bool{
    bool validUniforms = true;
    validUniforms &= set_uniform_matrix(MVM, camM.mv.conv<float>());
    validUniforms &= set_uniform_matrix(NM,  camM.normal.conv<float>());
    validUniforms &= set_uniform_matrix(MVP, camM.mvp.conv<float>());
    validUniforms &= set_model_matrix_uniform(camM.m);
    return validUniforms;
}

auto ShaderProgram::debug_display() -> void{

    Logger::message("[shader files]:\n");
    for(const auto& file : m_loadedShadersFileNames){
        Logger::message(std::format("  -{}\n", file));
    }
    Logger::message("[attribs]:\n");
    for(const auto& attrib : attribs){
        // Logger::message(std::format("  -{}\n", attrib));
    }
    Logger::message("[uniforms]:\n");
    for(const auto& uniform : uniforms){
        Logger::message(std::format("  -[{}][{}]\n", uniform.first, get_name(uniform.second.type)));
    }
    Logger::message("[uniforms blocks]:\n");
    for(const auto& uniformBlock : uniformBlocks){
        Logger::message(std::format("  -[{}][size: {}][loc: {}]\n", uniformBlock.first, uniformBlock.second.size, uniformBlock.second.location));
        for(const auto &element : uniformBlock.second.elements){
            Logger::message(std::format("    -[name: {}][index: {}][offset: {}]\n", element.first, element.second.index, element.second.offset));
        }
    }
}

auto ShaderProgram::loaded_files_names_to_str() const -> std::string{

    if(m_loadedShadersFileNames.size() == 0){
        return "[]";
    }

    std::string filesStr = "[";

    for (const auto &file : m_loadedShadersFileNames){
        filesStr += file + ";";
    }
    filesStr[filesStr.size()-1] = ']';
    return filesStr;
}

auto ShaderProgram::link_program() -> bool{

    if(m_linked){
        Logger::error("[ShaderProgram::link] Program is already linked.\n"sv);
        return true;
    }

    if(m_handle <= 0){
        Logger::error("[ShaderProgram::link] Program has not been compiled.\n"sv);
        return false;
    }

    GL::link_program(m_handle);

    // Retrieve link status
    GLint status = 0;
    GL::get_program_iv(m_handle, GL_LINK_STATUS, &status);

    if(status == GL_FALSE) {
        GL::get_program_info_log(m_handle, static_cast<GLsizei>(m_infoLog.size()), nullptr, m_infoLog.data());

        // GLint logSize = 0;
        // GL::get_program_iv(m_handle, GL_INFO_LOG_LENGTH, &logSize);

        Logger::error(std::format("[ShaderProgram::link] Program link failed: {}\n"sv, m_infoLog.data()));
        return m_linked = false;
    }

    return m_linked = true;
}

auto ShaderProgram::retrieve_attribs_info() -> void{

    // Clean
    attribs.clear();

    // Get active vertex input attributes
    GLint numAttribs = 0;
    GL::get_program_interface_iv(m_handle, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttribs);

    GLenum propertiesAttribs[] = {
        GL_NAME_LENGTH, // a single integer identifying the length of the name string associated with an active variable, interface block, or subroutine is written to params. The name length includes a terminating null character.
        GL_TYPE,        // a single integer identifying the type of an active variable is written to params. The integer returned is one of the values found in table 2.16.
        GL_LOCATION     // a single integer identifying the assigned location for an active uniform, input, output, or subroutine uniform variable is written to params.
        // For input, output, or uniform variables with locations specified by a layout qualifier, the specified location is used.
        // For vertex shader input or fragment shader output variables without a layout qualifier, the location assigned when a program is linked is written to params.
        // For all other input and output variables, the value -1 is written to params. For uniforms in uniform blocks, the value -1 is written to params.
    };

    for(GLuint ii = 0; ii < static_cast<GLuint>(numAttribs); ++ii){

        // get properties
        GLint attribPropertiesValues[3];
        GL::get_program_resource_iv(m_handle, GL_PROGRAM_INPUT, ii, 3, propertiesAttribs, 3, nullptr, attribPropertiesValues);

        const auto &nameLength = attribPropertiesValues[0];
        const auto &type       = attribPropertiesValues[1];
        const auto &location   = attribPropertiesValues[2];

        // retrieve attrib name
        std::vector<GLchar> attribNameArray(static_cast<size_t>(nameLength));
        GL::get_program_resource_name(m_handle, GL_PROGRAM_INPUT, ii, static_cast<GLint>(attribNameArray.size()), nullptr, attribNameArray.data());

        std::string attribName(std::begin(attribNameArray), std::end(attribNameArray) - 1);
        attribs[attribName] = AttribInfo{attribName, get_type(static_cast<GLenum>(type)), location};
    }
}

auto ShaderProgram::retrieve_uniforms_info() -> void{

    // Clean
    uniforms.clear();
    uniformBlocks.clear();

    // Get active uniform variables
    GLint numUniforms = 0;
    GL::get_program_interface_iv(m_handle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);

    GLenum propertiesUniforms[] = {
        GL_NAME_LENGTH,
        GL_TYPE,
        GL_LOCATION,
        GL_BLOCK_INDEX, // a single integer identifying the index of the active interface block containing an active variable is written to params.
        // If the variable is not the member of an interface block, the value -1 is written to params.
        GL_OFFSET,      // a single integer identifying the offset of an active variable is written to params.
        // For variables in the GL_UNIFORM and GL_BUFFER_VARIABLE interfaces that are backed by a buffer object,
        // the value written is the offset of that variable relative to the base of the buffer range holding its value.
        // For variables in the GL_TRANSFORM_FEEDBACK_VARYING interface, the value written is the offset in the transform feedback buffer storage
        // assigned to each vertex captured in transform feedback mode where the value of the variable will be stored.
        // Such offsets are specified via the xfb_offset layout qualifier or assigned according to the variables position
        // in the list of strings passed to glTransformFeedbackVaryings. Offsets are expressed in basic machine units.
        // For all variables not recorded in transform feedback mode, including the special names "gl_NextBuffer", "gl_SkipComponents1", "gl_SkipComponents2", "gl_SkipComponents3", and "gl_SkipComponents4", -1 is written to params.
        GL_ARRAY_STRIDE // a single integer identifying the stride between array elements in an active variable is written to params.
        // For active variables declared as an array of basic types, the value written is the difference, in basic machine units,
        // between the offsets of consecutive elements in an array. For active variables not declared as an array of basic types, zero is written to params.
        // For active variables not backed by a buffer object, -1 is written to params, regardless of the variable type.
    };

    for(GLuint ii = 0; ii < static_cast<GLuint>(numUniforms); ++ii){

        // get properties
        GLint uniformPropertiesValues[6];
        GL::get_program_resource_iv(m_handle, GL_UNIFORM, ii, 4, propertiesUniforms, 4, nullptr, uniformPropertiesValues);

        const auto &nameLength   = uniformPropertiesValues[0];
        const auto &type         = uniformPropertiesValues[1];
        const auto &location     = uniformPropertiesValues[2];
        const auto &blockIndex   = uniformPropertiesValues[3];
        const auto &offset       = uniformPropertiesValues[4];
        const auto &arrayStride  = uniformPropertiesValues[5];

        // retrieve uniform name
        std::vector<GLchar> uniformNameArray(static_cast<size_t>(nameLength));
        GL::get_program_resource_name(m_handle, GL_UNIFORM, ii, static_cast<GLint>(uniformNameArray.size()), nullptr, uniformNameArray.data());

        std::string uniformName(std::begin(uniformNameArray), std::end(uniformNameArray) - 1);

        if(blockIndex == -1){ // uniform
            uniforms[uniformName] = UniformInfo{uniformName, get_type(static_cast<GLenum>(type)), location, offset, arrayStride};
        }else{ // uniform block

            auto split = String::split(uniformName, '.');
            if(split.size() == 2){

                const auto fullBlockName = split[0];
                if(uniformBlocks.count(fullBlockName) == 0){

                    GLint blockSize = 0;
                    GL::get_active_uniform_block_iv(m_handle, static_cast<GLuint>(blockIndex), GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

                    uniformBlocks[fullBlockName] = UniformBlockInfo{fullBlockName, blockSize, static_cast<GLuint>(blockIndex), {}};
                }

                GLuint indices = 0;
                const GLchar *names[] = {uniformName.c_str()};
                glGetUniformIndices(m_handle, 1, names, &indices);

                GLint offset = 0;
                glGetActiveUniformsiv(m_handle, 1, &indices, GL_UNIFORM_OFFSET, &offset);
                uniformBlocks[fullBlockName].elements[uniformName] = UniformBlockElementInfo{uniformName, indices, offset};
            }
        }
    }
}

auto ShaderProgram::clean() -> void{

    if (m_handle == 0){
        return;
    }

    detach_and_delete_shaders();
    GL::delete_program(m_handle);
    m_handle = 0;

    // clean introspection
    attribs.clear();
    uniforms.clear();
    uniformBlocks.clear();
}

auto ShaderProgram::load_from_files(std::span<const std::string> shadersPaths) -> bool{

    for(const auto& sh : shadersPaths){
        Logger::message(std::format("s:{}\n", sh));
    }

    m_shadersFilePaths.resize(shadersPaths.size());
    std::copy(shadersPaths.begin(), shadersPaths.end(), m_shadersFilePaths.begin());

    namespace fs = std::filesystem;

    std::vector<std::tuple<ShaderType, std::string>> shadersCode;

    m_loadedShadersFileNames.clear();

    // open files
    for(const auto &shaderPath : shadersPaths){

        const auto path = fs::path(shaderPath);

        // check if exists
        if(!fs::exists(path)){
            Logger::error(std::format("[ShaderProgram::load_from_files] Shader path [{}] [{}] doesn't exists.\n"sv, path.string(), shaderPath));
            return false;
        }

        // check extension
        auto ext = path.filename().string();
        if(auto id = ext.find("."); id != std::string::npos){
            ext.erase(ext.begin(), ext.begin() + id);
        }else{
            Logger::error("[ShaderProgram::load_from_files] Invalid shader file name.\n"sv);
            return false;
        }

        if(shadersTypeExtensions.count(ext) == 0){
            Logger::error(std::format("[ShaderProgram::load_from_files] Extension {} of shader file {} is not valid.\n"sv, ext, path.string()));
            return false;
        }

        // open file
        std::ifstream shaderFile(path, std::ios::in);
        if(!shaderFile.is_open()){
            Logger::error(std::format("[ShaderProgram::load_from_files] Cannot open shader file {} \n"sv, path.string()));
            return false;
        }

        m_loadedShadersFileNames.emplace_back(path.filename().string());

        // read file
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();

        // close file
        shaderFile.close();

        // add to shader array
        shadersCode.emplace_back(shadersTypeExtensions[ext], shaderStream.str());
        // Shader::get_name(Shader::extensions[ext])
    }

    return load_from_source_code(shadersCode);
}

auto ShaderProgram::load_from_source_code(std::span<const std::tuple<ShaderType, std::string>> shadersSourceCode) -> bool{

    std::vector<GLuint> shaders;
    for(const auto &shaderSourceCode : shadersSourceCode){
        GLuint shader = 0;
        if(!create_shader_from_source_code(shader, std::get<0>(shaderSourceCode), std::get<1>(shaderSourceCode))){
            Logger::error(std::format(
                "[ShaderProgram::load_from_source_code] Cannot load shader source code : {} of type: {}.\n"sv,
                std::get<1>(shaderSourceCode),
                get_name(std::get<0>(shaderSourceCode)))
            );
            return false;
        }
        shaders.emplace_back(shader);
    }

    return initialize(shaders);
}

auto ShaderProgram::create_shader_from_source_code(GLuint &shader, ShaderType shaderType, const std::string &sourceCode) -> bool{

    // create shader
    shader = GL::create_shader(get_gl_type(shaderType));

    // set source code
    auto cstr = sourceCode.c_str();
    GL::shader_source(shader, 1, &cstr, nullptr);

    // compile shader
    GL::compile_shader(shader);

    // check shader compilation success
    GLint compilationStatus = 0;
    GL::get_shader_iv(shader, GL_COMPILE_STATUS, &compilationStatus);
    if(compilationStatus == GL_FALSE){

        glGetShaderInfoLog(shader, m_infoLog.size(), nullptr, m_infoLog.data());

        Logger::error(std::format("[ShaderProgram::create_shader_from_source_code] Shader compilation failed: {} \n"sv, std::string_view(m_infoLog)));
        clean();
        return false;
    }

    return true;
}

auto ShaderProgram::initialize(const std::vector<GLuint> &shadersObjects) -> bool{

    m_handle = GL::create_program();

    // attach shaders objects to program
    for(const auto shaderObject : shadersObjects){
        GL::attach_shader(m_handle, shaderObject);
    }

    if(!link_program()){
        return false;
    }

    retrieve_attribs_info();
    retrieve_uniforms_info();

    detach_and_delete_shaders();

    return true;
}

auto ShaderProgram::detach_and_delete_shaders() -> void{

    // Retrieve count of attached shaders
    GLint numShaders = 0;
    GL::get_program_iv(m_handle, GL_ATTACHED_SHADERS, &numShaders);

    // Retrieve attached shaders
    std::vector<GLuint> shaderNames(static_cast<size_t>(numShaders));
    GL::get_attached_shaders(m_handle, numShaders, nullptr, shaderNames.data());

    // Detach and delete them all
    for (const GLuint shader : shaderNames) {
        GL::detach_shader(m_handle, shader);
        GL::delete_shader(shader);
    }
}

