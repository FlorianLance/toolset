
#include "stencil_sample.hpp"

using namespace tool::gl;
auto StencilSample::initialize() -> bool{

    // solid
    {
        QFile file1(":/shader/learn/stencil/stencil_testing.vs");
        file1.open(QIODevice::ReadOnly);
        QFile file2(":/shader/learn/stencil/stencil_testing.fs");
        file2.open(QIODevice::ReadOnly);
        std::array source = {
            std::make_tuple(ShaderType::vertex,     file1.readAll().toStdString()),
            std::make_tuple(ShaderType::fragment,   file2.readAll().toStdString())
        };
        if(!stencilTesting.load_from_source_code(source)){
            QtLog::error(u"Cannot load stencilTesting shader.");
        }
    }

    // transparent
    {
        QFile file1(":/shader/learn/stencil/stencil_testing.vs");
        file1.open(QIODevice::ReadOnly);
        QFile file2(":/shader/learn/stencil/stencil_single_color.fs");
        file2.open(QIODevice::ReadOnly);
        std::array source = {
            std::make_tuple(ShaderType::vertex,     file1.readAll().toStdString()),
            std::make_tuple(ShaderType::fragment,   file2.readAll().toStdString())
        };
        if(!stencilSingleColor.load_from_source_code(source)){
            QtLog::error(u"Cannot load stencilSingleColor shader.");
        }
    }

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    // cube VAO
    cubeD.initialize(1.f);

    // plane VAO
    quadD.initialize(false);

    return true;
}

auto StencilSample::draw(CommonUniforms &cUniforms) -> void{

    GLuint currentFBO = GL::get_current_framebuffer();

    // configure global opengl state
    // -----------------------------
    GL::enable(GL_DEPTH_TEST);
    GL::depth_func(GL_LESS);

    GL::enable(GL_STENCIL_TEST);
    GL::stencil_func_separate(GL_FRONT_AND_BACK, GL_NOTEQUAL, 1, 0xFF);
    GL::stencil_op_separate(GL_FRONT_AND_BACK, GL_KEEP, GL_KEEP, GL_REPLACE);

    // render
    // ------
    // Définir la couleur de nettoyage (rouge)
    GLfloat clearColor[] = {1.0f, 0.0f, 0.0f, 1.0f};
    GL::clear_named_framebuffer_fv(currentFBO, GL_COLOR, 0, clearColor);
    GL::dsa_clear_depth_stencil(currentFBO, 1.f, 0);

    // set uniforms
    stencilSingleColor.use();
    stencilSingleColor.set_uniform_matrix("view", cUniforms.view);
    stencilSingleColor.set_uniform_matrix("projection", cUniforms.projection);

    stencilTesting.use();
    stencilTesting.set_uniform_matrix("view", cUniforms.view);
    stencilTesting.set_uniform_matrix("projection", cUniforms.projection);

    // draw floor as normal, but don't write the floor to the stencil buffer, we only care about the containers. We set its mask to 0x00 to not write to the stencil buffer.
    GL::stencil_mask_separate(GL_FRONT_AND_BACK, 0x00);
    // floor
    // glBindTexture(GL_TEXTURE_2D, floorTexture);
    stencilTesting.set_uniform_matrix("model", geo::transform(geo::Vec3f{3,3,3,},geo::Vec3f{90.f,0,0},geo::Vec3f{0,-0.5,0}));
    quadD.draw();
    // glBindVertexArray(0);

    // 1st. render pass, draw objects as normal, writing to the stencil buffer
    // --------------------------------------------------------------------
    GL::stencil_func_separate(GL_FRONT_AND_BACK, GL_ALWAYS, 1, 0xFF);
    GL::stencil_mask_separate(GL_FRONT_AND_BACK, 0xFF);
    // cubes
    // glBindVertexArray(cubeVAO);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, cubeTexture);
    // model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    stencilTesting.set_uniform_matrix("model", geo::transform(geo::Vec3f{1,1,1,},geo::Vec3f{},geo::Vec3f{-1.0f, 0.0f, -1.0f}));
    // glDrawArrays(GL_TRIANGLES, 0, 36);
    cubeD.draw();
    // model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    stencilTesting.set_uniform_matrix("model", geo::transform(geo::Vec3f{1,1,1,},geo::Vec3f{},geo::Vec3f{2.0f, 0.0f, 0.0f}));
    // glDrawArrays(GL_TRIANGLES, 0, 36);
    cubeD.draw();

    // 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
    // Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing
    // the objects' size differences, making it look like borders.
    // -----------------------------------------------------------------------------------------------------------------------------
    GL::stencil_func_separate(GL_FRONT_AND_BACK, GL_NOTEQUAL, 1, 0xFF);
    GL::stencil_mask_separate(GL_FRONT_AND_BACK, 0x00);
    GL::disable(GL_DEPTH_TEST);
    stencilSingleColor.use();
    // cubes
    float scale = 1.1f;
    stencilSingleColor.set_uniform_matrix("model", geo::transform(geo::Vec3f{scale,scale,scale,},geo::Vec3f{},geo::Vec3f{-1.0f, 0.0f, -1.0f}));
    cubeD.draw();
    stencilSingleColor.set_uniform_matrix("model", geo::transform(geo::Vec3f{scale,scale,scale,},geo::Vec3f{},geo::Vec3f{2.0f, 0.0f, 0.0f}));
    cubeD.draw();

    GL::stencil_mask_separate(GL_FRONT_AND_BACK, 0xFF);
    GL::stencil_func_separate(GL_FRONT_AND_BACK, GL_ALWAYS, 0, 0xFF);
    GL::disable(GL_DEPTH_TEST);
}
