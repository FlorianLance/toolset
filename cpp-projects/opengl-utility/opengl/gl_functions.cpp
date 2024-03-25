
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

#include "gl_functions.hpp"

// base
#include "utility/logger.hpp"

using namespace tool::gl;

static auto __stdcall gl_debug_output(
    GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar* message, const GLvoid* userParam) -> GLvoid{

    using namespace tool;
    static_cast<void>(length);
    static_cast<void>(userParam);

    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) {
        return;
    }

    Logger::error(std::format("[GL] Debug message ({}): {} \n", id, message));

    switch (source){
        case GL_DEBUG_SOURCE_API:               Logger::error("\tSource: API\n"); break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     Logger::error("\tSource: Window System\n"); break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:   Logger::error("\tSource: Shader Compiler\n"); break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:       Logger::error("\tSource: Third Party\n"); break;
        case GL_DEBUG_SOURCE_APPLICATION:       Logger::error("\tSource: Application\n"); break;
        case GL_DEBUG_SOURCE_OTHER:             Logger::error("\tSource: Other\n"); break;
    }

    switch (type){
        case GL_DEBUG_TYPE_ERROR:               Logger::error("\tType: Error\n"); break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: Logger::error("\tType: Deprecated Behaviour\n"); break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  Logger::error("\tType: Undefined Behaviour\n"); break;
        case GL_DEBUG_TYPE_PORTABILITY:         Logger::error("\tType: Portability\n"); break;
        case GL_DEBUG_TYPE_PERFORMANCE:         Logger::error("\tType: Performance\n"); break;
        case GL_DEBUG_TYPE_MARKER:              Logger::error("\tType: Marker\n"); break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          Logger::error("\tType: Push Group\n"); break;
        case GL_DEBUG_TYPE_POP_GROUP:           Logger::error("\tType: Pop Group\n"); break;
        case GL_DEBUG_TYPE_OTHER:               Logger::error("\tType: Other\n"); break;
    }

    bool abort = false;
    switch (severity){
    case GL_DEBUG_SEVERITY_HIGH:
        Logger::error("Severity: high\n");
        abort = true;
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        Logger::error("Severity: medium\n");
        abort = true;
        break;
    case GL_DEBUG_SEVERITY_LOW:
        Logger::error("Severity: low\n");
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        Logger::error("Severity: notification\n");
        break;
    }

    if(abort){
        Logger::error("Program will now abort.\n");
        std::abort();
    }
}


auto GL::init_glew() -> bool{

    Logger::message("Init glew.\n");
    if(GLenum initGlew = glewInit() != GLEW_OK){
        Logger::error(std::format("[GL::init_glew] init error: {}\n", reinterpret_cast<const char*>(glewGetErrorString(initGlew))));
        return false;
    }

    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT){
        Logger::message("Debug context available, debug message callback enabled.\n");
        GL::enable(GL_DEBUG_OUTPUT);
        GL::enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_debug_output, nullptr);
        glDebugMessageControl(
            GL_DONT_CARE,   // source
            GL_DONT_CARE,   // type
            GL_DONT_CARE,   // severity
            0,              // count
            nullptr,        // ids
            GL_TRUE         // enabled
        );
    }

    return true;
}

auto GL::display_glew_info() -> void{
    Logger::message(
        std::format(
            "----------------------OpenGL Info----------------------------\n"
            "Glew version: {} \n"
            "     Version: {} \n"
            "      Vendor: {} \n"
            "    Renderer: {} \n"
            "     Shading: {} \n"
            "----------------------------------------------------------------\n",
            reinterpret_cast<const char*>(glewGetString(GLEW_VERSION)),
            reinterpret_cast<const char*>(glGetString(GL_VERSION)),
            reinterpret_cast<const char*>(glGetString(GL_VENDOR)),
            reinterpret_cast<const char*>(glGetString(GL_RENDERER)),
            reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION))
        )
    );
}

