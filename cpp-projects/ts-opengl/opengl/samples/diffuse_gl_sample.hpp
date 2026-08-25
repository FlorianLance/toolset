
// #pragma once

// // base
// #include "geometry/camera.hpp"
// #include "geometry/screen.hpp"

// // opengl
// #include "opengl/texture/textures_manager.hpp"
// #include "opengl/shader/shaders_manager.hpp"

// #include "opengl/draw/lines_drawers.hpp"
// #include "opengl/buffer/framebuffer_object.hpp"
// #include "opengl/shader/uniform_buffer_object.hpp"
// #include "opengl/shader/shader_storage_buffer_object.hpp"
// #include "opengl/buffer/pixel_buffer_object.hpp"
// #include "opengl/buffer/atomic_buffer_object.hpp"
// #include "opengl/gl_material.hpp"
// #include "opengl/texture/sampler.hpp"
// #include "opengl/texture/geometry_texture_2d_tbo.hpp"
// #include "opengl/draw/points_drawers.hpp"


// namespace tool::gl{

// struct BaseScene{
//     virtual ~BaseScene(){}
// };

// struct BaseGlSample{

//     BaseGlSample(geo::Camera cam, geo::Screen screen) :  m_camera(cam), m_screen(screen){
//         m_shadersM = graphics::ShadersManager::get_instance();
//     }

//     virtual ~BaseGlSample(){}
//     virtual auto initialize() -> bool{return false;}
//     virtual auto draw() -> void{}
//     virtual auto update() -> void{}

//     auto reload_current_shader() -> void{}

// protected:

//     virtual auto draw_scene() -> void{}

// private:

//     auto draw_common_elements() -> void{}

// protected:

//     geo::Camera m_camera;
//     geo::Screen m_screen;

//     graphics::ShadersManager *m_shadersM   = nullptr;
//     gl::TexturesManager *m_texturesM  = nullptr;

//     std::shared_ptr<ShaderProgram> m_shader = nullptr;
//     std::shared_ptr<BaseScene> m_scene = nullptr;

//     geo::Pt4d m_mobileLightPos1 = geo::Pt4d(5.0, 5.0, 5.0, 1.0);
// };


// struct DiffuseGlSample : public BaseGlSample{

//     geo::Pt3f kd = geo::Pt3f{0.9f, 0.5f, 0.3f};
//     geo::Pt3f ld = geo::Pt3f{0.5f, 0.5f, 0.5f};

//     auto initialize() -> bool final override{
//         return (m_shader = m_shadersM->get_shader("ch3/diffuse").lock()) != nullptr;
//     }

//     auto draw() -> void final override{

//         m_shader->use();
//         m_shader->set_uniform("LightPosition", m_camera.view_matrix().multiply_point(m_mobileLightPos1).conv<float>());
//         m_shader->set_uniform("Kd", kd);
//         m_shader->set_uniform("Ld", ld);

//         draw_scene();
//     }
// };

// }