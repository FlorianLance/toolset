/*******************************************************************************
** Toolset-imgui-opengl-engine                                                **
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

#include "shaders_tests.hpp"

using namespace tool::gl;

ColoredCloudShader::ColoredCloudShader(){

    // init shader
    const char* vertexShaderSource = R"shaderDef(
    #version 430 core

    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;

    // model
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    // camera
    uniform vec3 camera_position;

    // cloud
    uniform float size_pt = 3.f;

    // color
    uniform vec4 unicolor = vec4(1,0,0,1);
    uniform bool enable_unicolor = false;
    uniform float factor_unicolor = 0.5f;
    out vec4 color;

    void main(){
        vec4 p = view * model * vec4(aPos, 1.0);
        gl_Position = projection*p;
        color = enable_unicolor ? mix(unicolor, vec4(aColor, 1.0), factor_unicolor) : vec4(aColor, 1.0);
        float l = sqrt(length(p.xyz-camera_position.xyz));
        gl_PointSize = size_pt;///(l);
    }

     )shaderDef";

    const char* fragmentShaderSource = R"shaderDef(
    #version 430 core
    out vec4 FragColor;
    in vec4 color;

    void main(){
        FragColor = color;
    }

    )shaderDef";

    std::array elems = {
        std::make_tuple(ShaderType::vertex,   std::string(vertexShaderSource)),
        std::make_tuple(ShaderType::fragment, std::string(fragmentShaderSource))
    };

    load_from_source_code(elems);
}

bool ColoredMeshShader::init(){

    // init shader
    const char* vertexShaderSource = R"shaderDef(
                                     #version 430 core

                                     // layout
                                     layout (location = 0) in vec3 aPos;
                                     layout (location = 1) in vec3 aColor;

                                     // uniform
                                     uniform mat4 model;
                                     uniform mat4 view;
                                     uniform mat4 projection;
                                     uniform vec4 unicolor = vec4(1,1,0,1);
                                     uniform bool enable_unicolor = false;

                                     layout (binding = 0) uniform BlobSettings{
                                     vec4 InnerColor;
                                     vec4 OuterColor;
                                     };


                                     // out
                                     out vec4 color;

                                     void main(){
                                     gl_Position = projection*view * model * vec4(aPos, 1.0);
                                     color = enable_unicolor ? unicolor : vec4(aColor, 1.0);
                                     }

                                     )shaderDef";

    const char* fragmentShaderSource = R"shaderDef(
                                       #version 330 core
                                       out vec4 FragColor;
                                       in vec4 color;

                                       void main(){
                                       FragColor = color;
                                       }

                                       )shaderDef";

    std::array elems = {
        std::make_tuple(ShaderType::vertex,   std::string(vertexShaderSource)),
        std::make_tuple(ShaderType::fragment, std::string(fragmentShaderSource))
    };

    return load_from_source_code(elems);
}

bool TexturedMeshShader::init(){

    // init shader
    const char *vertexShaderSource = "#version 430 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "layout (location = 1) in vec2 aTexCoord;\n"
                                     "uniform mat4 model;\n"
                                     "uniform mat4 view;\n"
                                     "uniform mat4 projection;\n"
                                     "out vec2 texCoord;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = projection*view * model * vec4(aPos, 1.0);\n"
                                     "   texCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
                                     "}\0";
    const char *fragmentShaderSource = "#version 430 core\n"
                                       "out vec4 FragColor;\n"
                                       "in vec2 texCoord;\n"
                                       "uniform sampler2D texture1;\n"
                                       "uniform sampler2D texture2;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   FragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), 0.2);\n"
                                       "   // FragColor = FragColor = vec4(texCoord.x, texCoord.y, 0,  1.0f);\n"
                                       "}\n\0";

    std::array elems = {
        std::make_tuple(ShaderType::vertex,   std::string(vertexShaderSource)),
        std::make_tuple(ShaderType::fragment, std::string(fragmentShaderSource))
    };
    return load_from_source_code(elems);
}

bool TexturedColoredMeshShader::init(){

    // init shader
    const char *vertexShaderSource = "#version 430 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "layout (location = 1) in vec3 aColor;\n"
                                     "layout (location = 2) in vec2 aTexCoord;\n"
                                     "uniform mat4 model;\n"
                                     "uniform mat4 view;\n"
                                     "uniform mat4 projection;\n"
                                     "out vec3 ourColor;\n"
                                     "out vec2 texCoord;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = projection*view * model * vec4(aPos, 1.0);\n"
                                     "   ourColor = aColor;\n"
                                     "   texCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
                                     "}\0";
    const char *fragmentShaderSource = "#version 430 core\n"
                                       "out vec4 FragColor;\n"
                                       "in vec3 ourColor;\n"
                                       "in vec2 texCoord;\n"
                                       "uniform sampler2D texture1;\n"
                                       "uniform sampler2D texture2;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   //FragColor = texture(texture1, texCoord);\n"
                                       "   //FragColor = vec4(1,texCoord.x, texCoord.y,1);//vec4(texCoord.x,texCoord.y,0,1);\n"
                                       "   //FragColor = texture(texture1, texCoord) * vec4(ourColor, 1.0);\n"
                                       "   FragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), 0.2);\n"
                                       "}\n\0";

    std::array elems = {
        std::make_tuple(ShaderType::vertex,   std::string(vertexShaderSource)),
        std::make_tuple(ShaderType::fragment, std::string(fragmentShaderSource))
    };
    return load_from_source_code(elems);
}

bool TexturedNormalsMeshShader::init(){

    // init shader
    const char* vertexShaderSource = R"shaderDef(
                                     #version 450 core
                                     #extension GL_ARB_bindless_texture : require

                                     // layout
                                     layout (location = 0) in vec3 aPos;
                                     layout (location = 1) in vec3 aNormal;
                                     layout (location = 2) in vec2 aTextCoord;

                                     // uniform
                                     uniform mat4 model;
                                     uniform mat4 view;
                                     uniform mat4 projection;

                                     // out
                                     out vec4 normal;
                                     out vec2 textCoord;

                                     void main(){
                                     gl_Position = projection*view * model * vec4(aPos, 1.0);
                                     normal    = vec4(aNormal.xyz, 1);
                                     textCoord = aTextCoord;
                                     }

                                     )shaderDef";

    const char* fragmentShaderSource = R"shaderDef(
                                       #version 450 core
                                       out vec4 FragColor;

                                       in vec4 normal;
                                       in vec2 textCoord;

                                       uniform layout (location = 10) sampler2D texture_diffuse1;
                                       //
                                       //uniform samplers
                                       //{
                                       //    sampler2D arr[10];
                                       //};

                                       //layout(bindless_sampler,  binding = 0) uniform samplers
                                       //{
                                       //    sampler2D textures[10];
                                       //};


                                       void main(){
                                       //if(gl_FragCoord.x < 200)
                                       //    FragColor = vec4(textCoord.x,textCoord.y,0,0.1);
                                       //else if(gl_FragCoord.x < 400)
                                       //    FragColor = vec4(normal.x,normal.y,0,1);
                                       //else
                                       FragColor = texture(texture_diffuse1, textCoord);
                                       //FragColor = vec4(1,0,0,0.2);
                                       }

                                       )shaderDef";

    std::array elems = {
        std::make_tuple(ShaderType::vertex,   std::string(vertexShaderSource)),
        std::make_tuple(ShaderType::fragment, std::string(fragmentShaderSource))
    };
    return load_from_source_code(elems);
}

