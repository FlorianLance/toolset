

/*******************************************************************************
** Toolset-qt-utility                                                         **
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

// Qt
#include <QImage>
#include <QFile>
#include <QDebug>

// opengl-utility
#include "opengl/shader/shader_program.hpp"
#include "opengl/texture/texture_2d_tbo.hpp"

namespace tool::gl{

class ShaderQt{
public:

    bool load_from_files(const char* pathVertexShader, const char* pathFragmentShader){

        QString vertexCode   = read_file(pathVertexShader);
        QString fragmentCode = read_file(pathFragmentShader);

        if(vertexCode.length() == 0){
            qWarning() << "Vertex shader is empty : " << pathVertexShader;
            return false;
        }
        if(fragmentCode.length() == 0){
            qWarning() << "Fragment shader is empty : " << pathFragmentShader;
            return false;
        }

        gl = std::make_unique<gl::ShaderProgram>();
        return gl->load_from_source_code({{Shader::Type::vertex, vertexCode.toStdString().c_str()}, {Shader::Type::fragment, fragmentCode.toStdString().c_str()}});
    }

    std::unique_ptr<gl::ShaderProgram> gl = nullptr;

private :

    QString read_file(QString path){

        QFile file(path);
        if(!file.open(QFile::OpenModeFlag::ReadOnly)){
            qWarning() << "Cannot read " << path << " file";
            return "";
        }

        QTextStream in(&file);
        return in.readAll();
    }
};


//class Texture2dQt{
//public :

//    bool load(const char* pathImage, QImage::Format format){

//        if(!load_texture(pathImage, format)){
//            qWarning() << "Cannot read image " << pathImage;
//            return false;
//        }
//        if(image.isNull()){
//            qWarning() << "Image empty" << pathImage;
//            return false;
//        }

//        GLenum glFormat = GL_RGB;
//        if(format == QImage::Format::Format_RGBA8888){
//            glFormat = GL_RGBA;
//        }
//        gl = std::make_unique<Texture2dBuffer>(image.bits(), image.width(), image.height(), glFormat);
//        return true;
//    }

//    std::unique_ptr<Texture2dBuffer> gl = nullptr;

//private:

//    bool load_texture(const char *path, QImage::Format format){
//        image = QImage(path);
//        if(image.isNull()){
//            return false;
//        }
//        image = image.mirrored().convertToFormat(format);
//        return true;
//    }

//    QImage image;
//};
}
