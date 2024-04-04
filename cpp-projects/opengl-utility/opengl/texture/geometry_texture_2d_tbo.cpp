
#include "geometry_texture_2d_tbo.hpp"

using namespace tool::gl;


auto GeometryTexture2D::init_position(GLsizei w, GLsizei h) -> void{
    
    TBO::initialize();
    TBO::init_data_f32(w, h, 1, 3);

    TextureOptions options;
    options.minFilter = TextureMinFilter::nearest;
    options.magFilter = TextureMagFilter::nearest;
    options.wrapS = TextureWrapMode::clamp_to_edge; // test
    options.wrapT = TextureWrapMode::clamp_to_edge; // test
    options.maxLevel  = 0;
    TBO::set_texture_options(options);
}

auto GeometryTexture2D::init_color(GLsizei w, GLsizei h) -> void{
    
    TBO::initialize();
    TBO::init_data_u8(w, h, 1, 3);

    TextureOptions options;
    options.minFilter = TextureMinFilter::nearest;
    options.magFilter = TextureMagFilter::nearest;
    options.maxLevel  = 0;
    TBO::set_texture_options(options);
}

auto GeometryTexture2D::init_ao(GLsizei w, GLsizei h) -> void{
    
    TBO::initialize();
    TBO::init_data_f32(w, h, 1, 1);

    TextureOptions options;
    options.minFilter = TextureMinFilter::nearest;
    options.magFilter = TextureMagFilter::nearest;
    options.maxLevel  = 0;
    TBO::set_texture_options(options);
}


auto GeometryMultisampleTexture2D::init_color(GLsizei w, GLsizei h, GLsizei samples) -> void{
    
    TBO::initialize();
    TBO::init_multisample_data_u8(w, h, 1, 3, samples);

    TextureOptions options;
    options.minFilter = TextureMinFilter::nearest;
    options.magFilter = TextureMagFilter::nearest;
    options.maxLevel  = 0;
    TBO::set_texture_options(options);

}

auto GeometryMultisampleTexture2D::init_position(GLsizei w, GLsizei h, GLsizei samples) -> void{
    
    TBO::initialize();
    TBO::init_multisample_data_f32(w, h, 1, 3, samples);

    TextureOptions options;
    options.minFilter = TextureMinFilter::nearest;
    options.magFilter = TextureMagFilter::nearest;
    options.maxLevel  = 0;
    TBO::set_texture_options(options);
}

