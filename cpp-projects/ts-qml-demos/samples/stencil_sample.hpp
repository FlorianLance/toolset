
#pragma once

// local
#include "base_sample.hpp"

namespace tool::gl{

class StencilSample : public BaseGlSample{
    Q_OBJECT
public:

    ShaderProgram stencilTesting;
    ShaderProgram stencilSingleColor;
    CommonUniforms cUniforms;

    tool::gl::CubeTrianglesDrawer cubeD;
    tool::gl::QuadTrianglesDrawer quadD;

    auto initialize() -> bool final override;
    auto draw(CommonUniforms &cUniforms) -> void final override;
};
}