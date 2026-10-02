
#pragma once

// base
#include "geometry/matrix4.hpp"

// opengl
#include "opengl/shader/shader_program.hpp"

#include <QDebug>

struct CommonUniforms{
    tool::geo::Pt3f camPosition;

    tool::geo::Mat4f projection;
    tool::geo::Mat4f view;
    tool::geo::Mat4f model;

    tool::geo::Mat3f normal;    // normal matrix
    tool::geo::Mat4f mv;        // model*view matrix
    tool::geo::Mat4f mvp;       // model*view*projection matrix

    std::string mvpStr              = "MVP";
    std::string mvStr               = "ModelViewMatrix";
    std::string modelMatrixStr      = "ModelMatrix";
    std::string viewMatrixStr       = "ViewMatrix";
    std::string projectionMatrixStr = "ProjectionMatrix";
    std::string normalMatrixM       = "NormalMatrix";

    auto update() -> void{
        mv     = model * view;        
        mvp    = mv * projection;
        normal = tool::geo::get_rotation_m3x3(tool::geo::transpose(tool::geo::inverse(model * view)));
        // normal = tool::geo::get_rotation_m3x3(mv);
    }

    auto set_uniforms(tool::gl::ShaderProgram &shader) -> void{
        shader.set_uniform_matrix(mvpStr,               mvp);
        shader.set_uniform_matrix(mvStr,                mv);
        shader.set_uniform_matrix(normalMatrixM,        normal);
        shader.set_uniform_matrix(modelMatrixStr,       model);
        shader.set_uniform_matrix(viewMatrixStr,        view);
        shader.set_uniform_matrix(projectionMatrixStr,  projection);
    }

    auto set_indentity_uniforms(tool::gl::ShaderProgram &shader) -> void{
        shader.set_uniform_matrix(mvpStr,               tool::geo::Mat4f::identity());
        shader.set_uniform_matrix(mvStr,                tool::geo::Mat4f::identity());
        shader.set_uniform_matrix(normalMatrixM,        tool::geo::Mat3f::identity());
        shader.set_uniform_matrix(modelMatrixStr,       tool::geo::Mat4f::identity());
        shader.set_uniform_matrix(viewMatrixStr,        tool::geo::Mat4f::identity());
        shader.set_uniform_matrix(projectionMatrixStr,  tool::geo::Mat4f::identity());
    }
};

