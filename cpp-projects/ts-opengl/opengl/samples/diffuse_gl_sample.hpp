
#pragma once

// base
#include "geometry/camera.hpp"
#include "geometry/screen.hpp"

// opengl
#include "opengl/draw/lines_drawers.hpp"
#include "opengl/buffer/framebuffer_object.hpp"
#include "opengl/shader/uniform_buffer_object.hpp"
#include "opengl/shader/shader_storage_buffer_object.hpp"
#include "opengl/buffer/pixel_buffer_object.hpp"
#include "opengl/buffer/atomic_buffer_object.hpp"
#include "opengl/gl_material.hpp"
#include "opengl/texture/sampler.hpp"
#include "opengl/texture/geometry_texture_2d_tbo.hpp"
#include "opengl/draw/points_drawers.hpp"
#include "opengl/shader/shaders_manager.hpp"

namespace tool::gl{

struct BaseGlSample{

    BaseGlSample(geo::Camera cam, geo::Screen screen) :  m_camera(cam), m_screen(screen){}
    virtual auto initialize() -> bool{return false;}

protected:

    geo::Camera m_camera;
    geo::Screen m_screen;
    graphics::ShadersManager *m_shadersM   = nullptr;
};


struct DiffuseGlSample : public BaseGlSample{

    auto initialize() -> bool final override{


        if((m_shader = m_shadersM->get_shader("colored-cloud").lock()) == nullptr){
            return false;
        }


        return true;
    }
//     auto draw(gl::BaseDrawer *drawer = nullptr) -> void final override;
//     auto update_imgui() -> void final override;
//     geo::Pt3f kd = geo::Pt3f{0.9f, 0.5f, 0.3f};
//     geo::Pt3f ld = geo::Pt3f{0.5f, 0.5f, 0.5f};
private:

    std::shared_ptr<ShaderProgram> m_shader = nullptr;
};

}