auto GL::clear(GLbitfield mask) -> void{
    // Clear buffers to preset values.
    glClear(
        mask    // Bitwise OR of masks that indicate the buffers to be cleared.
                // The three masks are GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT, and GL_STENCIL_BUFFER_BIT.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glClear.xhtml
}

auto GL::clear_color(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) -> void{
    // Specify clear values for the color buffers.
    glClearColor(
        red,    // Specify the red, green, blue, and alpha values used when the color buffers are cleared. The initial values are all 0.
        green,
        blue,
        alpha
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glClearColor.xhtml
}

auto GL::clear_named_framebuffer_fv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, GLfloat *value) -> void{
    // Clear individual buffers of a framebuffer.
    glClearNamedFramebufferfv(
        framebuffer,    // Specifies the name of the framebuffer object for glClearNamedFramebuffer*.
                        //  framebuffer is zero, indicating the default draw framebuffer, or the name of a framebuffer object.
        buffer,         // Specify the buffer to clear.
                        //  GL_COLOR,  GL_DEPTH,  GL_STENCIL
        drawbuffer,     // Specify a particular draw buffer to clear.
                        // Must be 0 if GL_DEPTH or GL_STENCIL
        value           // A pointer to the value or values to clear the buffer to.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glClearBuffer.xhtml
}

auto GL::enable(GLenum cap) -> void{
    // Enable server-side GL capabilities.
    glEnable(
        cap  // Specifies a symbolic constant indicating a GL capability.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glEnable.xhtml

    // GL_BLEND
    // If enabled, blend the computed fragment color values with the values in the color buffers. See glBlendFunc.

    // GL_CLIP_DISTANCE i
    // If enabled, clip geometry against user-defined half space i.

    // GL_COLOR_LOGIC_OP
    // If enabled, apply the currently selected logical operation to the computed fragment color and color buffer values. See glLogicOp.

    //  GL_CULL_FACE
    // If enabled, cull polygons based on their winding in window coordinates. See glCullFace.

    // GL_DEBUG_OUTPUT
    // If enabled, debug messages are produced by a debug context. When disabled, the debug message log is silenced. Note that in a non-debug context, very few, if any messages might be produced, even when GL_DEBUG_OUTPUT is enabled.

    // GL_DEBUG_OUTPUT_SYNCHRONOUS
    // If enabled, debug messages are produced synchronously by a debug context. If disabled, debug messages may be produced asynchronously. In particular, they may be delayed relative to the execution of GL commands, and the debug callback function may be called from a thread other than that in which the commands are executed. See glDebugMessageCallback.

    // GL_DEPTH_CLAMP
    // If enabled, the −wc≤zc≤wc plane equation is ignored by view volume clipping (effectively, there is no near or far plane clipping). See glDepthRange.

    // GL_DEPTH_TEST
    // If enabled, do depth comparisons and update the depth buffer. Note that even if the depth buffer exists and the depth mask is non-zero, the depth buffer is not updated if the depth test is disabled. See glDepthFunc and glDepthRange.

    // GL_DITHER
    // If enabled, dither color components or indices before they are written to the color buffer.

    // GL_FRAMEBUFFER_SRGB
    // If enabled and the value of GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING for the framebuffer attachment corresponding to the destination buffer is GL_SRGB, the R, G, and B destination color values (after conversion from fixed-point to floating-point) are considered to be encoded for the sRGB color space and hence are linearized prior to their use in blending.

    // GL_LINE_SMOOTH
    // If enabled, draw lines with correct filtering. Otherwise, draw aliased lines. See glLineWidth.

    // GL_MULTISAMPLE
    // If enabled, use multiple fragment samples in computing the final color of a pixel. See glSampleCoverage.

    // GL_POLYGON_OFFSET_FILL
    // If enabled, and if the polygon is rendered in GL_FILL mode, an offset is added to depth values of a polygon's fragments before the depth comparison is performed. See glPolygonOffset.

    // GL_POLYGON_OFFSET_LINE
    // If enabled, and if the polygon is rendered in GL_LINE mode, an offset is added to depth values of a polygon's fragments before the depth comparison is performed. See glPolygonOffset.

    // GL_POLYGON_OFFSET_POINT
    // If enabled, an offset is added to depth values of a polygon's fragments before the depth comparison is performed, if the polygon is rendered in GL_POINT mode. See glPolygonOffset.

    // GL_POLYGON_SMOOTH
    // If enabled, draw polygons with proper filtering. Otherwise, draw aliased polygons. For correct antialiased polygons, an alpha buffer is needed and the polygons must be sorted front to back.

    // GL_PRIMITIVE_RESTART
    // Enables primitive restarting. If enabled, any one of the draw commands which transfers a set of generic attribute array elements to the GL will restart the primitive when the index of the vertex is equal to the primitive restart index. See glPrimitiveRestartIndex.

    // GL_PRIMITIVE_RESTART_FIXED_INDEX
    // Enables primitive restarting with a fixed index. If enabled, any one of the draw commands which transfers a set of generic attribute array elements to the GL will restart the primitive when the index of the vertex is equal to the fixed primitive index for the specified index type. The fixed index is equal to 2n−1
    // where n is equal to 8 for GL_UNSIGNED_BYTE, 16 for GL_UNSIGNED_SHORT and 32 for GL_UNSIGNED_INT.

    // GL_RASTERIZER_DISCARD
    //  If enabled, primitives are discarded after the optional transform feedback stage, but before rasterization. Furthermore, when enabled, glClear, glClearBufferData, glClearBufferSubData, glClearTexImage, and glClearTexSubImage are ignored.

    // GL_SAMPLE_ALPHA_TO_COVERAGE
    // If enabled, compute a temporary coverage value where each bit is determined by the alpha value at the corresponding sample location. The temporary coverage value is then ANDed with the fragment coverage value.

    // GL_SAMPLE_ALPHA_TO_ONE
    // If enabled, each sample alpha value is replaced by the maximum representable alpha value.

    // GL_SAMPLE_COVERAGE
    // If enabled, the fragment's coverage is ANDed with the temporary coverage value. If GL_SAMPLE_COVERAGE_INVERT is set to GL_TRUE, invert the coverage value. See glSampleCoverage.

    // GL_SAMPLE_SHADING
    // If enabled, the active fragment shader is run once for each covered sample, or at fraction of this rate as determined by the current value of GL_MIN_SAMPLE_SHADING_VALUE. See glMinSampleShading.

    // GL_SAMPLE_MASK
    //  If enabled, the sample coverage mask generated for a fragment during rasterization will be ANDed with the value of GL_SAMPLE_MASK_VALUE before shading occurs. See glSampleMaski.

    // GL_SCISSOR_TEST
    // If enabled, discard fragments that are outside the scissor rectangle. See glScissor.

    // GL_STENCIL_TEST
    // If enabled, do stencil testing and update the stencil buffer. See glStencilFunc and glStencilOp.

    // GL_TEXTURE_CUBE_MAP_SEAMLESS
    // If enabled, cubemap textures are sampled such that when linearly sampling from the border between two adjacent faces, texels from both faces are used to generate the final sample value. When disabled, texels from only a single face are used to construct the final sample value.

    // GL_PROGRAM_POINT_SIZE
    // If enabled and a vertex or geometry shader is active, then the derived point size is taken from the (potentially clipped) shader builtin gl_PointSize and clamped to the implementation-dependent point size range.
}

auto GL::disable(GLenum cap) -> void{
    // Disable server-side GL capabilities.
    glDisable(
        cap  // Specifies a symbolic constant indicating a GL capability.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glEnable.xhtml
}

auto GL::enable_i(GLenum cap, GLuint index) -> void{
    // Enable server-side GL capabilities.
    glEnablei(
        cap,    // Specifies a symbolic constant indicating a GL capability.
        index   // Specifies the index of the switch to disable (for glEnablei and glDisablei only).
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glEnable.xhtml
}

auto GL::disable_i(GLenum cap, GLuint index) -> void{
    // Disable server-side GL capabilities.
    glDisablei(
        cap,    // Specifies a symbolic constant indicating a GL capability.
        index   // Specifies the index of the switch to disable (for glEnablei and glDisablei only).
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glEnable.xhtml
}

auto GL::create_buffers(GLsizei n, GLuint *buffers) -> void{    
    // Create buffer objects.
    glCreateBuffers(
        n,          // Specifies the number of buffer objects to create.
        buffers     // Specifies an array in which names of the new buffer objects are stored.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glCreateBuffers.xhtml
}

auto GL::delete_buffers(GLsizei n, GLuint *buffers) -> void{    
    // Delete named buffer objects.
    glDeleteBuffers(
        n,          // Specifies the number of buffer objects to be deleted.
        buffers     // Specifies an array of buffer objects to be deleted.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDeleteBuffers.xhtml
}

auto GL::depth_mask(GLboolean flag) -> void{
    // Enable or disable writing into the depth buffer.
    glDepthMask(
        flag        // Specifies whether the depth buffer is enabled for writing.
                    // If flag is GL_FALSE, depth buffer writing is disabled.
                    // Otherwise, it is enabled. Initially, depth buffer writing is enabled.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDepthMask.xhtml
}

auto GL::create_framebuffers(GLsizei n, GLuint *framebuffers) -> void{
    // Create framebuffer objects.
    glCreateFramebuffers(
        n,              // Number of framebuffer objects to create.
        framebuffers    // Specifies an array in which names of the new framebuffer objects are stored.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glCreateFramebuffers.xhtml
}

auto GL::bind_framebuffer(GLenum target, GLuint framebuffer) -> void{
    // Bind a framebuffer to a framebuffer target.
    glBindFramebuffer(
        target,     // Specifies the framebuffer target of the binding operation.
                    // either GL_DRAW_FRAMEBUFFER, GL_READ_FRAMEBUFFER or GL_FRAMEBUFFER
        framebuffer // Specifies the name of the framebuffer object to bind.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindFramebuffer.xhtml
}

auto GL::named_framebuffer_texture(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level) -> void{
    // Attach a level of a texture object as a logical buffer of a framebuffer object.
    glNamedFramebufferTexture(
        framebuffer,    // Specifies the name of the framebuffer object for glNamedFramebufferTexture.
        attachment,     // Specifies the attachment point of the framebuffer.
        texture,        // Specifies the name of an existing texture object to attach.
        level           // Specifies the mipmap level of the texture object to attach.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glFramebufferTexture.xhtml
}

auto GL::named_framebuffer_renderbuffer(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) -> void{
    // Attach a renderbuffer as a logical buffer of a framebuffer object.
    glNamedFramebufferRenderbuffer(
        framebuffer,            // Specifies the name of the framebuffer object for glNamedFramebufferRenderbuffer.
        attachment,             // Specifies the attachment point of the framebuffer.
        renderbuffertarget,     // Specifies the renderbuffer target. Must be GL_RENDERBUFFER.
        renderbuffer            // Specifies the name of an existing renderbuffer object of type renderbuffertarget to attach.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glFramebufferRenderbuffer.xhtml
}

auto GL::named_framebuffer_read_buffer(GLuint framebuffer, GLenum mode) -> void{
    // Select a color buffer source for pixels.
    glNamedFramebufferReadBuffer(
        framebuffer,    // Specifies the name of the framebuffer object for glNamedFramebufferReadBuffer function.
        mode            // Specifies a color buffer.
                        // Accepted values are GL_FRONT_LEFT, GL_FRONT_RIGHT, GL_BACK_LEFT, GL_BACK_RIGHT, GL_FRONT,
                        // GL_BACK, GL_LEFT, GL_RIGHT, and the constants GL_COLOR_ATTACHMENTi.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glReadBuffer.xhtml
}

auto GL::named_framebuffer_draw_buffer(GLuint framebuffer, GLenum buf) -> void{
    // Specify which color buffers are to be drawn into.
    glNamedFramebufferDrawBuffer(
        framebuffer,    // Specifies the name of the framebuffer object for glNamedFramebufferDrawBuffer function.
                        // Must be zero or the name of a framebuffer object.
        buf             // For default framebuffer, the argument specifies up to four color buffers to be drawn into.
                        // Symbolic constants GL_NONE, GL_FRONT_LEFT, GL_FRONT_RIGHT, GL_BACK_LEFT, GL_BACK_RIGHT,
                        // GL_FRONT, GL_BACK, GL_LEFT, GL_RIGHT, and GL_FRONT_AND_BACK are accepted.
                        // The initial value is GL_FRONT for single-buffered contexts, and GL_BACK for double-buffered contexts.
                        // For framebuffer objects, GL_COLOR_ATTACHMENT$m$ and GL_NONE enums are accepted,
                        // where $m$ is a value between 0 and GL_MAX_COLOR_ATTACHMENTS.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawBuffer.xhtml
}

auto GL::named_framebuffer_draw_buffers(GLuint framebuffer, GLsizei n, const GLenum *bufs) -> void{    
    // Specifies a list of color buffers to be drawn into.
    glNamedFramebufferDrawBuffers(
        framebuffer,    // Specifies the name of the framebuffer object for glNamedFramebufferDrawBuffers.
        n,              // Specifies the number of buffers in bufs.
        bufs            // oints to an array of symbolic constants specifying the buffers into which fragment colors or data values will be written.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawBuffers.xhtml
}

auto GL::check_named_framebuffer_status(GLuint framebuffer, GLenum target) -> GLenum{    
    // Check the completeness status of a framebuffer.
    return glCheckNamedFramebufferStatus(
        framebuffer,    // Specify the target to which the framebuffer is bound for glCheckFramebufferStatus,
                        // and the target against which framebuffer completeness of framebuffer is checked for glCheckNamedFramebufferStatus.
        target          // Specifies the name of the framebuffer object for glCheckNamedFramebufferStatus
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glCheckFramebufferStatus.xhtml
}

auto GL::create_textures(GLenum target, GLsizei n, GLuint *textures) -> void{    
    // Create texture objects.
    glCreateTextures(
        target,     // Specifies the effective texture target of each created texture.
                    // target must be one of:
                    //  GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D, GL_TEXTURE_1D_ARRAY, GL_TEXTURE_2D_ARRAY,
                    //  GL_TEXTURE_RECTANGLE, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_BUFFER,
                    //  GL_TEXTURE_2D_MULTISAMPLE or GL_TEXTURE_2D_MULTISAMPLE_ARRAY.
        n,          // Number of texture objects to create.
        textures    // Specifies an array in which names of the new texture objects are stored.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glCreateTextures.xhtml
}

auto GL::delete_textures(GLsizei n, GLuint *textures) -> void{
    // Delete named textures
    glDeleteTextures(
        n,          // Specifies the number of textures to be deleted.
        textures    // Specifies an array of textures to be deleted.
    );
    // https://registry.khronos.org/OpenGL-Refpages/es3/html/glDeleteTextures.xhtml
}

auto GL::bind_textures(GLuint first, GLsizei count, const GLuint *textures) -> void{
    // Bind one or more named textures to a sequence of consecutive texture units.
    glBindTextures(
        first,      // Specifies the first texture unit to which a texture is to be bound.
        count,      // Specifies the number of textures to bind.
        textures    // Specifies the address of an array of names of existing texture objects.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindTextures.xhtml
}

auto GL::bind_image_texture(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format) -> void{
    // Bind a level of a texture to an image unit.
    glBindImageTexture(
        unit,       // Specifies the index of the image unit to which to bind the texture
        texture,    // Specifies the name of the texture to bind to the image unit.
        level,      // Specifies the level of the texture that is to be bound.
        layered,    // Specifies whether a layered texture binding is to be established.
        layer,      // If layered is GL_FALSE, specifies the layer of texture to be bound to the image unit. Ignored otherwise.
        access,     // Specifies a token indicating the type of access that will be performed on the image.
        format      // Specifies the format that the elements of the image will be treated as for the purposes of formatted stores.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindImageTexture.xhtml
}

auto GL::bind_image_textures(GLuint first, GLsizei count, const GLuint *textures) -> void{    
    // Bind one or more named texture images to a sequence of consecutive image units.
    glBindImageTextures(
        first,      // Specifies the first image unit to which a texture is to be bound.
        count,      // Specifies the number of textures to bind.
        textures    // Specifies the address of an array of names of existing texture objects.
    );
    // https://docs.gl/gl4/glBindImageTextures
}

auto GL::texture_parameter_i(GLuint texture, GLenum pname, GLint param) -> void{
    // Set texture parameters.
    glTextureParameteri(
        texture,    // Specifies the texture object name for glTextureParameter functions.
        pname,      // Specifies the symbolic name of a single-valued texture parameter.
                    // pname can be one of the following:
                    //  GL_DEPTH_STENCIL_TEXTURE_MODE, GL_TEXTURE_BASE_LEVEL, GL_TEXTURE_COMPARE_FUNC, GL_TEXTURE_COMPARE_MODE,
                    //  GL_TEXTURE_LOD_BIAS, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_LOD, GL_TEXTURE_MAX_LOD,
                    //  GL_TEXTURE_MAX_LEVEL, GL_TEXTURE_SWIZZLE_R, GL_TEXTURE_SWIZZLE_G, GL_TEXTURE_SWIZZLE_B, GL_TEXTURE_SWIZZLE_A,
                    //  GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T, or GL_TEXTURE_WRAP_R.
                    // For the vector commands (glTexParameter*v), pname can also be one of GL_TEXTURE_BORDER_COLOR or GL_TEXTURE_SWIZZLE_RGBA.
        param       // For the scalar commands, specifies the value of pname.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexParameter.xhtml
}

auto GL::texture_parameter_fv(GLuint texture, GLenum pname, const GLfloat *params) -> void{
    // Set texture parameters.
    // see texture_parameter_i
    glTextureParameterfv(
        texture,
        pname,
        params      // For the vector commands, specifies a pointer to an array where the value or values of pname are stored.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexParameter.xhtml
}

auto GL::texture_storage_2d_multisample(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations) -> void{
    // Specify storage for a two-dimensional multisample texture.
    glTextureStorage2DMultisample(
        texture,                // Specifies the texture object name for glTextureStorage2DMultisample.
                                // The effective target of texture must be one of the valid non-proxy target values above.
        samples,                // Specify the number of samples in the texture.
        internalformat,         // Specifies the sized internal format to be used to store texture image data.
        width,                  // Specifies the width of the texture, in texels.
        height,                 // Specifies the height of the texture, in texels.
        fixedsamplelocations    // Specifies whether the image will use identical sample locations and the same number of samples for all texels in the image,
                                // and the sample locations will not depend on the internal format or size of the image.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexStorage2DMultisample.xhtml
}

auto GL::texture_storage_2d(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) -> void{
    // Simultaneously specify storage for all levels of a two-dimensional or one-dimensional array texture.
    glTextureStorage2D(
        texture,        // Specifies the texture object name for glTextureStorage2D. The effective target of texture must be one of the valid non-proxy target values below:
                        // GL_TEXTURE_3D, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_CUBE_MAP_ARRAY, GL_PROXY_TEXTURE_3D, GL_PROXY_TEXTURE_2D_ARRAY or GL_PROXY_TEXTURE_CUBE_MAP_ARRAY.
        levels,         // Specify the number of texture levels.
        internalformat, // Specifies the sized internal format to be used to store texture image data.
        width,          // Specifies the width of the texture, in texels.
        height          // Specifies the height of the texture, in texels.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexStorage2D.xhtml
}

auto GL::texture_storage_3d(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth) -> void{
    // Simultaneously specify storage for all levels of a three-dimensional, two-dimensional array or cube-map array texture.
    glTextureStorage3D(
        texture,        // Specifies the texture object name for glTextureStorage3D. The effective target of texture must be one of the valid non-proxy target values below:
                        // GL_TEXTURE_3D, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_CUBE_MAP_ARRAY, GL_PROXY_TEXTURE_3D, GL_PROXY_TEXTURE_2D_ARRAY or GL_PROXY_TEXTURE_CUBE_MAP_ARRAY.
        levels,         // Specify the number of texture levels.
        internalformat, // Specifies the sized internal format to be used to store texture image data.
        width,          // Specifies the width of the texture, in texels.
        height,         // Specifies the height of the texture, in texels.
        depth           // Specifies the depth of the texture, in texels.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexStorage3D.xhtml
}

auto GL::texture_sub_image_2d(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels) -> void{
    // Specify a two-dimensional texture subimage
    glTextureSubImage2D(
        texture,        // Specifies the texture object name for glTextureSubImage2D. The effective target of texture must be one of the valid target values above.
        level,          // Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap reduction image.
        xoffset,        // Specifies a texel offset in the x direction within the texture array.
        yoffset,        // Specifies a texel offset in the y direction within the texture array.
        width,          // Specifies a texel offset in the y direction within the texture array.
        height,         // Specifies the height of the texture subimage.
        format,         // Specifies the format of the pixel data. The following symbolic values are accepted:
                        // GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_DEPTH_COMPONENT, and GL_STENCIL_INDEX.
        type,           // Specifies the data type of the pixel data. The following symbolic values are accepted:
                        // GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_FLOAT,
                        // GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV,
                        // GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV,
                        // GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, and GL_UNSIGNED_INT_2_10_10_10_REV.
        pixels          // Specifies a pointer to the image data in memory.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexSubImage2D.xhtml
}

auto GL::named_buffer_storage(GLuint buffer, GLsizeiptr size, const void *data, GLbitfield flags) -> void{
    // Creates and initializes a buffer object's immutable data store.
    glNamedBufferStorage(
        buffer,     // Specifies the name of the buffer object for glNamedBufferStorage function.
        size,       // Specifies the size in bytes of the buffer object's new data store.
        data,       // Specifies a pointer to data that will be copied into the data store for initialization, or NULL if no data is to be copied.
        flags       // Specifies the intended usage of the buffer's data store.
                    // Must be a bitwise combination of the following flags.
                    // GL_DYNAMIC_STORAGE_BIT, GL_MAP_READ_BIT GL_MAP_WRITE_BIT, GL_MAP_PERSISTENT_BIT, GL_MAP_COHERENT_BIT, and GL_CLIENT_STORAGE_BIT.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBufferStorage.xhtml

    // GL_DYNAMIC_STORAGE_BIT
    // The contents of the data store may be updated after creation through calls to glBufferSubData.
    // If this bit is not set, the buffer content may not be directly updated by the client.
    // The data argument may be used to specify the initial content of the buffer's data store regardless of the presence of the GL_DYNAMIC_STORAGE_BIT.
    // Regardless of the presence of this bit, buffers may always be updated with server-side calls such as glCopyBufferSubData and glClearBufferSubData.

    // GL_MAP_READ_BIT
    // The data store may be mapped by the client for read access and a pointer in the client's address space obtained that may be read from.

    // GL_MAP_WRITE_BIT
    // The data store may be mapped by the client for write access and a pointer in the client's address space obtained that may be written through.

    // GL_MAP_PERSISTENT_BIT
    // The client may request that the server read from or write to the buffer while it is mapped.
    // The client's pointer to the data store remains valid so long as the data store is mapped, even during execution of drawing or dispatch commands.

    // GL_MAP_COHERENT_BIT
    // Shared access to buffers that are simultaneously mapped for client access and are used by the server will be coherent,
    // so long as that mapping is performed using glMapBufferRange.
    // That is, data written to the store by either the client or server will be immediately visible to the other with no further action taken by the application.
    // In particular,
    //  If GL_MAP_COHERENT_BIT is not set and the client performs a write followed by a call to the glMemoryBarrier
    //      command with the GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT set, then in subsequent commands the server will see the writes.
    //  If GL_MAP_COHERENT_BIT is set and the client performs a write, then in subsequent commands the server will see the writes.
    //  If GL_MAP_COHERENT_BIT is not set and the server performs a write,
    //      the application must call glMemoryBarrier with the GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT set and then call glFenceSync with GL_SYNC_GPU_COMMANDS_COMPLETE (or glFinish). Then the CPU will see the writes after the sync is complete.
    //  If GL_MAP_COHERENT_BIT is set and the server does a write, the app must call glFenceSync with GL_SYNC_GPU_COMMANDS_COMPLETE (or glFinish).
    //      Then the CPU will see the writes after the sync is complete.

    // GL_CLIENT_STORAGE_BIT
    // When all other criteria for the buffer storage allocation are met,
    // this bit may be used by an implementation to determine whether to use storage that is local to the server
    //  or to the client to serve as the backing store for the buffer.
}

auto GL::named_buffer_sub_data(GLuint buffer, GLintptr offset, GLsizeiptr size, const void *data) -> void{    
    // Updates a subset of a buffer object's data store.
    glNamedBufferSubData(
        buffer,     // Specifies the name of the buffer object for glNamedBufferSubData.
        offset,     // Specifies the offset into the buffer object's data store where data replacement will begin, measured in bytes.
        size,       // Specifies the size in bytes of the data store region being replaced.
        data        // Specifies a pointer to the new data that will be copied into the data store.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBufferSubData.xhtml
}

auto GL::bind_buffer_range(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size) -> void{
    // Bind a range within a buffer object to an indexed buffer target.
    glBindBufferRange(
        target,     // Specify the target of the bind operation. target must be one of
                    // GL_ATOMIC_COUNTER_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER, GL_UNIFORM_BUFFER, or GL_SHADER_STORAGE_BUFFER.
        index,      // Specify the index of the binding point within the array specified by target.
        buffer,     // The name of a buffer object to bind to the specified binding point.
        offset,     // The starting offset in basic machine units into the buffer object buffer.
        size        // The amount of data in machine units that can be read from the buffer object while used as an indexed target.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindBufferRange.xhtml
}

auto GL::bind_buffers_range(GLenum target, GLuint index, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLintptr *sizes) -> void{
    // Bind ranges of one or more buffer objects to a sequence of indexed buffer targets.
    glBindBuffersRange(
        target,     // Specify the target of the bind operation. target must be one of
                    // GL_ATOMIC_COUNTER_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER, GL_UNIFORM_BUFFER, or GL_SHADER_STORAGE_BUFFER.
        index,      // Specify the index of the first binding point within the array specified by target.
        count,      // Specify the number of contiguous binding points to which to bind buffers.
        buffers,    // A pointer to an array of names of buffer objects to bind to the targets on the specified binding point, or NULL.
        offsets,    // A pointer to an array of offsets into the corresponding buffer in buffers to bind, or NULL if buffers is NULL.
        sizes       // A pointer to an array of sizes of the corresponding buffer in buffers to bind, or NULL if buffers is NULL.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindBuffersRange.xhtml
}

auto GL::vertex_array_vertex_buffer(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride) -> void{    
    // Bind a buffer to a vertex buffer bind point.
    glVertexArrayVertexBuffer(
        vaobj,          // Specifies the name of the vertex array object to be used by glVertexArrayVertexBuffer function.
        bindingindex,   // The index of the vertex buffer binding point to which to bind the buffer.
        buffer,         // The name of a buffer to bind to the vertex buffer binding point.
        offset,         // The offset of the first element of the buffer.
        stride          // The distance between elements within the buffer.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindVertexBuffer.xhtml
}

auto GL::enable_vertex_array_attrib(GLuint vaobj, GLuint index) -> void{
    // Enable or disable a generic vertex attribute array.
    glEnableVertexArrayAttrib(
        vaobj,  // Specifies the name of the vertex array object for glDisableVertexArrayAttrib and glEnableVertexArrayAttrib functions.
        index   // Specifies the index of the generic vertex attribute to be enabled or disabled.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glEnableVertexAttribArray.xhtml
}

auto GL::vertex_array_attrib_format(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset) -> void{
    // Specify the organization of vertex arrays
    glVertexArrayAttribFormat(
        vaobj,          // Specifies the name of the vertex array object for glVertexArrayAttrib{I, L}Format functions.
        attribindex,    // The generic vertex attribute array being described.
        size,           // The number of values per vertex that are stored in the array.
        type,           // he type of the data stored in the array.
        normalized,     // Specifies whether fixed-point data values should be normalized (GL_TRUE)
                        // or converted directly as fixed-point values (GL_FALSE) when they are accessed. This parameter is ignored if type is GL_FIXED.
        relativeoffset  // The distance between elements within the buffer.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribFormat.xhtml
}

auto GL::vertex_array_attrib_binding(GLuint vaobj, GLuint attribindex, GLuint bindingindex) -> void{
    // Associate a vertex attribute and a vertex buffer binding for a vertex array object
    glVertexArrayAttribBinding(
        vaobj,          // Specifies the name of the vertex array object for glVertexArrayAttribBinding.
        attribindex,    // The index of the attribute to associate with a vertex buffer binding.
        bindingindex    // The index of the vertex buffer binding with which to associate the generic vertex attribute.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribBinding.xhtml
}

auto GL::use_program(GLuint program) -> void{
    // Installs a program object as part of current rendering state.
    glUseProgram(
        program         // Specifies the handle of the program object whose executables are to be used as part of current rendering state.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glUseProgram.xhtml
}

auto GL::draw_arrays_instance_base_instance(GLenum mode, GLint first, GLsizei count, GLsizei instancecount, GLuint baseinstance) -> void{
    // Draw multiple instances of a range of elements with offset applied to instanced attributes
    glDrawArraysInstancedBaseInstance(
        mode,           // Specifies what kind of primitives to render
        first,          // Specifies the starting index in the enabled arrays.
        count,          // Specifies the number of indices to be rendered.
        instancecount,  // Specifies the number of instances of the specified range of indices to be rendered.
        baseinstance    // Specifies the base instance for use in fetching instanced vertex attributes.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawArraysInstancedBaseInstance.xhtml
}

auto GL::draw_elements_instanced_base_vertex_base_instance(GLenum mode, GLsizei count, GLenum type, void *indices, GLsizei instancecount, GLint basevertex, GLuint baseinstance) -> void{
    // Render multiple instances of a set of primitives from array data with a per-element offset.
    glDrawElementsInstancedBaseVertexBaseInstance(
        mode,           // Specifies what kind of primitives to render.
                        // Symbolic constants GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_LINES_ADJACENCY, GL_LINE_STRIP_ADJACENCY, GL_TRIANGLES_ADJACENCY, GL_TRIANGLE_STRIP_ADJACENCY and GL_PATCHES are accepted.
        count,          // Specifies the number of elements to be rendered.
        type,           // Specifies the type of the values in indices. Must be one of GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, or GL_UNSIGNED_INT.
        indices,        // Specifies a pointer to the location where the indices are stored.
        instancecount,  // Specifies the number of instances of the indexed geometry that should be drawn.
        basevertex,     // Specifies a constant that should be added to each element of indices when chosing elements from the enabled vertex arrays.
        baseinstance    // Specifies the base instance for use in fetching instanced vertex attributes.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawElementsInstancedBaseVertexBaseInstance.xhtml
}

auto GL::bind_buffer(GLenum target, GLuint buffer) -> void{    
    // Bind a named buffer object.
    glBindBuffer(
        target, // Specifies the target to which the buffer object is bound, which must be one of the buffer binding targets in the following table:
                //  Buffer Binding Target        Purpose
                //  GL_ARRAY_BUFFER              Vertex attributes
                //  GL_ATOMIC_COUNTER_BUFFER 	 Atomic counter storage
                //  GL_COPY_READ_BUFFER          Buffer copy source
                //  GL_COPY_WRITE_BUFFER         Buffer copy destination
                //  GL_DISPATCH_INDIRECT_BUFFER  Indirect compute dispatch commands
                //  GL_DRAW_INDIRECT_BUFFER      Indirect command arguments
                //  GL_ELEMENT_ARRAY_BUFFER      Vertex array indices
                //  GL_PIXEL_PACK_BUFFER         Pixel read target
                //  GL_PIXEL_UNPACK_BUFFER       Texture data source
                //  GL_QUERY_BUFFER              Query result buffer
                //  GL_SHADER_STORAGE_BUFFER 	 Read-write storage for shaders
                //  GL_TEXTURE_BUFFER            Texture data buffer
                //  GL_TRANSFORM_FEEDBACK_BUFFER Transform feedback buffer
                //  GL_UNIFORM_BUFFER            Uniform block storage
        buffer  // Specifies the name of a buffer object.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindBuffer.xhtml
}

auto GL::enable_vertex_attrib_array(GLuint index) -> void{    
    // Enable or disable a generic vertex attribute array.
    glEnableVertexAttribArray(
        index   // Specifies the index of the generic vertex attribute to be enabled or disabled.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glEnableVertexAttribArray.xhtml
}

auto GL::vertex_attrib_i_pointer(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer) -> void{
    // Define an array of generic vertex attribute data.
    glVertexAttribIPointer(
        index,  // Specifies the index of the generic vertex attribute to be modified.
        size,   // Specifies the number of components per generic vertex attribute.
                // Must be 1, 2, 3, 4. Additionally, the symbolic constant GL_BGRA is accepted by glVertexAttribPointer. The initial value is 4.
        type,   // Specifies the data type of each component in the array.
                // The symbolic constants GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, and GL_UNSIGNED_INT
                //  are accepted by glVertexAttribPointer and glVertexAttribIPointer.
                // Additionally GL_HALF_FLOAT, GL_FLOAT, GL_DOUBLE, GL_FIXED, GL_INT_2_10_10_10_REV, GL_UNSIGNED_INT_2_10_10_10_REV and GL_UNSIGNED_INT_10F_11F_11F_REV are accepted by glVertexAttribPointer.
                // GL_DOUBLE is also accepted by glVertexAttribLPointer and is the only token accepted by the type parameter for that function. The initial value is GL_FLOAT.
        stride, // Specifies the byte offset between consecutive generic vertex attributes.
                // If stride is 0, the generic vertex attributes are understood to be tightly packed in the array.
                // The initial value is 0.
        pointer // Specifies a offset of the first component of the first generic vertex attribute in the array
                // in the data store of the buffer currently bound to the GL_ARRAY_BUFFER target.
                // The initial value is 0.
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml
}

auto GL::vertex_attrib_pointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) -> void{    
    // Define an array of generic vertex attribute data.
    // see: vertex_attrib_i_pointer
    glVertexAttribPointer(
        index,
        size,
        type,
        normalized, // For glVertexAttribPointer, specifies whether fixed-point data values should be normalized (GL_TRUE)
                    // or converted directly as fixed-point values (GL_FALSE) when they are accessed.
        stride,
        pointer
    );
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml
}
