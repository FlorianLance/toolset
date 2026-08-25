
#pragma once

// base
#include "geometry/matrix4.hpp"


struct CommonUniforms{
    tool::geo::Pt3f camPosition;

    tool::geo::Mat4f projection;
    tool::geo::Mat4f view;
    tool::geo::Mat4f model;

    tool::geo::Mat3f normal;    // normal matrix
    tool::geo::Mat4f mv;        // model*view matrix
    tool::geo::Mat4f mvp;       // model*view*projection matrix

    auto update() -> void{
        mv     = model * view;
        // normal = tool::geo::get_rotation_m3x3(mv);
        mvp    = mv * projection;


        normal = tool::geo::get_rotation_m3x3(tool::geo::transpose(tool::geo::inverse(model * view)));
    }
};

