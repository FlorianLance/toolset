
#include "samples.hpp"

// std
#include <algorithm>
#include <execution>
#include <format>

// imgui
#include "imgui-sfml/imgui-SFML.h"
#include "imgui-tb/imgui_ui_drawer.hpp"

// base
#include "utility/io_file.hpp"
#include "utility/logger.hpp"
#include "utility/string.hpp"

// opengl-utility
#include "opengl/utility/gl_utility.hpp"
#include "opengl/gl_functions.hpp"

//#include "glm/mat4x4.hpp"

using namespace tool::geo;
using namespace tool::graphics;
using namespace tool::gl;

Sample::Sample(Camera *cam) : // managers
      shadersM(ShadersManager::get_instance()),
      texturesM(TexturesManager::get_instance()),
      modelsM(ModelsManager::get_instance()),
      drawersM(DrawersManager::get_instance()),
      // camera
      camera(cam)
{}

auto Sample::parent_init() -> bool{

    //  common shaders
    if(commonShaders.empty()){

        // store
        // commonShaders["cloud"sv]                = shadersM->get_shader("others/cloud"sv).lock();
        
        Log::message("1\n");
        commonShaders["floor"sv]                = shadersM->get_shader("ch5/scene-texture"sv).lock();
        Log::message("2\n");
        commonShaders["unicolor"sv]             = shadersM->get_shader("others/unicolor"sv).lock();
        Log::message("3\n");
        commonShaders["skybox"sv]               = shadersM->get_shader("others/skybox"sv).lock();
        commonShaders["ch7-solid"sv]            = shadersM->get_shader("ch7/solid"sv).lock();
        commonShaders["ch8-solid"sv]            = shadersM->get_shader("ch8/solid"sv).lock();
        commonShaders["shadow_mapping_depth"sv] = shadersM->get_shader("learn/3_1_1_shadow_mapping_depth"sv).lock();
        commonShaders["debug_quad"sv]           = shadersM->get_shader("learn/3_1_1_debug_quad"sv).lock();

        // check
        for(const auto &shader : commonShaders){
            if(shader.second == nullptr){
                Log::error(std::format("[Sample::parent_init] Shader with alias [{}] is null.\n", shader.first));
                return false;
            }
        }
    }

    //  common drawers
    if(commonDrawers.empty()){

        // store
        commonDrawers["frustum"sv]             = drawersM->get_drawer("frustum"sv).lock();
        commonDrawers["sphere"sv]              = drawersM->get_drawer("sphere"sv).lock();
        commonDrawers["cube"sv]                = drawersM->get_drawer("cube"sv).lock();
        commonDrawers["floor"sv]               = drawersM->get_drawer("floor"sv).lock();
        commonDrawers["torus"sv]               = drawersM->get_drawer("torus"sv).lock();
        commonDrawers["skybox"sv]              = drawersM->get_drawer("skybox"sv).lock();
        // commonDrawers["teapot"sv]              = drawersM->get_drawer("teapot"sv).lock();
        commonDrawers["notext-plane-10x10"sv]  = drawersM->get_drawer("notext-plane-10x10"sv).lock();
        commonDrawers["notext-plane-20x10"sv]  = drawersM->get_drawer("notext-plane-20x10"sv).lock();
        commonDrawers["notext-plane-40x40"sv]  = drawersM->get_drawer("notext-plane-40x40"sv).lock();
        commonDrawers["notext-spot"sv]         = drawersM->get_drawer("notext-spot"sv).lock();
        commonDrawers["spot"sv]                = drawersM->get_drawer("spot"sv).lock();
        commonDrawers["grid-floor"sv]          = drawersM->get_drawer("grid-floor"sv).lock();
        commonDrawers["dragon"sv]              = drawersM->get_drawer("dragon"sv).lock();
        commonDrawers["cloud"sv]               = drawersM->get_drawer("cloud"sv).lock();

        // check
        for(const auto &drawer : commonDrawers){
            if(drawer.second == nullptr){
                Log::error(std::format("[Sample::parent_init] Drawer with alias [{}] is null.\n", drawer.first));
                return false;
            }
        }
    }

    materialFloorUBO.initialize();
    materialFloorUBO.set_data_space_from_shader(commonShaders["floor"sv].get());

    materialUBO.initialize();        
    lightUBO.initialize();

    floorM.Ka = {0.5f, 0.5f, 0.5f};
    floorM.Kd = {0.5f, 0.5f, 0.5f};
    floorM.Ks = {0.8f, 0.8f, 0.8f};
    floorM.Shininess = 10.0f;

    // call child init
    return init();
}

auto Sample::update(float elapsedSeconds) -> void{

    camera->set_fov(camFov);
    camM.update_vp(camera->view(),camera->projection());

    this->elapsedSeconds = elapsedSeconds;

    // update lights positions
    float lightRotationSpeed = 1.5f;
    float deltaT = elapsedSeconds * lightRotationSpeed;

    alpha = deltaT;

    auto ray= 20.f;
    auto x = ray * cos(alpha);
    auto z = ray * sin(alpha);
    if(moveLight){
        geo::Pt3f offset = {0,5.f,0.f};
        mobileLightPos1 = to_pt4(offset + geo::Pt3f{x,0,z}, 1.f);
        offset = {0,4.f,1.f};
        mobileLightPos2 = to_pt4(offset + geo::Pt3f{-x,0,z}, 1.f);
    }
}


auto Sample::parent_draw(tool::gl::BaseDrawer *drawer) -> void {

    currentDrawer = drawer;

    if(drawLights){
        draw_lights();
    }
    draw_skybox();

    if(drawFloor){
        draw_floor();
    }

    // // update animation of current drawer
    // nbAnimations = 0;
    // if(auto modelDrawer = dynamic_cast<gl::ModelDrawer*>(drawer)){
    //     if(auto model = modelDrawer->model()){

    //         nbAnimations = static_cast<int>(model->animations.size());

    //         if(idAnimation < nbAnimations){

    //             durationAnimation = static_cast<float>(model->animations[idAnimation].duration);

    //             Log::message(std::format("animate {} {} {}\n", model->animations[idAnimation].name, timeAnimation, durationAnimation));
    //             modelDrawer->update_animation(
    //                 model->animations[idAnimation].name,
    //                 stopAnimation ? timeAnimation : elapsedSeconds
    //             );
    //         }
    //     }
    // }
}

auto Sample::parent_update_imgui() -> void{

    // Expose a couple of the available flags. In most cases you may just call BeginTabBar() with no flags (0).
    static ImGuiTabBarFlags tabBarFags =
        ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_FittingPolicyDefault_;

    const char* names[5] = { "Misc", "Lights", "Materials", "Model", "Current"};

    ImGuiColorEditFlags miscFlags = ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoOptions;

    if (ImGui::BeginTabBar("Common", tabBarFags)){

        for (int n = 0; n < 5; n++)
            if (ImGui::BeginTabItem(names[n], nullptr, ImGuiTabItemFlags_None)){
                switch (n) {
                case 0:{
                    ImGui::Text("########### Shader:");
                    if(ImGui::Button("Reload shader###reload_shader_button")){
                        reload_shader();
                    }

                    ImGui::Text("Open current shader:");
                    ImGui::SameLine();


                    size_t id = 0;
                    auto sPaths = sampleShader->shaders_file_paths();
                    for(const auto &sp : sPaths){
                        auto ext = str::split(sp, '.').back();
                        if(ImGui::Button(std::format("{}###open_current_shader_{}",ext,ext).c_str())){
                            File::execute_file(sp);
                        }
                        if(id++ < sPaths.size()-1){
                            ImGui::SameLine();
                        }
                    }
                    ImGui::Spacing();

                    ImGui::Text("########### Misc:");
                    ImGui::Checkbox("Draw floor###Misc1", &drawFloor);
                    ImGui::Spacing();

                    ImGui::Text("########### Camera:");
                    ImGui::SliderFloat("FOV###Camera1", &camFov, 10.f, 150.f, "v = %.1f");

                    {
                        auto p = camera->position().conv<float>();
                        auto d = camera->direction().conv<float>();
                        auto u = camera->up().conv<float>();
                        if(ImGui::DragFloat3("Position###Camera2",p.array.data(), 0.1f, -100.f, 100.f)){
                            camera->set_position(p.conv<double>());
                        }
                        if(ImGui::DragFloat3("Direction###Camera3",d.array.data(), 0.01f, -1.f, 1.f)){
                            camera->set_direction(d.conv<double>(), u.conv<double>());
                        }
                        if(ImGui::DragFloat3("Up###Camera4",u.array.data(), 0.01f, -1.f, 1.f)){
                            camera->set_up_vector(u.conv<double>());
                        }
                    }

                    ImGui::Text("########### Skybox:");
                    ImGui::Checkbox("Draw###Skybox1", &drawSkybox);
                    ImGui::SameLine();
                    ImGui::SliderFloat3("Rotation###Skybox2", skyboxRot.array.data(), -360.f, 360.f, "° = %.1f");

                    break;
                }case 1:
                    ImGui::Checkbox("Move###Lights1", &moveLight);
                    ImGui::SameLine();
                    ImGui::Checkbox("Draw###Lights2", &drawLights);

                    ImGui::ColorEdit3("Ambiant###Lights3", lInfo.La.array.data(), miscFlags);
                    ImGui::ColorEdit3("Diffuse###Lights4", lInfo.Ld.array.data(), miscFlags);
                    ImGui::ColorEdit3("Specular###Lights5", lInfo.Ls.array.data(), miscFlags);
                    break;
                case 2:
                    ImGui::Text("########### Blinn-phong:");
                    ImGui::ColorEdit3("Ambiant###BPM1",  mInfo.Ka.array.data(), miscFlags);
                    ImGui::ColorEdit3("Diffuse###BPM2",  mInfo.Kd.array.data(), miscFlags);
                    ImGui::ColorEdit3("Specular###BPM3", mInfo.Ks.array.data(), miscFlags);
                    ImGui::SliderFloat("Shininess###BPM4",  &mInfo.Shininess, 0.f, 1000.f, "v = %.1f");
                    break;
                case 3: // model
                    ImGui::Text("### Infos:");
                    if(currentDrawer != nullptr){
                        ImGui::Text("Current drawer:");
//                         if(auto modelDrawer = dynamic_cast<gl::ModelDrawer*>(currentDrawer)){
//                             auto model = modelDrawer->model();
//                             ImGui::Indent();
//                             tool::ImGuiUiDrawer::text(std::format("Number of meshes: {}", model->meshes.size()));

//                             tool::ImGuiUiDrawer::text(std::format("Number of textures: {}", model->textures.size()));
//                             tool::ImGuiUiDrawer::text(std::format("Number of animations: {}", model->animations.size()));
//                             ImGui::Unindent();

// //                            modelDrawer->model()->textures.at(
// //
// //                            if(modelDrawer->model()->textures.size() > 0){
// //                                if(ImGui::BeginCombo("###textures", modelDrawer->model()->textures.at(currentTextureId)){
// //                                    for(size_t ii = 0; ii < modelDrawer->model()->textures.size(); ++ii){
// //                                        bool selected = ii == currentTextureId;
// //                                        if (ImGui::Selectable(modelDrawer->model()->textures[currentTextureId].c_str(),selected)){
// //                                            currentTextureId = ii;
// //                                        }
// //                                        if(selected){
// //                                            ImGui::SetItemDefaultFocus();
// //                                        }
// //                                    }
// //                                    ImGui::EndCombo();
// //                                }
// //                            }

//                         }else{
//                             ImGui::Text("Procedural drawer.");
//                         }

                    }else{
                        ImGui::Text("No current drawer defined.");
                    }


                    ImGui::Text("### Coords:");
                    ImGui::SliderInt3("Numbers###M1", nb.array.data(), 1, 10);
                    ImGui::DragFloat3("Position###M2", modelPos.array.data(), 0.05f, -10.f, 10.f, "ratio = %.2f");
                    ImGui::SliderFloat3("Rotation###M3", modelRot.array.data(), -360.f, 360.f, "ratio = %.1f");
                    ImGui::SliderFloat("Scale###M4", &scale, 0.01f, 5.f, "ratio = %.2f");
                    ImGui::Text("### Animation:");
                    ImGui::SliderInt("Id###M5", &idAnimation, 0, nbAnimations-1);
                    ImGui::Checkbox("Stop animation###M6", &stopAnimation);
                    ImGui::SliderFloat("Time animation###M7", &timeAnimation, 0.f, durationAnimation, "ratio = %.2f");

                    ImGui::DragFloat3("xyz###M8", xyz.array.data(), 0.05f, -10.f, 10.f, "ratio = %.2f");

                    break;
                case 4:
                    update_imgui();
                }
                ImGui::EndTabItem();
            }
        ImGui::EndTabBar();
    }
}

auto Sample::draw_nb(gl::ShaderProgram *shader, tool::gl::BaseDrawer *drawer) -> void{

    float s = scale*drawer->scaleHint;
    auto p = modelPos.conv<double>();
    for(int ii = 0; ii < nb.x(); ++ii){
        for(int jj = 0; jj < nb.y(); ++jj){
            for(int kk = 0; kk < nb.z(); ++kk){

                camM.m = geo::transform(
                    {{s,s,s}},
                    modelRot.conv<double>(),
                    {{p.x() + 1.f*(ii-nb.x()/2), p.y() + 1.f*(jj-nb.y()/2), p.z() + 1.f*(kk-nb.z()/2)}}
                );
                update_matrices();
                shader->set_camera_matrices_uniforms(camM);
                // drawer->draw(shader);
                drawer->draw();

            }
        }
    }
}

auto Sample::update_matrices() -> void{
    camM.update();
}

auto Sample::update_matrices_m(const Mat4d &model) -> void{
    camM.update_m(model);
}

auto Sample::update_matrices_p(const Mat4d &proj) -> void{
    camM.update_p(proj);
}

auto Sample::update_matrices_vp(const Mat4d &view, const Mat4d &proj)
     -> void{
    camM.update_vp(view, proj);
}

auto Sample::update_matrices_mvp(const Mat4d &model, const Mat4d &view, const Mat4d &proj) -> void{
    camM.update_mvp(model, view, proj);
}

auto Sample::draw_screen_quad(tool::gl::ShaderProgram *shader) -> void{

    CameraMatrices sqM;
    sqM.update_mvp(geo::Mat4d::identity(),geo::Mat4d::identity(), geo::Mat4d::identity());
    shader->set_camera_matrices_uniforms(sqM);
    drawersM->draw("screen-quad"sv, shader);
}

auto Sample::draw_floor() -> void{

    update_matrices_m(geo::transform(Vec3d{10.,10.,10.},Vec3d{0.,0.,0},Vec3d{0.,-4.,0.}));

    gl::TBO::unbind_textures(0,1);

    auto fs = commonShaders["floor"sv].get();
    fs->use();
    fs->set_uniform("Light.L",  Vec3f{0.8f,0.8f,0.8f});
    fs->set_uniform("Light.La", Vec3f{0.2f,0.2f,0.2f});
    fs->set_uniform("Light.Position",camera->view().conv<float>().multiply_point(Sample::worldLight));
    fs->set_camera_matrices_uniforms(camM);

    materialFloorUBO.update(floorM);
    materialFloorUBO.bind(0);

    commonDrawers["floor"sv]->draw();
}


auto Sample::draw_lights() -> void{

    commonShaders["unicolor"sv]->use();
    commonShaders["unicolor"sv]->set_uniform("unicolor", geo::Pt3f{1.f,1.f,0.f});

    camM.m = geo::transform(Vec3d{1.0,1,1},{}, Sample::worldLight.xyz().conv<double>());
    update_matrices();
    commonShaders["unicolor"sv]->set_camera_matrices_uniforms(camM);
    commonDrawers["sphere"sv]->draw();

    camM.m = geo::transform(Vec3d{0.3,0.3,0.3},{}, Sample::mobileLightPos1.xyz().conv<double>());
    update_matrices();
    commonShaders["unicolor"sv]->set_camera_matrices_uniforms(camM);
    commonDrawers["sphere"sv]->draw();

    camM.m = geo::transform(Vec3d{0.3,0.3,0.3},{}, Sample::mobileLightPos2.xyz().conv<double>());
    update_matrices();
    commonShaders["unicolor"sv]->set_camera_matrices_uniforms(camM);
    commonDrawers["sphere"sv]->draw();
}


auto Sample::draw_skybox() -> void{

    if(!drawSkybox){
        gl::TBO::unbind_textures(0,1);
        return;
    }

    if(auto shader = commonShaders["skybox"sv]; shader != nullptr){

        camM.m = geo::transform(Vec3d{1.,1.,1.},skyboxRot.conv<double>(),Vec3d{0.,0.,0.});
        update_matrices();

        shader->use();
        shader->set_uniform_matrix("MVP"sv, camM.mvp.conv<float>());

        commonDrawers["skybox"sv]->texturesId.values = {texturesM->cube_map_id("pisa")};
        commonDrawers["skybox"sv]->draw();
    }
}

auto Sample::draw_scene1(tool::gl::ShaderProgram *shader) -> void{

    mInfo.Kd = {0.9f, 0.3f, 0.2f};
    mInfo.Ks = {1.0f, 1.0f, 1.0f};
    mInfo.Ka = {0.2f, 0.2f, 0.2f};
    mInfo.Shininess = 100.0;
    materialUBO.update(mInfo);

    // backdrop plane
    CameraMatrices sCam;
    sCam.v = camM.v;
    sCam.p = camM.p;

    sCam.m = geo::transform(Vec3d{1.0,1.0,1.0}, Vec3d{-90.0,0.0,0.0}, Vec3d{0,5,0});
    sCam.update();
    shader->set_camera_matrices_uniforms(sCam);    
    commonDrawers["notext-plane-20x10"sv]->draw();

    // bottom plane
    sCam.m = geo::transform(Vec3d{1.0,1.0,1.0}, Vec3d{-90.0,0.0,0.0}, Vec3d{0,-5,0});
    sCam.update();
    shader->set_camera_matrices_uniforms(sCam);
    commonDrawers["notext-plane-20x10"sv]->draw();

    // top plane
    sCam.m = geo::transform(Vec3d{1.0,1.0,1.0}, Vec3d{180.0,0.0,0.0}, Vec3d{0,5,0});
    sCam.update();
    shader->set_camera_matrices_uniforms(sCam);
    commonDrawers["notext-plane-20x10"sv]->draw();

    // sphere
    mInfo.Kd = {0.4f, 0.9f, 0.4f};
    materialUBO.update(mInfo);

    sCam.m = geo::transform(Vec3d{1.0,1.0,1.0}, Vec3d{180.0,0.0,0.0}, Vec3d{-3,-3,2.0});
    sCam.update();
    shader->set_camera_matrices_uniforms(sCam);
    // commonDrawers["notext-plane-20x10"sv]->draw();

    // teapot
    mInfo.Kd = {0.4f, 0.4f, 0.9f};
    materialUBO.update(mInfo);
    sCam.m = geo::transform(Vec3d{1.0,1.0,1.0}, Vec3d{-90.0,0.0,0.0}, Vec3d{4,-5,1.5});
    sCam.update();
    shader->set_camera_matrices_uniforms(sCam);
    // commonDrawers["notext-plane-20x10"sv]->draw();
}

auto Sample::reload_shader() -> void{
    if(sampleShader != nullptr){
        Log::message("Try to reload current shader.\n");
        if(auto reloadedShader = shadersM->reload_shader(sampleShader.get()); reloadedShader != nullptr){
            sampleShader = reloadedShader;
        }
    }
}

auto Ch3Diffuse::init() -> bool{

    if((sampleShader = shadersM->get_shader("ch3/diffuse").lock()) == nullptr){
        return false;
    }
    return true;
}


auto Ch3Diffuse::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    sampleShader->use();    
    sampleShader->set_uniform("LightPosition", Pt4f{camera->view().multiply_point(mobileLightPos1.conv<double>()).conv<float>()});
    sampleShader->set_uniform("Kd", kd);
    sampleShader->set_uniform("Ld", ld);
    draw_nb(sampleShader.get(), drawer);
}

auto Ch3Diffuse::update_imgui() -> void{
    ImGui::DragFloat3("Kd###CH3-kd", kd.array.data(), 0.01f, 0.0f, 1.00f, "ratio = %.2f");
    ImGui::DragFloat3("Ld###CH3-Ld", ld.array.data(), 0.01f, 0.0f, 1.00f, "ratio = %.2f");
}


auto Ch3Flat::init() -> bool{

    if((sampleShader = shadersM->get_shader("ch3/flat").lock()) == nullptr){
        return false;
    }

    return true;
}

auto Ch3Flat::update_parameters() -> void{

    lightUBO.clean();
    lightUBO.initialize();
    lightUBO.set_data_space_from_shader(sampleShader.get());

    materialUBO.clean();
    materialUBO.initialize();
    materialUBO.set_data_space_from_shader(sampleShader.get());
    materialUBO.update(mInfo);
}

auto Ch3Flat::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    sampleShader->use();

    lInfo.Position = camera->view().multiply_point(worldLight.conv<double>()).conv<float>();
    lightUBO.update(lInfo);
    lightUBO.bind(0);

    materialUBO.bind(1);
    draw_nb(sampleShader.get(),drawer);
}


auto Ch3Discard::init() -> bool{

    if((sampleShader = shadersM->get_shader("ch3/discard").lock()) == nullptr){
        return false;
    }


    return true;
}

auto Ch3Discard::update_parameters() -> void{

    lightUBO.clean();
    lightUBO.initialize();
    lightUBO.set_data_space_from_shader(sampleShader.get());

    materialUBO.clean();
    materialUBO.initialize();
    materialUBO.set_data_space_from_shader(sampleShader.get());
    materialUBO.update(mInfo);
}

auto Ch3Discard::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    sampleShader->use();

    lInfo.Position = camera->view().multiply_point(worldLight.conv<double>()).conv<float>();
    lightUBO.update(lInfo);
    lightUBO.bind(0);


    materialUBO.bind(1);

    sampleShader->set_uniform("discardV", discardV);
    sampleShader->set_uniform("scaleV", scaleV);
    draw_nb(sampleShader.get(),drawer);
}

auto Ch3Discard::update_imgui() -> void{
    ImGui::SliderFloat("discard value:", &discardV, 0.f, 1.f, "r = %.3f");
    ImGui::SliderFloat("scale value:", &scaleV, 0.f, 100.f, "v = %.2f");
}


auto Ch3TwoSide::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch3/twoside").lock()) == nullptr){
        return false;
    }

    return true;
}

auto Ch3TwoSide::update_parameters() -> void{

    lightUBO.clean();
    lightUBO.initialize();
    lightUBO.set_data_space_from_shader(sampleShader.get());

    materialUBO.clean();
    materialUBO.initialize();
    materialUBO.set_data_space_from_shader(sampleShader.get());
    materialUBO.update(mInfo);
}

auto Ch3TwoSide::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    sampleShader->use();

    lInfo.Position = camera->view().multiply_point(worldLight.conv<double>()).conv<float>();
    lightUBO.update(lInfo);
    lightUBO.bind(0);

    materialUBO.bind(1);

    draw_nb(sampleShader.get(),drawer);
}

auto Ch3Phong::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch3/phong").lock()) == nullptr){
        return false;
    }

    return true;
}

auto Ch3Phong::update_parameters() -> void{

    lightUBO.clean();
    lightUBO.initialize();
    lightUBO.set_data_space_from_shader(sampleShader.get());

    materialUBO.clean();
    materialUBO.initialize();
    materialUBO.set_data_space_from_shader(sampleShader.get());
    materialUBO.update(mInfo);
}


auto Ch3Phong::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    sampleShader->use();

    lInfo.Position = camera->view().multiply_point(mobileLightPos1.conv<double>()).conv<float>();

    lightUBO.update(lInfo);
    lightUBO.bind(0);

    materialUBO.bind(1);

    draw_nb(sampleShader.get(),drawer);
}

auto Ch4PhongDirectionnalLight::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch4/phong-directional-light").lock()) == nullptr){
        return false;
    }


    return true;
}

auto Ch4PhongDirectionnalLight::update_parameters() -> void{

    materialUBO.clean();
    materialUBO.initialize();
    materialUBO.set_data_space_from_shader(sampleShader.get());
    materialUBO.update(mInfo);
}

auto Ch4PhongDirectionnalLight::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    sampleShader->use();
    sampleShader->set_uniform("Light.L",  Vec3f{0.8f,0.8f,0.8f});
    sampleShader->set_uniform("Light.La", lInfo.La);
    sampleShader->set_uniform("Light.Position", Pt4f{camera->view().multiply_point(worldLight.conv<double>()).conv<float>()});

    materialUBO.bind(0);

    draw_nb(sampleShader.get(),drawer);
}


auto Ch4BlinnPhong::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch4/blinn-phong").lock()) == nullptr){
        return false;
    }

    return true;
}

auto Ch4BlinnPhong::update_parameters() -> void{

    lightUBO.clean();
    lightUBO.initialize();
    lightUBO.set_data_space_from_shader(sampleShader.get());

    materialUBO.clean();
    materialUBO.initialize();
    materialUBO.set_data_space_from_shader(sampleShader.get());
    materialUBO.update(mInfo);
}


auto Ch4BlinnPhong::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    sampleShader->use();

    lInfo.Position = camera->view().multiply_point(mobileLightPos1.conv<double>()).conv<float>();

    lightUBO.update(lInfo);
    lightUBO.bind(0);

    materialUBO.bind(1);

    draw_nb(sampleShader.get(),drawer);
}


auto Ch4Cartoon::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch4/cartoon").lock()) == nullptr){
        return false;
    }

    return true;
}

auto Ch4Cartoon::update_parameters() -> void{
    materialUBO.clean();
    materialUBO.initialize();
    materialUBO.set_data_space_from_shader(sampleShader.get());
    materialUBO.update(mInfo);
}

auto Ch4Cartoon::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    sampleShader->use();
    sampleShader->set_uniform("Light.L",  lInfo.Ld);//Vec3f{0.8f,0.8f,0.8f});
    sampleShader->set_uniform("Light.La", lInfo.La);
    sampleShader->set_uniform("Light.Position", Pt4f{camera->view().multiply_point(mobileLightPos1.conv<double>()).conv<float>()});
    sampleShader->set_uniform("levels", levels);

    materialUBO.bind(0);

    draw_nb(sampleShader.get(),drawer);
}

auto Ch4Cartoon::update_imgui() -> void{
    ImGui::SliderInt("Levels", &levels, 1, 20);
}

auto Ch4PhongMultiLights::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch4/phong-multi-lights").lock()) == nullptr){
        return false;
    }

    return true;
}

auto Ch4PhongMultiLights::update_parameters() -> void{
    materialUBO.clean();
    materialUBO.initialize();
    materialUBO.set_data_space_from_shader(sampleShader.get());
    materialUBO.update(mInfo);
}

auto Ch4PhongMultiLights::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    std::vector<Pt4f> lPos ={
        Pt4f{camera->view().multiply_point(Pt4d{5.0,5.0,2.0,1.0}).conv<float>()},
        Pt4f{camera->view().multiply_point(Pt4d{0.0,5.0,2.0,1.0}).conv<float>()},
        Pt4f{camera->view().multiply_point(Pt4d{5.0,0.0,2.0,1.0}).conv<float>()},
        Pt4f{camera->view().multiply_point(Pt4d{5.0,5.0,0.0,1.0}).conv<float>()},
        Pt4f{camera->view().multiply_point(Pt4d{0.0,5.0,0.0,1.0}).conv<float>()},
    };

    std::vector<Vec3f> lL ={
        Vec3f{0.f,0.8f,0.8f},
        Vec3f{8.f,0.8f,0.8f},
        Vec3f{0.f,0.8f,0.0f},
        Vec3f{0.f,0.8f,0.8f},
        Vec3f{0.8f,0.8f,0.0f}
    };

    std::vector<Vec3f> lLa ={
        Vec3f{0.f,0.2f,0.2f},
        Vec3f{0.f,0.2f,0.2f},
        Vec3f{0.f,0.2f,0.2f},
        Vec3f{0.f,0.2f,0.2f},
        Vec3f{0.f,0.2f,0.2f}
    };

    sampleShader->use();
    for(size_t ii = 0; ii < lPos.size(); ++ii){
        std::string lightName = "lights[" + std::to_string(ii) + "].";
        sampleShader->set_uniform((lightName + "L").c_str(), lL[ii]);
        sampleShader->set_uniform((lightName + "La").c_str(), lLa[ii]);
        sampleShader->set_uniform((lightName + "Position").c_str(), lPos[ii]);
    }

    materialUBO.bind(0);

    draw_nb(sampleShader.get(), drawer);
}


auto Ch4PhongPerFragment::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch4/phong-per-fragment").lock()) == nullptr){
        return false;
    }

    return true;
}

auto Ch4PhongPerFragment::update_parameters() -> void{
    materialUBO.clean();
    materialUBO.initialize();
    materialUBO.set_data_space_from_shader(sampleShader.get());
    materialUBO.update(mInfo);
}

auto Ch4PhongPerFragment::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    sampleShader->use();
    sampleShader->set_uniform("Light.L",  Vec3f{0.f,0.8f,0.8f});
    sampleShader->set_uniform("Light.La", lInfo.La);
    sampleShader->set_uniform("Light.Position", Pt4f{camera->view().multiply_point(mobileLightPos1.conv<double>()).conv<float>()});

    materialUBO.update(mInfo);
    materialUBO.bind(0);

    draw_nb(sampleShader.get(),drawer);
}


auto Ch4PBR::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch4/pbr").lock()) == nullptr){
        return false;
    }

    lights.resize(3);
    lights[0].La = Vec3f{45.0f,45.0f,45.0f};
    lights[1].La = Vec3f{15.0f,15.0f,15.0f};
    lights[2].La = Vec3f{30.0f,30.0f,30.0f};

    pbrMatUBO.initialize();
    pbrMatUBO.load_data(&mPbrInfo, 1*sizeof(MaterialPbr),GL_DYNAMIC_STORAGE_BIT);

    lightsUBO.initialize();
    lightsUBO.load_data(lights.data(), lights.size()*sizeof(Light2), GL_DYNAMIC_STORAGE_BIT);

    return true;
}


auto Ch4PBR::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    sampleShader->use();
    lights[0].Position = camera->view().conv<float>().multiply_point(mobileLightPos1);
    lights[1].Position = camera->view().conv<float>().multiply_point(mobileLightPos2);
    lights[2].Position = camera->view().conv<float>().multiply_point(worldLight);
    lightsUBO.update_data(lights.data(), lights.size()*sizeof(Light2));
    lightsUBO.bind(0);

    pbrMatUBO.update_data(&mPbrInfo, 1*sizeof(MaterialPbr));
    pbrMatUBO.bind(1);

    draw_nb(sampleShader.get(), drawer);
}

auto Ch4PBR::update_imgui() -> void{
    ImGui::ColorEdit4("PBR color", mPbrInfo.color.array.data());
    ImGui::SliderFloat("PBR rough", &mPbrInfo.rough, 0.0f, 1.00f, "ratio = %.3f");
    ImGui::SliderFloat("PBR metal", &mPbrInfo.metal, 0.0f, 1.00f, "ratio = %.3f");
}


auto Ch5DiscardPixels::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch5/discard-pixels").lock()) == nullptr){
        return false;
    }

    if((cementMossD = drawersM->get_drawer("cement-moss-cube"sv).lock()) == nullptr){
        return false;
    }

    return true;
}

auto Ch5DiscardPixels::update_parameters() -> void{
    materialUBO.clean();
    materialUBO.initialize();
    materialUBO.set_data_space_from_shader(sampleShader.get());
    materialUBO.update(mInfo);
}

auto Ch5DiscardPixels::draw(tool::gl::BaseDrawer *) -> void{

    parent_draw();

    sampleShader->use();
    sampleShader->set_uniform("Light.L",  Vec3f{0.8f,0.8f,0.8f});
    sampleShader->set_uniform("Light.La", lInfo.La);
    sampleShader->set_uniform("Light.Position", Pt4f{camera->view().multiply_point(mobileLightPos1.conv<double>()).conv<float>()});
    sampleShader->set_uniform_matrix("ModelViewMatrix",   camM.mv.conv<float>());
    sampleShader->set_uniform("decay_factor",      decayFactor);

    materialUBO.bind(0);

    draw_nb(sampleShader.get(), cementMossD.get());
}

auto Ch5DiscardPixels::update_imgui() -> void{
    ImGui::SliderFloat("Decay", &decayFactor, 0.0f, 1.00f, "ratio = %.3f");
}

auto Ch5SceneTexture::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch5/scene-texture").lock()) == nullptr){
        return false;
    }

    return true;
}

auto Ch5SceneTexture::update_parameters() -> void{
    materialUBO.clean();
    materialUBO.initialize();
    materialUBO.set_data_space_from_shader(sampleShader.get());
    materialUBO.update(mInfo);
}

auto Ch5SceneTexture::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    sampleShader->use();
    sampleShader->set_uniform("Light.L",  Vec3f{0.8f,0.8f,0.8f});
    sampleShader->set_uniform("Light.La", Vec3f{0.8f,0.8f,0.8f});
    sampleShader->set_uniform("Light.Position", Pt4f{camera->view().multiply_point(mobileLightPos1.conv<double>()).conv<float>()});

    materialUBO.bind(0);

    draw_nb(sampleShader.get(),drawer);
}

auto Ch5SceneMutliTexture::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch5/scene-multi-textures").lock()) == nullptr){
        return false;
    }

    materialUBO.initialize();
    materialUBO.set_data_space_from_shader(sampleShader.get());
    brickMossD = drawersM->get_drawer("brick-moss-cube"sv).lock();

    return true;
}

auto Ch5SceneMutliTexture::draw(tool::gl::BaseDrawer *) -> void{

    parent_draw();

    sampleShader->use();
    sampleShader->set_uniform("Light.L",  Vec3f{0.8f,0.8f,0.8f});
    sampleShader->set_uniform("Light.La", Vec3f{0.8f,0.8f,0.8f});
    sampleShader->set_uniform("Light.Position", Pt4f{camera->view().multiply_point(mobileLightPos1.conv<double>()).conv<float>()});

    materialUBO.update(mInfo);
    materialUBO.bind(0);

    draw_nb(sampleShader.get(), brickMossD.get());
}

auto Ch5NormalMap::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch5/normal-map").lock()) == nullptr){
        return false;
    }

    materialUBO.initialize();
    materialUBO.set_data_space_from_shader(sampleShader.get());

    return true;
}

auto Ch5NormalMap::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    sampleShader->use();
    sampleShader->set_uniform("animate", nbAnimations > 0);
    sampleShader->set_uniform("Light.L",  Vec3f{0.8f,0.8f,0.8f});
    sampleShader->set_uniform("Light.La", Vec3f{0.8f,0.8f,0.8f});
    sampleShader->set_uniform("Light.Position", Pt4f{camera->view().multiply_point(mobileLightPos1.conv<double>()).conv<float>()});

    materialUBO.update(mInfo);
    materialUBO.bind(0);

    draw_nb(sampleShader.get(),drawer);
}

auto Ch5ParallaxMapping::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch5/parallax-mapping").lock()) == nullptr){
        return false;
    }

    if((mutlitTextPlaneD = drawersM->get_drawer("multi-tex-plane"sv).lock()) == nullptr){
        return false;
    }

    materialUBO.initialize();
    materialUBO.set_data_space_from_shader(sampleShader.get());

    return true;
}

auto Ch5ParallaxMapping::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    sampleShader->use();
    sampleShader->set_uniform("showHeightMap", showHeightMap);
    sampleShader->set_uniform("bumpFactor", bumpFactor);
    sampleShader->set_uniform("Light.L",  Vec3f{0.8f,0.8f,0.8f});
    sampleShader->set_uniform("Light.La", Vec3f{0.8f,0.8f,0.8f});
    sampleShader->set_uniform("Light.Position", Pt4f{camera->view().multiply_point(mobileLightPos1.conv<double>()).conv<float>()});

    materialUBO.update(mInfo);
    materialUBO.bind(0);

    draw_nb(sampleShader.get(), mutlitTextPlaneD.get());
}

auto Ch5ParallaxMapping::update_imgui() -> void{
    ImGui::Checkbox("Show heightmap", &showHeightMap);
    ImGui::SliderFloat("Bump factor", &bumpFactor, 0.0f, 1.f, "ratio = %.3f");
}

auto Ch5SteepParallaxMapping::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch5/steep-parallax-mapping").lock()) == nullptr){
        return false;
    }

    if((mutlitTextPlaneD = drawersM->get_drawer("multi-tex-plane"sv).lock()) == nullptr){
        return false;
    }

    materialUBO.initialize();
    materialUBO.set_data_space_from_shader(sampleShader.get());

    return true;
}

auto Ch5SteepParallaxMapping::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    sampleShader->use();
    sampleShader->set_uniform("Light.L",  Vec3f{0.8f,0.8f,0.8f});
    sampleShader->set_uniform("Light.La", Vec3f{0.8f,0.8f,0.8f});
    sampleShader->set_uniform("Light.Position", Pt4f{camera->view().multiply_point(mobileLightPos1.conv<double>()).conv<float>()});
    sampleShader->set_uniform("bumpScale", bumpScale);

    materialUBO.bind(0);
    materialUBO.update(mInfo);

    draw_nb(sampleShader.get(),mutlitTextPlaneD.get());
}

auto Ch5SteepParallaxMapping::update_imgui() -> void{
    ImGui::SliderFloat("Bumpscale", &bumpScale, 0.001f, 0.20f, "ratio = %.3f");
}

auto Ch5ReflectCubeMap::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch5/reflect-cubemap").lock()) == nullptr){
        return false;
    }

    return true;
}

auto Ch5ReflectCubeMap::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    sampleShader->use();
    sampleShader->set_uniform("WorldCameraPosition", camera->position().conv<float>());
    sampleShader->set_uniform("MaterialColor", matColor);
    sampleShader->set_uniform("ReflectFactor", reflectFactor);

    gl::TBO::bind({texturesM->cube_map_id("grace")},0);
    draw_nb(sampleShader.get(), drawer);
}

auto Ch5ReflectCubeMap::update_imgui() -> void{
    ImGui::SliderFloat("Reflect factor", &reflectFactor, 0.0f, 1.f, "ratio = %.3f");
    ImGui::SliderFloat4("Mat color", matColor.array.data(), 0.0, 1.0f, "ratio = %.2f");
}

auto Ch5RefractCubeMap::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch5/refract-cubemap").lock()) == nullptr){
        return false;
    }

    return true;
}

auto Ch5RefractCubeMap::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    sampleShader->use();
    sampleShader->set_uniform("WorldCameraPosition", camera->position().conv<float>());
    sampleShader->set_uniform("Material.Eta", rmInfo.eta);
    sampleShader->set_uniform("Material.ReflectionFactor", rmInfo.reflectionFactor);

    gl::TBO::bind({texturesM->cube_map_id("grace")},0);
    draw_nb(sampleShader.get(), drawer);
}

auto Ch5RefractCubeMap::update_imgui() -> void{
    ImGui::SliderFloat("refract Eta", &rmInfo.eta, 0.0f, 1.00f, "ratio = %.3f");
    ImGui::SliderFloat("reflect factor", &rmInfo.reflectionFactor, 0.0f, 1.00f, "ratio = %.3f");
}

auto Ch5ProjectTexture::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch5/projected-texture").lock()) == nullptr){
        return false;
    }

    materialUBO.initialize();
    materialUBO.set_data_space_from_shader(sampleShader.get());

    projOptions.wrapS = TextureWrapMode::clamp_to_border;
    projOptions.wrapT = TextureWrapMode::clamp_to_border;

    return true;
}

auto Ch5ProjectTexture::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    auto frustumM =  geo::transform(Vec3d{1,1,1},projRot.conv<double>(),projPos.conv<double>());
    commonShaders["unicolor"sv]->use();
    commonShaders["unicolor"sv]->set_uniform("unicolor", Vec3f{1.0f,0.0f,0.0f});
    commonShaders["unicolor"sv]->set_uniform_matrix("MVP",((frustumM*camera->view())*camera->projection()).conv<float>());

    auto frustum = dynamic_cast<gl::FrustumDrawerLinesDrawer*>(commonDrawers["frustum"sv].get());
    frustum->update(fov, aspectRatio, zNear, zFar);
    frustum->draw();

    auto frustumP = geo::perspective<double>(deg_2_rad(fov), aspectRatio, zNear, zFar);
    sampleShader->use();
    auto tr = geo::translate(geo::Mat4d::identity(), Vec3d{0.5,0.5,0.5});
    Mat4d bias = geo::scale(tr, Vec3d{0.5,0.5,0.5});
    sampleShader->set_uniform_matrix("ProjectorMatrix", ((inverse(frustumM*-1.0)*frustumP*bias).conv<float>()));

    camM.m = geo::transform(Vec3d{1,1,1},Vec3d{0.,0.,0},Vec3d{0.,-0.75,0.});
    update_matrices();

    sampleShader->set_uniform("Light.L",  Vec3f{0.8f,0.8f,0.8f});
    sampleShader->set_uniform("Light.La", Vec3f{0.2f,0.2f,0.2f});
    sampleShader->set_uniform("Light.Position", Pt4f{camera->view().multiply_point(mobileLightPos1.conv<double>()).conv<float>()});

    materialUBO.update(mInfo);
    materialUBO.bind(0);

    sampleShader->set_camera_matrices_uniforms(camM);

    auto tbo = texturesM->texture_tbo("flower-projected");
    tbo->set_texture_options(projOptions);
    tbo->bind(0);

    commonDrawers["notext-plane-10x10"sv]->draw();

    draw_nb(sampleShader.get(), drawer);
}

auto Ch5ProjectTexture::update_imgui() -> void{
    ImGui::SliderFloat("FOV###CH5PT-2", &fov, 5.0f, 150.00f, "ratio = %.1f");
    ImGui::SliderFloat("Aspect ratio###CH5PT-3", &aspectRatio, 0.0f, 5.00f, "ratio = %.3f");
    ImGui::DragFloat("Near###CH5PT-4", &zNear, 0.1f,  0.0f, 100.00f, "ratio = %.1f");
    ImGui::DragFloat("Far###CH5PT-5", &zFar, 0.1f, 0.0f, 100.00f, "ratio = %.1f");
    ImGui::DragFloat3("Proj pos###CH5PT-6", projPos.array.data(), 0.05f, -50.0f, 50.00f, "ratio = %.2f");
    ImGui::DragFloat3("Proj rot###CH5PT-7", projRot.array.data(), 1.f, -360.0f, 360.00f, "ratio = %.2f");
}

auto Ch5DiffuseImageBasedLighting::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch5/diffuse-image-based-lighting").lock()) == nullptr){
        return false;
    }

    return true;
}

auto Ch5DiffuseImageBasedLighting::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    sampleShader->use();
    gl::TBO::bind({texturesM->cube_map_id("grace-diffuse"),texturesM->texture_id("spot_texture")},0);

    sampleShader->set_uniform("gamma",  gamma);
    sampleShader->set_uniform("CamPos", camera->position().conv<float>());

    draw_nb(sampleShader.get(), commonDrawers["notext-spot"sv].get());
}

auto Ch5DiffuseImageBasedLighting::update_imgui() -> void{
    ImGui::SliderFloat("gamma", &gamma, 0.0f, 10.00f, "ratio = %.3f");
}

auto Ch5SamplerObject::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch5/sampler-objects").lock()) == nullptr){
        return false;
    }

    // sampler objects
    options1.magFilter = TextureMagFilter::linear;
    options1.minFilter = TextureMinFilter::linear;
    sampler1.initialize();
    sampler1.update(options1);

    return true;
}

auto Ch5SamplerObject::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    sampleShader->use();
    sampleShader->set_uniform("Light.L", Vec3f{1.0f,1.0f, 1.0f});
    sampleShader->set_uniform("Light.La", lInfo.La);
    sampleShader->set_uniform("Light.Position", Pt4f{camera->view().multiply_point(mobileLightPos1.conv<double>()).conv<float>()});
    sampleShader->set_uniform("Material.Ks", mInfo.Ks);
    sampleShader->set_uniform("Material.Shininess", mInfo.Shininess);

    camM.m = geo::transform(Vec3d{0.3,0.3,0.3},Vec3d{90.,0.,0.},Vec3d{0.,0.,-2.});
    update_matrices();
    sampleShader->set_camera_matrices_uniforms(camM);

    sampler1.bind(0);
    commonDrawers["grid-floor"sv]->draw();

    // draw_nb(sampleShader.get(), drawer);

    gl::Sampler::unbind();

}

auto Ch5SamplerObject::update_imgui() -> void{

    bool updateSampler = false;
    auto &magf1 = options1.magFilter;

    int id = static_cast<int>(magf1);
    if(ImGuiUiDrawer::combo("magFilter###CH5SO-2", &id, magFiltersStr)){
        magf1 = static_cast<TextureMagFilter>(id);
        updateSampler = true;
    }

    auto &minf1 = options1.minFilter;
    id = static_cast<int>(minf1);
    if(ImGuiUiDrawer::combo("minFilter###CH5SO-3", &id, minFiltersStr)){
        minf1 = static_cast<TextureMinFilter>(id);
        updateSampler = true;
    }

    auto &wrapR1 = options1.wrapR;
    id = static_cast<int>(wrapR1);
    if(ImGuiUiDrawer::combo("wrapR###CH5SO-4", &id, wrapModeStr)){
        wrapR1 = static_cast<TextureWrapMode>(id);
        updateSampler = true;
    }

    auto &wrapS1 = options1.wrapS;
    id = static_cast<int>(wrapS1);
    if(ImGuiUiDrawer::combo("wrapS###CH5SO-5", &id, wrapModeStr)){
        wrapS1 = static_cast<TextureWrapMode>(id);
        updateSampler = true;
    }

    auto &wrapT1 = options1.wrapT;
    id = static_cast<int>(wrapT1);
    if(ImGuiUiDrawer::combo("wrapT###CH5SO-6", &id, wrapModeStr)){
        wrapT1 = static_cast<TextureWrapMode>(id);
        updateSampler = true;
    }

    ImGuiColorEditFlags miscFlags = ImGuiColorEditFlags_None;
    if(ImGui::ColorEdit4("borderColor###CH5SO-7", options1.borderColor.array.data(), miscFlags)){
        updateSampler = true;
    }

    if(updateSampler){
        sampler1.update(options1);
    }
}


auto Ch5RenderToTexture::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch5/render-to-texture").lock()) == nullptr){
        return false;
    }

    // Create the texture object
    renderTexCh5RenderToTexture.init_render(512,512);

    TextureOptions options;
    options.minFilter = TextureMinFilter::linear;
    options.magFilter = TextureMagFilter::linear;
    renderTexCh5RenderToTexture.set_texture_options(options);

    update_screen_size();

    return true;
}

auto Ch5RenderToTexture::update_screen_size() -> void {

    // Generate and bind the framebuffer
    fboCh5RenderToTexture.initialize();
    fboCh5RenderToTexture.bind();

    // Create the depth buffer
    depthBufCh5RenterToTexture.initialize();
    depthBufCh5RenterToTexture.bind();
    depthBufCh5RenterToTexture.set_data_storage();

    // Bind the texture to the FBO
    fboCh5RenderToTexture.attach_color0_texture(renderTexCh5RenderToTexture);

    // Bind the depth buffer to the FBO
    fboCh5RenderToTexture.attach_depth_buffer(depthBufCh5RenterToTexture);

    // set colors buffers to be drawn
    fboCh5RenderToTexture.set_draw_buffers({
        FrameBuffer::Color0
    });
    fboCh5RenderToTexture.check_validity();

    // Unbind the framebuffer, and revert to default framebuffer
    fboCh5RenderToTexture.unbind();
}

auto Ch5RenderToTexture::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    sampleShader->use();

    // pass 0
    fboCh5RenderToTexture.bind();
    glViewport(0,0,512,512);
    GL::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    CameraMatrices rMat;
    rMat.update_mvp(
        geo::transform(Vec3d{projScale,projScale,projScale}, projModelRot.conv<double>(), projModelPos.conv<double>()),
        geo::look_at(Vec3d{0.0,0.,-2.}, Vec3d{0.0,0.0,1.0}, Vec3d{0.0,1.0,0.0}),
        geo::perspective(deg_2_rad(60.0), 1.0, 0.3, 1000.0)
    );

    sampleShader->set_uniform("Light.L", Vec3f{1.0f,1.0f,1.0f});
    sampleShader->set_uniform("Light.La", lInfo.La);
    sampleShader->set_uniform("Light.Position", lInfo.Position);
    sampleShader->set_uniform("Material.Ks", mInfo.Ks);
    sampleShader->set_uniform("Material.Shininess", mInfo.Shininess);
    sampleShader->set_camera_matrices_uniforms(rMat);
    commonDrawers["spot"sv]->draw();

    glFlush();

    // pass 1
    gl::FBO::unbind();
    renderTexCh5RenderToTexture.bind(0);

    glViewport(0,0, camera->screen()->width(), camera->screen()->height());

    sampleShader->set_uniform("Light.Position", lInfo.Position);
    sampleShader->set_uniform("Material.Ks", mInfo.Ks);
    sampleShader->set_uniform("Material.Shininess", mInfo.Shininess);

    draw_nb(sampleShader.get(), commonDrawers["cube"sv].get());
}

auto Ch5RenderToTexture::update(float elapsedSeconds) -> void {
    Sample::update(elapsedSeconds);
    angle = (elapsedSeconds * 10.);
}

auto Ch5RenderToTexture::update_imgui() -> void{

    ImGui::Text("########### Projected texture model:");
    ImGui::Text("### Coords:");
    ImGui::SliderFloat3("Position###CH5RT-1", projModelPos.array.data(), -10.f, 10.f, "ratio = %.2f");
    ImGui::SliderFloat3("Rotation###CH5RT-2", projModelRot.array.data(), -360.f, 360.f, "ratio = %.1f");
    ImGui::SliderFloat("Scale###CH5RT-3", &projScale, 0.01f, 5.f, "ratio = %.2f");
}

auto Ch6EdgeDetectionFilter::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch6/edge-detection-filter").lock()) == nullptr){
        return false;
    }

    lightUBO.initialize();
    lightUBO.set_data_space_from_shader(sampleShader.get());
    materialUBO.initialize();
    materialUBO.set_data_space_from_shader(sampleShader.get());
    update_screen_size();

    return true;
}

auto Ch6EdgeDetectionFilter::update_screen_size() -> void{

    // Generate and bind the framebuffer
    screenFBO.clean();
    screenFBO.initialize();
    screenFBO.bind();

    // Create the texture object
    screenRenderTexture.clean();
    screenRenderTexture.init_render(camera->screen()->width(),camera->screen()->height());

    TextureOptions options;
    options.minFilter = TextureMinFilter::nearest;
    options.magFilter = TextureMagFilter::nearest;
    options.maxLevel = 0;
    screenRenderTexture.set_texture_options(options);

    // Create the depth buffer
    screenDepthBuffer.clean();
    screenDepthBuffer.initialize();
    screenDepthBuffer.bind();
    screenDepthBuffer.set_data_storage(camera->screen()->width(),camera->screen()->height());

    // Bind the texture to the FBO
    screenFBO.attach_color0_texture(screenRenderTexture);

    // Bind the depth buffer to the FBO
    screenFBO.attach_depth_buffer(screenDepthBuffer);

    // set colors buffers to be drawn
    screenFBO.set_draw_buffers({
        FrameBuffer::Color0
    });
    screenFBO.check_validity();

    // Unbind the framebuffer, and revert to default framebuffer
    screenFBO.unbind();
}

auto Ch6EdgeDetectionFilter::draw(tool::gl::BaseDrawer *drawer) -> void {

    // pass 1 : blinnphong
    if(enable){
        screenFBO.bind();
        GL::enable(GL_DEPTH_TEST);
        GL::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    parent_draw(drawer);
    sampleShader->use();
    sampleShader->set_uniform("Pass", 1);
    sampleShader->set_uniform("EdgeThreshold", edgeThreshold);

    lInfo.Position = camera->view().multiply_point(mobileLightPos1.conv<double>()).conv<float>();

    lightUBO.update(lInfo);
    lightUBO.bind(0);

    materialUBO.update(mInfo);
    materialUBO.bind(1);

    draw_nb(sampleShader.get(), drawer);

    if(enable){

        // pass 2
        glFlush();

        gl::FBO::unbind();
        screenRenderTexture.bind(0);

        GL::disable(GL_DEPTH_TEST);
        GL::clear(GL_COLOR_BUFFER_BIT);

        sampleShader->set_uniform("Pass", 2);
        draw_screen_quad(sampleShader.get());
    }
}

auto Ch6EdgeDetectionFilter::update_imgui() -> void{

    ImGui::Checkbox("enable###CH6EDF-1", &enable);
    ImGui::SliderFloat("edge threshold###CH6EDF-2", &edgeThreshold, 0.005f, 0.30f, "ratio = %.3f");
}

auto Ch6GaussianFilter::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch6/gaussian-filter").lock()) == nullptr){
        return false;
    }

    // gaussian weights
    weights.resize(5);
    float sum = 0.f;

    // Compute and sum the weights
    weights[0] = gauss(0,sigma2);
    sum = weights[0];
    for( size_t ii = 1; ii < weights.size(); ii++ ) {
        weights[ii] = gauss(static_cast<float>(ii), sigma2);
        sum += 2 * weights[ii];
    }

    for(size_t ii = 0; ii < weights.size(); ii++ ) {
        weights[ii] /= sum;
    }


    lightUBO.initialize();
    lightUBO.set_data_space_from_shader(sampleShader.get());
    materialUBO.initialize();
    materialUBO.set_data_space_from_shader(sampleShader.get());

    update_screen_size();

    return true;
}

auto Ch6GaussianFilter::update_screen_size() -> void {

    {
        // Generate and bind the framebuffer
        screenFBO.clean();
        screenFBO.initialize();
        screenFBO.bind();

        // Create the texture object
        screenRenderTexture.clean();
        screenRenderTexture.init_render(camera->screen()->width(),camera->screen()->height());

        TextureOptions options;
        options.minFilter = TextureMinFilter::nearest;
        options.magFilter = TextureMagFilter::nearest;
        options.maxLevel = 0;
        screenRenderTexture.set_texture_options(options);

        // Create the depth buffer
        screenDepthBuffer.clean();
        screenDepthBuffer.initialize();
        screenDepthBuffer.bind();
        screenDepthBuffer.set_data_storage(camera->screen()->width(),camera->screen()->height());

        // Bind the texture to the FBO
        screenFBO.attach_color0_texture(screenRenderTexture);

        // Bind the depth buffer to the FBO
        screenFBO.attach_depth_buffer(screenDepthBuffer);

        // set colors buffers to be drawn
        screenFBO.set_draw_buffers({
            FrameBuffer::Color0
        });
        screenFBO.check_validity();
    }

    {
        // Generate and bind the framebuffer
        intermediateFBO.clean();
        intermediateFBO.initialize();
        intermediateFBO.bind();

        // Create the texture object
        intermediateRenderTexture.clean();
        intermediateRenderTexture.init_render(camera->screen()->width(),camera->screen()->height());

        TextureOptions options;
        options.minFilter = TextureMinFilter::nearest;
        options.magFilter = TextureMagFilter::nearest;
        options.maxLevel = 0;
        intermediateRenderTexture.set_texture_options(options);

        // Bind the texture to the FBO
        intermediateFBO.attach_color0_texture(intermediateRenderTexture);

        // set colors buffers to be drawn
        intermediateFBO.set_draw_buffers({
            FrameBuffer::Color0
        });
        intermediateFBO.check_validity();
    }

    // Unbind the framebuffer, and revert to default framebuffer
    gl::FBO::unbind();
}

auto Ch6GaussianFilter::draw(tool::gl::BaseDrawer *drawer) -> void {


    // pass 1 : blinnPhong
    if(enable){
        screenFBO.bind();
        GL::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GL::enable(GL_DEPTH_TEST);
    }

    parent_draw(drawer);

    sampleShader->use();
    sampleShader->set_uniform("Weight[0]", weights);
    sampleShader->set_uniform("Pass", 1);

    lInfo.Position = camera->view().multiply_point(mobileLightPos1.conv<double>()).conv<float>();

    lightUBO.update(lInfo);
    lightUBO.bind(0);

    materialUBO.update(mInfo);
    materialUBO.bind(1);

    draw_nb(sampleShader.get(), drawer);

    if(enable){

        // pass 2
        intermediateFBO.bind();
        screenRenderTexture.bind(0);

        GL::disable(GL_DEPTH_TEST);
        GL::clear(GL_COLOR_BUFFER_BIT);

        sampleShader->set_uniform("Pass", 2);
        draw_screen_quad(sampleShader.get());

        // pass 3
        gl::FBO::unbind();
        intermediateRenderTexture.bind(0);

        GL::clear(GL_COLOR_BUFFER_BIT);

        sampleShader->set_uniform("Pass", 3);
        draw_screen_quad(sampleShader.get());
    }
}

auto Ch6GaussianFilter::update_imgui() -> void{

    ImGui::Checkbox("enable###CH6GF-1", &enable);
    if(ImGui::SliderFloat("Sigma2###CH6GF-2", &sigma2, 0.01f, 10.00f, "ratio = %.3f")){
        // Compute and sum the weights
        float sum = 0.f;
        weights[0] = gauss(0,sigma2);
        sum = weights[0];
        for( size_t ii = 1; ii < weights.size(); ii++ ) {
            weights[ii] = gauss(static_cast<float>(ii), sigma2);
            sum += 2 * weights[ii];
        }

        for(size_t ii = 0; ii < weights.size(); ii++ ) {
            weights[ii] /= sum;
        }
    }
}


auto Ch6HdrLightingToneMapping::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch6/hdr-lighting-tone-mapping").lock()) == nullptr){
        return false;
    }

    materialUBO.set_data_space_from_shader(sampleShader.get());

    update_screen_size();

    return true;
}

auto Ch6HdrLightingToneMapping::update_screen_size() -> void {

    int size = camera->screen()->size_pixels();
    texData.resize(size*3);

    // Generate and bind the framebuffer
    hdrFBO.clean();
    hdrFBO.initialize();
    hdrFBO.bind();

    // Create the depth buffer
    hdrDepthBuffer.clean();
    hdrDepthBuffer.initialize();
    hdrDepthBuffer.bind();
    hdrDepthBuffer.set_data_storage(camera->screen()->width(),camera->screen()->height());

    // Create the  HDR texture object
    hdrRenderTexture.clean();
    hdrRenderTexture.init_hdr_render(camera->screen()->width(),camera->screen()->height(), 4);

    TextureOptions options;
    options.minFilter = TextureMinFilter::nearest;
    options.magFilter = TextureMagFilter::nearest;
    options.maxLevel = 0;
    hdrRenderTexture.set_texture_options(options);

    // Bind the texture to the FBO
    hdrFBO.attach_color0_texture(hdrRenderTexture);

    // Bind the depth buffer to the FBO
    hdrFBO.attach_depth_buffer(hdrDepthBuffer);

    // set colors buffers to be drawn
    hdrFBO.set_draw_buffers({
        FrameBuffer::None,
        FrameBuffer::Color0
    });
    hdrFBO.check_validity();

    hdrFBO.unbind();
}

auto Ch6HdrLightingToneMapping::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    return;
    sampleShader->use();

    auto intense = Vec3f{1.0f,1.0f,1.0f};
    sampleShader->set_uniform("Lights[0].L", intense );
    sampleShader->set_uniform("Lights[1].L", intense );
    sampleShader->set_uniform("Lights[2].L", intense );

    intense = Vec3f{0.2f,0.2f,0.2f};
    sampleShader->set_uniform("Lights[0].La", lInfo.La );
    sampleShader->set_uniform("Lights[1].La", lInfo.La );
    sampleShader->set_uniform("Lights[2].La", lInfo.La );
    sampleShader->set_uniform("DoToneMap", doToneMap);
    sampleShader->set_uniform("Pass", 1);

    hdrFBO.bind();
    GL::clear_color(0.5f,0.5f,0.5f,1.0f);
    GL::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GL::enable(GL_DEPTH_TEST);


    auto lightPos = Vec4f{0.0f, 4.0f, 2.5f, 1.0f};
    lightPos.x() = -7.0f;
    sampleShader->set_uniform("Lights[0].Position", Pt4f{camera->view().multiply_point(mobileLightPos1.conv<double>()).conv<float>()});
    lightPos.x() = 0.0f;
    sampleShader->set_uniform("Lights[1].Position", Pt4f{camera->view().multiply_point(lightPos.conv<double>()).conv<float>()});
    lightPos.x() = 7.0f;
    sampleShader->set_uniform("Lights[2].Position", Pt4f{camera->view().multiply_point(lightPos.conv<double>()).conv<float>()});

    materialUBO.bind(0);
    draw_scene1(sampleShader.get());

    // compute log avg luminance
    const int size = camera->screen()->size_pixels();
    hdrRenderTexture.bind(0);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, texData.data());
    //    glGetTextureImage(hdrRenderTexture.id(), 0, GL_RGB, GL_FLOAT, static_cast<GLsizei>(texData.size()*4), texData.data());

    float sum = 0.0f;
    size_t count = 0;

    static const auto v = geo::Pt3f{0.2126f, 0.7152f, 0.0722f};
    auto d = reinterpret_cast<geo::Pt3<GLfloat>*>(texData.data());
    std::for_each(std::execution::unseq, d, d + size, [&](const geo::Pt3<GLfloat> &pt){

        if(pt.x() != 0){
            count++;
        }
        if(pt.y() != 0){
            count++;
        }
        if(pt.z() != 0){
            count++;
        }
        sum += logf( geo::dot(pt, v) + 0.00001f);
    });
    sampleShader->set_uniform( "AveLum", expf( sum / size ) );


    // pass 2
    gl::FBO::unbind();
    GL::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GL::disable(GL_DEPTH_TEST);

    sampleShader->set_uniform("Pass", 2);
    draw_screen_quad(sampleShader.get());
}

auto Ch6HdrLightingToneMapping::update_imgui() -> void{
    ImGui::Checkbox("do tone mapping", &doToneMap);
}

auto Ch6HdrBloom::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch6/hdr-bloom").lock()) == nullptr){
        return false;
    }

    materialUBO.initialize();
    materialUBO.set_data_space_from_shader(sampleShader.get());

    weights.resize(10);
    float sum = 0.f;

    // Compute and sum the weights
    weights[0] = gauss(0,sigma);
    sum = weights[0];
    for(size_t ii = 1; ii < weights.size(); ii++ ) {
        weights[ii] = gauss(float(ii), sigma);
        sum += 2 * weights[ii];
    }

    for(size_t ii = 0; ii < weights.size(); ii++ ) {
        weights[ii] /= sum;
    }

    update_screen_size();

    return true;
}

auto Ch6HdrBloom::update_screen_size() -> void {

    int size = camera->screen()->size_pixels();
    texData.resize(size*3);

    //    gl::FBO::unbind();
    {
        // Generate and bind the framebuffer
        hdrFBO.clean();
        hdrFBO.initialize();
        hdrFBO.bind();

        // Create the depth buffer
        hdrDepthBuffer.clean();
        hdrDepthBuffer.initialize();
        hdrDepthBuffer.bind();
        hdrDepthBuffer.set_data_storage(camera->screen()->width(),camera->screen()->height());

        // Create the  HDR texture object
        hdrRenderTexture.clean();
        hdrRenderTexture.init_hdr_render(camera->screen()->width(),camera->screen()->height(), 4);

        TextureOptions options;
        options.minFilter = TextureMinFilter::nearest;
        options.magFilter = TextureMagFilter::nearest;
        options.maxLevel = 0;
        hdrRenderTexture.set_texture_options(options);

        // Bind the texture to the FBO
        hdrFBO.attach_color0_texture(hdrRenderTexture);

        // Bind the depth buffer to the FBO
        hdrFBO.attach_depth_buffer(hdrDepthBuffer);

        // set colors buffers to be drawn
        hdrFBO.set_draw_buffers({FrameBuffer::Color0});
        hdrFBO.check_validity();

    }

    {
        // Generate and bind the framebuffer
        blurFBO.clean();
        blurFBO.initialize();
        blurFBO.bind();

        bloomBufWidth  = camera->screen()->width()/8;
        bloomBufHeight = camera->screen()->height()/8;

        // Create two texture objects to ping-pong for the bright-pass filter
        // and the two-pass blur
        blurTex1.clean();
        blurTex1.init_hdr_render(bloomBufWidth,bloomBufHeight, 3);

        blurTex2.clean();
        blurTex2.init_hdr_render(bloomBufWidth,bloomBufHeight, 3);

        // Bind tex1 to the FBO
        blurFBO.attach_color0_texture(blurTex1);

        //        GLenum drawBufs[] = {GL_COLOR_ATTACHMENT0};
        //        glNamedFramebufferDrawBuffers(blurFBO.id(), 1, drawBufs);
        blurFBO.set_draw_buffers({FrameBuffer::Color0});
        blurFBO.check_validity();
    }
    gl::FBO::unbind();
}

auto Ch6HdrBloom::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    gl::TBO::unbind_textures(0, 3);

    sampleShader->use();
    sampleShader->set_uniform("Weight[0]", weights);
    sampleShader->set_uniform("LumThresh", luminanceThreshold);
    sampleShader->set_uniform("Gamma", gamma);
    sampleShader->set_uniform("Exposure", exposure);
    sampleShader->set_uniform("White", white);

    auto intense = Vec3f{1.0f,1.0f,1.0f};
    sampleShader->set_uniform("Lights[0].L", intense );
    sampleShader->set_uniform("Lights[1].L", intense );
    sampleShader->set_uniform("Lights[2].L", intense );

    sampleShader->set_uniform("Lights[0].La", lInfo.La );
    sampleShader->set_uniform("Lights[1].La", lInfo.La );
    sampleShader->set_uniform("Lights[2].La", lInfo.La );

    auto lightPos = Vec4f{0.0f, 4.0f, 2.5f, 1.0f};
    lightPos.x() = -7.0f;
    sampleShader->set_uniform("Lights[0].Position", Pt4f{camera->view().multiply_point(mobileLightPos1.conv<double>()).conv<float>()});
    lightPos.x() = 0.0f;
    sampleShader->set_uniform("Lights[1].Position", Pt4f{camera->view().multiply_point(lightPos.conv<double>()).conv<float>()});
    lightPos.x() = 7.0f;
    sampleShader->set_uniform("Lights[2].Position", Pt4f{camera->view().multiply_point(lightPos.conv<double>()).conv<float>()});

    // pass 1
    sampleShader->set_uniform("Pass", 1);

    hdrFBO.bind();
    GL::clear_color(0.5f,0.5f,0.5f,1.0f);
    GL::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GL::enable(GL_DEPTH_TEST);

    materialUBO.bind(0);
    draw_scene1(sampleShader.get());

    // compute log average luminance
    const int size = camera->screen()->size_pixels();
    hdrRenderTexture.bind(0);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, texData.data());
    //    glGetTextureImage(hdrRenderTexture.id(), 0, GL_RGB, GL_FLOAT, static_cast<GLsizei>(texData.size()*4), texData.data());
    // gl::TBO::bind_textures({hdrRenderTexture.id(),0,0});
    // texData.resize(size*4);
    //     glGetTextureImage(hdrRenderTexture.id(), 0, GL_RGB, GL_FLOAT, texData.size()/4, texData.data());
    // hdrRenderTexture.get_hdr_texture_data(texData);

    float sum = 0.0f;
    size_t count = 0;

    static const auto v = geo::Pt3f{0.2126f, 0.7152f, 0.0722f};
    auto d = reinterpret_cast<geo::Pt3<GLfloat>*>(texData.data());
    std::for_each(std::execution::unseq, d, d + size/3, [&](const geo::Pt3<GLfloat> &pt){

        if(pt.x() != 0){
            count++;
        }
        if(pt.y() != 0){
            count++;
        }
        if(pt.z() != 0){
            count++;
        }
        sum += logf( geo::dot(pt, v) + 0.00001f);
    });
    sampleShader->set_uniform( "AveLum", expf( sum / size ) );


    // pass 2
    sampleShader->set_uniform("Pass", 2);

    blurFBO.bind();
    blurFBO.attach_color0_texture(blurTex1);

    glViewport(0,0,bloomBufWidth, bloomBufHeight);
    GL::disable(GL_DEPTH_TEST);
    GL::clear_color(0,0,0,0);
    GL::clear(GL_COLOR_BUFFER_BIT);

    draw_screen_quad(sampleShader.get());

    // pass 3
    sampleShader->set_uniform("Pass", 3);
    blurFBO.attach_color0_texture(blurTex2);
    draw_screen_quad(sampleShader.get());

    // pass 4
    sampleShader->set_uniform("Pass", 4);
    blurFBO.attach_color0_texture(blurTex1);
    draw_screen_quad(sampleShader.get());


    // pass 5
    sampleShader->set_uniform("Pass", 5);

    // Bind to the default framebuffer, this time we're going to
    // actually draw to the screen!
    gl::FBO::unbind();
    GL::clear(GL_COLOR_BUFFER_BIT);

    glViewport(0,0,camera->screen()->width(), camera->screen()->height());

    // In this pass, we're reading from tex1 (unit 1) and we want
    // linear sampling to get an extra blur
    linearSampler.bind(1);

    // Render the full-screen quad
    draw_screen_quad(sampleShader.get());

    // Revert to nearest sampling
    nearestSampler.bind(1);

}

auto Ch6HdrBloom::update_imgui() -> void{
    ImGui::SliderFloat("luminance threshold###CH6HB-1", &luminanceThreshold, 0.f, 5.f, "ratio = %.3f");
    ImGui::SliderFloat("exposure###CH6HB-2", &exposure, 0.f, 1.f, "ratio = %.3f");
    ImGui::SliderFloat("white###CH6HB-3", &white, 0.f, 1.f, "ratio = %.3f");
    if(ImGui::SliderFloat("sigma###CH6HB-4", &sigma, 0.f, 100.f, "ratio = %.1f")){
        float sum = 0.f;

        // Compute and sum the weights
        weights[0] = gauss(0,sigma);
        sum = weights[0];
        for(size_t ii = 1; ii < weights.size(); ii++ ) {
            weights[ii] = gauss(float(ii), sigma);
            sum += 2 * weights[ii];
        }

        for(size_t ii = 0; ii < weights.size(); ii++ ) {
            weights[ii] /= sum;
        }
    }
}


auto Ch6Deferred::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch6/deferred").lock()) == nullptr){
        return false;
    }

    const std::vector<geo::Pt3f> colors = {
        Vec3f{1.f,0.f,0.f},
        Vec3f{0.f,1.f,0.f},
        Vec3f{0.f,0.f,1.f},
        Vec3f{0.f,1.f,0.f},
        Vec3f{0.f,1.f,1.f},
        Vec3f{1.f,0.f,1.f},
        Vec3f{1.f,1.f,0.f},
        Vec3f{1.f,1.f,1.f}
    };

    for(size_t ii = 0; ii < 25; ++ii){
        lightsColors.emplace_back(colors[ii%8]);
    }

    update_screen_size();

    return true;
}

auto Ch6Deferred::update_screen_size() -> void {

    const auto width = camera->screen()->width();
    const auto height = camera->screen()->height();

    // Generate and bind the framebuffer
    deferredFBO.clean();
    deferredFBO.initialize();
    deferredFBO.bind();

    // Create the depth buffer
    depthBuf.clean();
    depthBuf.initialize();
    depthBuf.bind();
    depthBuf.set_data_storage(width, height);

    // Create the textures for position, normal and color
    posTex.clean();
    posTex.init_position(width, height);
    normTex.clean();
    normTex.init_position(width, height);
    diffuseColorTex.clean();
    diffuseColorTex.init_color(width, height);
    ambiantColorTex.clean();
    ambiantColorTex.init_color(width, height);
    specularColorTex.clean();
    specularColorTex.init_color(width, height);

    // Attach the textures to the framebuffer
    deferredFBO.attach_depth_buffer(depthBuf);
    deferredFBO.attach_color_textures({
        &posTex,&normTex,
        &diffuseColorTex,&ambiantColorTex,&specularColorTex
    });

    // set colors buffers to be drawn
    deferredFBO.set_draw_buffers({
        FrameBuffer::None,
        FrameBuffer::Color0,
        FrameBuffer::Color1,
        FrameBuffer::Color2,
        FrameBuffer::Color3,
        FrameBuffer::Color4
    });
    deferredFBO.check_validity();
    deferredFBO.unbind();
}

auto Ch6Deferred::draw(tool::gl::BaseDrawer *drawer) -> void {

    // pass 1
    deferredFBO.bind();
    GL::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GL::enable(GL_DEPTH_TEST);

    parent_draw(drawer);

    gl::TBO::unbind_textures(0, 3);
    sampleShader->use();
    sampleShader->set_uniform("Pass", 1);
    sampleShader->set_uniform("Material.Ks", mInfo.Ks);
    sampleShader->set_uniform("Material.Ka", mInfo.Ka);
    sampleShader->set_uniform("Material.Shininess", mInfo.Shininess);
    sampleShader->set_uniform("LightCount", 25);

    int count = 0;
    for(int ii = 0; ii < 5; ++ii){
        for(int jj = 0; jj < 5; ++jj){

            std::string lightName = "Light[" + std::to_string(count) + "].";
            sampleShader->set_uniform((lightName + "L").c_str(),  Vec3f{0.8f,0.8f,0.8f});
            sampleShader->set_uniform((lightName + "La").c_str(), lightsColors[count]);

            sampleShader->set_uniform("Material.Kd", lightsColors[count]);

            auto lightP = Pt4f{-10.f+ii*4,2,-10.f+jj*4, 1.f};
            sampleShader->set_uniform((lightName + "Position").c_str(), Pt4f{camera->view().multiply_point(lightP.conv<double>()).conv<float>()});
            ++count;

            camM.m = geo::transform(Vec3d{0.1,0.1,0.1},Vec3d{0.,0.,0.},lightP.xyz().conv<double>());
            update_matrices();
            sampleShader->set_camera_matrices_uniforms(camM);

            // commonDrawers["sphere"sv]->draw(sampleShader.get());
            commonDrawers["sphere"sv]->draw();
        }
    }

    // teapot
    sampleShader->set_uniform("Material.Kd", mInfo.Kd);
    sampleShader->set_uniform("Material.Ks", mInfo.Ks);
    sampleShader->set_uniform("Material.Ka", mInfo.Ka);
    sampleShader->set_uniform("Material.Shininess", mInfo.Shininess);

    for(int ii = 0; ii < 10; ++ii){
        for(int jj = 0; jj < 10; ++jj){
            update_matrices_m(geo::transform(Vec3d{0.3,0.3,0.3},Vec3d{-90.,0.,0.},Vec3d{-15.f+ii*3,0,-15.f+jj*3}));
            sampleShader->set_camera_matrices_uniforms(camM);
            // commonDrawers["teapot"sv]->draw(sampleShader.get());
            commonDrawers["torus"sv]->draw();
        }
    }

    // current
    draw_nb(sampleShader.get(), drawer);

    // pass 2
    sampleShader->set_uniform("Pass", 2);
    gl::FBO::unbind();
    gl::TBO::bind({posTex.id(), normTex.id(),
        diffuseColorTex.id(),ambiantColorTex.id(),specularColorTex.id()}
    ,0);
    GL::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GL::disable(GL_DEPTH_TEST);
    draw_screen_quad(sampleShader.get());
}

auto Ch6SSAO::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch6/ssao").lock()) == nullptr){
        return false;
    }

    std::mt19937 generator;
    std::uniform_real_distribution<float> distr01(0.0f, 1.0f);

    generator.seed(rd());

    auto uniformCircle = [&](){
        geo::Vec3f result;
        float x = distr01(generator);
        result.x() = std::cos(tool::two_PI<float> * x);
        result.y() = std::sin(tool::two_PI<float> * x);
        return result;
    };

    auto uniformHemisphere = [&](){
        geo::Vec3f result;
        float x1 = distr01(generator);
        float x2 = distr01(generator);
        float s = sqrt(1.0f - x1 * x1);
        result.x() = std::cos(tool::two_PI<float> * x2) *s;
        result.y() = std::sin(tool::two_PI<float> * x2) *s;
        result.z() = x1;
        return result;
    };

    int size = 4; // 10 -> lag
    std::vector<GLfloat> randDirections(3 * size * size);
    for (int i = 0; i < size * size; i++) {
        geo::Vec3f v = uniformCircle();
        randDirections[i * 3 + 0] = v.x();
        randDirections[i * 3 + 1] = v.y();
        randDirections[i * 3 + 2] = v.z();
        //        std::cout << i << " " << v << "\n";
    }

    gl::TextureOptions options;
    options.minFilter   = TextureMinFilter::nearest;
    options.magFilter   = TextureMagFilter::nearest;
    options.maxLevel    = 0;
    randRotationTex.load_data(randDirections.data(), size, size, 3, true, options);

    int kernSize = 64;
    kern.resize(kernSize);
    for (int i = 0; i < kernSize; i++) {
        geo::Vec3f v = uniformHemisphere();
        float scale = ((float)(i * i)) / (kernSize * kernSize);
        // v *= glm::mix(0.1f, 1.0f, scale);
        //  x * (1.0 - a) + y * a
        v *= 0.1f * (1.f - scale) + 1.0f * scale;
        kern[i] = {v.x(), v.y(), v.z()};
//        std::cout << i << " " << v << "\n";
    }

    //    std::vector<geo::Pt3f> colors;
    //    colors.resize(kernSize);
    //    std::fill(colors.begin(), colors.end(), geo::Pt3f{1,0,0});
    //    tool::io::save_cloud("./kernel.obj", kern.data(), colors.data(), kern.size());

    update_screen_size();

    return true;
}

auto Ch6SSAO::update_screen_size() -> void {

    const auto width  = camera->screen()->width();
    const auto height = camera->screen()->height();

    // Generate and bind the framebuffer
    deferredFBO.clean();
    deferredFBO.initialize();
    deferredFBO.bind();

    // Create the depth buffer
    depthBuf.clean();
    depthBuf.initialize();
    depthBuf.bind();
    depthBuf.set_data_storage(width, height);

    // Create the textures for position, normal and color
    posTex.clean();
    posTex.init_position(width, height);
    normTex.clean();
    normTex.init_position(width, height);
    colorTex.clean();
    colorTex.init_color(width, height);
    aoTex[0].clean();
    aoTex[0].init_ao(width, height);
    aoTex[1].clean();
    aoTex[1].init_ao(width, height);

    // Attach the textures to the framebuffer
    deferredFBO.attach_depth_buffer(depthBuf);
    deferredFBO.attach_color_textures({
        &posTex,
        &normTex,
        &colorTex
    });

    // set colors buffers to be drawn
    deferredFBO.set_draw_buffers({
        FrameBuffer::None,
        FrameBuffer::Color0,
        FrameBuffer::Color1,
        FrameBuffer::Color2,
        FrameBuffer::None
    });
    deferredFBO.check_validity();

    // Generate and bind the framebuffer
    ssaoFBO.clean();
    ssaoFBO.initialize();
    ssaoFBO.bind();

    // Attach the textures to the framebuffer
    ssaoFBO.attach_color_textures({
        &aoTex[0]
    });

    // set colors buffers to be drawn
    ssaoFBO.set_draw_buffers({
        FrameBuffer::None,
        FrameBuffer::None,
        FrameBuffer::None,
        FrameBuffer::None,
        FrameBuffer::Color0
    });
    ssaoFBO.check_validity();

    gl::FBO::unbind();
}

auto Ch6SSAO::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    sampleShader->use();
    sampleShader->set_uniform("doBlurPass", doBlurPass);
    sampleShader->set_uniform("randScale", geo::Vec2f{800.f/factorScale, 600.f/factorScale});
    sampleShader->set_uniform("SampleKernel[0]", kern);
    sampleShader->set_uniform_matrix("ProjectionMatrix", camera->projection().conv<float>());
    sampleShader->set_uniform("Radius", radius);

    // pass 1 : Render to G-Buffers
    deferredFBO.bind();
    sampleShader->set_uniform("Pass", 1);
    GL::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GL::enable(GL_DEPTH_TEST);

    sampleShader->set_uniform("Light.L", geo::Vec3f{0.3f, 0.3f, 0.3f});
    sampleShader->set_uniform("Light.La", lInfo.La);
    // shader->set_uniform("Light.Position", Pt4f{3.0f, 3.0f, 1.5f, 1.0f});
    sampleShader->set_uniform("Light.Position", Pt4f{camera->view().multiply_point(mobileLightPos1.conv<double>()).conv<float>()});

    // floor
    gl::TBO::bind({texturesM->texture_id("hardwood_diffuse")}, 5);
    sampleShader->set_uniform("Material.UseTex", true);

    update_matrices_m(Mat4d(true));
    sampleShader->set_camera_matrices_uniforms(camM);
    commonDrawers["notext-plane-20x10"sv]->draw();//sampleShader.get());


    // walls
    gl::TBO::bind({texturesM->texture_id("brick")}, 5);

    camM.m = geo::translate(Mat4d(true), Vec3d{0,0,-2});
    update_matrices_m(geo::rotate(camM.m, Vec3d{1,0,0},90.));
    sampleShader->set_camera_matrices_uniforms(camM);
    commonDrawers["notext-plane-20x10"sv]->draw();//sampleShader.get());

    camM.m = geo::translate(Mat4d(true), Vec3d{-2,0,0});
    camM.m = geo::rotate(camM.m, Vec3d{0,1,0},90.);
    update_matrices_m(geo::rotate(camM.m, Vec3d{1,0,0},90.));
    sampleShader->set_camera_matrices_uniforms(camM);
    commonDrawers["notext-plane-20x10"sv]->draw();//sampleShader.get());

    // dragon
    sampleShader->set_uniform("Material.UseTex", false);
    sampleShader->set_uniform("Material.Kd", geo::Vec3f{0.9f, 0.5f, 0.2f});

    camM.m = geo::rotate(Mat4d(true), Vec3d{0,1,0}, 135.);
    camM.m = geo::scale(camM.m, Vec3d{2,2,2});
    update_matrices_m(geo::translate(camM.m, Vec3d{0,0.282958,0}));
    sampleShader->set_camera_matrices_uniforms(camM);
    commonDrawers["dragon"sv]->draw();//sampleShader.get());

    // current
    gl::TBO::unbind_textures(0, 5); // TODO: change index texture in shader
    sampleShader->set_uniform("Material.UseTex", false);
    sampleShader->set_uniform("Material.Kd", mInfo.Kd);
    draw_nb(sampleShader.get(), drawer);

    // pass 2 : SSAO
    sampleShader->set_uniform("Pass", 2);
    ssaoFBO.bind();
    ssaoFBO.attach_color0_texture(aoTex[0]);
    GL::clear(GL_COLOR_BUFFER_BIT);
    GL::disable(GL_DEPTH_TEST);

    gl::TBO::bind({posTex.id(),normTex.id(),colorTex.id()/**,aoTex[0].id(),randRotationTex.id()*/},0);
    gl::TBO::bind({randRotationTex.id()}, 4);
    draw_screen_quad(sampleShader.get());

    // pass 3 : Blur
    // Read from aoTex[0], write to aoTex[1]
    sampleShader->set_uniform("Pass", 3);
    ssaoFBO.attach_color0_texture(aoTex[1]);
    GL::clear(GL_COLOR_BUFFER_BIT);
    GL::disable(GL_DEPTH_TEST);
    gl::TBO::bind({aoTex[0].id() }, 3);
    draw_screen_quad(sampleShader.get());

    // pass 4 : Lighting
    // Read from aoTex[1] (blurred)
    sampleShader->set_uniform("Pass", 4);
    gl::FBO::unbind();
    GL::clear(GL_COLOR_BUFFER_BIT);
    GL::disable(GL_DEPTH_TEST);
    gl::TBO::bind({aoTex[1].id() }, 3);
    draw_screen_quad(sampleShader.get());
}

auto Ch6SSAO::update_imgui() -> void{
    ImGui::SliderFloat("radius", &radius, 0.01f, 10.f, "ratio = %.3f");
    ImGui::SliderFloat("factor scale", &factorScale, 0.5f, 16.f, "ratio = %.3f");
    ImGui::Checkbox("do blur pass", &doBlurPass);
}

auto Ch6OIT::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch6/oit").lock()) == nullptr){
        return false;
    }


    // quad VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    // set up framebuffers and their texture attachments
    // ------------------------------------------------------------------
    glGenFramebuffers(1, &opaqueFBO);
    glGenFramebuffers(1, &transparentFBO);

    // set up attachments for opaque framebuffer
    glGenTextures(1, &opaqueTexture);
    glBindTexture(GL_TEXTURE_2D, opaqueTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1920, 1080, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1920, 1080, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, opaqueFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, opaqueTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // set up attachments for transparent framebuffer
    glGenTextures(1, &accumTexture);
    glBindTexture(GL_TEXTURE_2D, accumTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1920, 1080, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);


    // rotSpeed = tool::PI<float> / 8.0f;
    // angle = 210.f;
    // tPrev = 0.f;

    // GL::enable(GL_DEPTH_TEST);
    // glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );



    // pass1Index = glGetSubroutineIndex(sampleShader->id(), GL_FRAGMENT_SHADER, "pass1");
    // pass2Index = glGetSubroutineIndex(sampleShader->id(), GL_FRAGMENT_SHADER, "pass2");

    // update_screen_size();

    return true;
}

auto Ch6OIT::update_screen_size() -> void {

    // const auto width  = camera->screen()->width();
    // const auto height = camera->screen()->height();

    // GLuint maxNodes = 20 * width * height;
    // GLint nodeSize  = 5 * sizeof(GLfloat) + sizeof(GLuint); // The size of a linked list node

    // // Our atomic counter
    // counterBuffer.clean();
    // counterBuffer.generate();
    // counterBuffer.set_data_storage(sizeof(GLuint), GL_DYNAMIC_STORAGE_BIT);

    // // The buffer for the head pointers, as an image texture
    // headPtrTexture.clean();
    // headPtrTexture.init_image_32u(width, height, 1);

    // // The buffer of linked lists
    // linkedListBuffer.clean();
    // linkedListBuffer.generate();
    // linkedListBuffer.set_data_storage(maxNodes * nodeSize);
    // sampleShader->set_uniform("MaxNodes", maxNodes);

    // std::vector<GLuint> headPtrClearBuf(width*height, 0xffffffff);
    // clearBuffer.clean();
    // clearBuffer.generate();
    // clearBuffer.set_data_storage(static_cast<GLsizei>(headPtrClearBuf.size() * sizeof(GLuint)), headPtrClearBuf.data());

    // glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

auto Ch6OIT::draw(tool::gl::BaseDrawer*) -> void{

    // const auto width  = camera->screen()->width();
    // const auto height = camera->screen()->height();

    // headPtrTexture.bind_image(0, 0, GL_FALSE, 0, GL_READ_WRITE);
    // linkedListBuffer.bind_to_index(0);
    // counterBuffer.bind_to_index(0);

    // // clear buffers
    // {
    //     // updates a subset of a buffer object's data store
    //     GLuint zero = 0;
    //     counterBuffer.update_data(&zero, sizeof(GLuint));



    //     glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clearBuffer.id());

    //     headPtrTexture.update_image_32u(nullptr, width, height, 0, 0, false);
    // }


    // // pass 1
    // GL::enable(GL_DEPTH_TEST);
    // glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    // GL::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // GL::depth_mask( GL_FALSE );

    // GL::use_program(sampleShader->id());
    // glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &pass1Index);
    // {
    //     // draw scene
    //     sampleShader->set_uniform("LightPosition", geo::Vec4f(0,0,0,1));
    //     sampleShader->set_uniform("LightIntensity", geo::Vec3f(0.9f,0.9f,0.9f));
    //     sampleShader->set_uniform("Kd", geo::Vec4f(0.2f, 0.2f, 0.9f, 0.55f));

    //     double size = 0.45;
    //     for( int i = 0; i <= 6; i++ ){
    //         for( int j = 0; j <= 6; j++ ){
    //             for( int k = 0; k <= 6; k++ ) {
    //                 if( (i + j + k) % 2 == 0 ) {

    //                     camM.m = geo::translate(Mat4d(true), Vec3d{i-3.0, j-3.0, k-3.0});
    //                     camM.m = geo::scale(camM.m, Vec3d{size,size,size});

    //                     update_matrices();
    //                     sampleShader->set_camera_matrices_uniforms(camM);
    //                     commonDrawers["sphere"sv]->draw();
    //                 }
    //             }
    //         }
    //     }

    //     sampleShader->set_uniform("Kd", geo::Vec4f(0.9f, 0.2f, 0.2f, 0.4f));
    //     size = 2.0;
    //     double pos = 1.75;
    //     camM.m = geo::translate(Mat4d(true), Vec3d{-pos, -pos, pos});
    //     camM.m = geo::scale(camM.m, Vec3d{size,size,size});
    //     update_matrices();
    //     sampleShader->set_camera_matrices_uniforms(camM);
    //     commonDrawers["cube"sv]->draw();
    //     camM.m = geo::translate(Mat4d(true), Vec3d{-pos, -pos, -pos});
    //     camM.m = geo::scale(camM.m, Vec3d{size,size,size});
    //     update_matrices();
    //     sampleShader->set_camera_matrices_uniforms(camM);
    //     commonDrawers["cube"sv]->draw();
    //     camM.m = geo::translate(Mat4d(true), Vec3d{-pos, pos, pos});
    //     camM.m = geo::scale(camM.m, Vec3d{size,size,size});
    //     update_matrices();
    //     sampleShader->set_camera_matrices_uniforms(camM);
    //     commonDrawers["cube"sv]->draw();
    //     camM.m = geo::translate(Mat4d(true), Vec3d{-pos, pos, -pos});
    //     camM.m = geo::scale(camM.m, Vec3d{size,size,size});
    //     update_matrices();
    //     sampleShader->set_camera_matrices_uniforms(camM);
    //     commonDrawers["cube"sv]->draw();
    //     camM.m = geo::translate(Mat4d(true), Vec3d{pos, pos, pos});
    //     camM.m = geo::scale(camM.m, Vec3d{size,size,size});
    //     update_matrices();
    //     sampleShader->set_camera_matrices_uniforms(camM);
    //     commonDrawers["cube"sv]->draw();
    //     camM.m = geo::translate(Mat4d(true), Vec3d{pos, pos, -pos});
    //     camM.m = geo::scale(camM.m, Vec3d{size,size,size});
    //     update_matrices();
    //     sampleShader->set_camera_matrices_uniforms(camM);
    //     commonDrawers["cube"sv]->draw();
    //     camM.m = geo::translate(Mat4d(true), Vec3d{pos, -pos, pos});
    //     camM.m = geo::scale(camM.m, Vec3d{size,size,size});
    //     update_matrices();
    //     sampleShader->set_camera_matrices_uniforms(camM);
    //     commonDrawers["cube"sv]->draw();
    //     camM.m = geo::translate(Mat4d(true), Vec3d{pos, -pos, -pos});
    //     camM.m = geo::scale(camM.m, Vec3d{size,size,size});
    //     update_matrices();
    //     sampleShader->set_camera_matrices_uniforms(camM);
    //     commonDrawers["cube"sv]->draw();
    //     glFinish();
    // }

    // //  glFlush ensures that previous OpenGL commands must complete in finite time
    // glFlush();

    // // pass 2
    // glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
    // glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &pass2Index);
    // GL::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    // {
    //     draw_screen_quad(sampleShader.get());
    // }

    // GL::depth_mask( GL_TRUE );
}


auto Ch7BezCurve::init() -> bool {

    // if((sampleShader = shadersM->get_shader("ch7/bezcurve").lock()) == nullptr){
    //     return false;
    // }

    // std::vector<geo::Pt2f> patch ={
    //     {-1.0f, -1.0f},
    //     {-0.5f, 1.0f},
    //     {0.5f, -1.0f},
    //     {1.0f, 1.0f}
    // };

    // bezPoints = std::make_unique<gl::CloudPointsDrawer>();
    // bezPoints->init(std::span<geo::Pt2f>(patch.data(), patch.size()));
    // sampleShader->set_uniform("NumStrips", 1);
    // sampleShader->set_uniform("LineColor", geo::Vec4f(1.0f,0.0f,0.0f,1.0f));

    // commonShaders["ch7-solid"sv]->set_uniform("Color", geo::Vec4f(0.5f,1.0f,1.0f,1.0f));

    // int maxVerts;
    // glGetIntegerv(GL_MAX_PATCH_VERTICES, &maxVerts);

    return true;
}

auto Ch7BezCurve::draw(tool::gl::BaseDrawer *drawer) -> void {

    // parent_draw(drawer);

    // GL::enable(GL_DEPTH_TEST);
    // glPointSize(10.0f);

    // // Set the number of vertices per patch.  IMPORTANT!!
    // glPatchParameteri( GL_PATCH_VERTICES, 4);
    // // GL::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // camM.m = geo::Mat4d::identity();
    // update_matrices();

    // // Draw the curve
    // sampleShader->use();
    // sampleShader->set_uniform("NumSegments", numSegments);
    // sampleShader->set_camera_matrices_uniforms(camM);
    // bezPoints->draw_patches();

    // // Draw the control points
    // commonShaders["ch7-solid"sv]->use();
    // commonShaders["ch7-solid"sv]->set_camera_matrices_uniforms(camM);
    // bezPoints->draw();

    // glFinish();
}

auto Ch7BezCurve::update_imgui() -> void{
    ImGui::SliderInt("bezcurve num segments", &numSegments, 1, 200, "ratio = %.3f");
}

auto Ch7ShadeWire::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch7/shadewire").lock()) == nullptr){
        return false;
    }

    update_screen_size();

    return true;
}

auto Ch7ShadeWire::update_screen_size() -> void {

    const auto width  = camera->screen()->width();
    const auto height = camera->screen()->height();

    float w2 = width / 2.0f;
    float h2 = height / 2.0f;
    Mat4f viewport = Mat4f{
        w2,0.0f,0.0f,0.0f,
        0.0f,h2,0.0f,0.0f,
        0.0f,0.0f,1.0f,0.0f,
        w2+0, h2+0, 0.0f, 1.0f
    };
    shader->set_uniform_matrix("ViewportMatrix", viewport);
}

auto Ch7ShadeWire::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    sampleShader->use();
    sampleShader->set_uniform("Line.Width", lineWidth);
    sampleShader->set_uniform("Light.Position", mobileLightPos1);
    sampleShader->set_uniform("Line.Color", geo::Vec4f(0.05f,0.0f,0.05f,1.0f));
    sampleShader->set_uniform("Light.Intensity", geo::Vec3f(1.0f, 1.0f, 1.0f));
    sampleShader->set_uniform("Material.Kd", mInfo.Kd);
    sampleShader->set_uniform("Material.Ka", mInfo.Ka);
    sampleShader->set_uniform("Material.Ks", mInfo.Ks);
    sampleShader->set_uniform("Material.Shininess", mInfo.Shininess);

    draw_nb(sampleShader.get(), drawer);

    glFinish();
}

auto Ch7ShadeWire::update_imgui() -> void{
    ImGui::SliderFloat("shadewire line width", &lineWidth, 0.01f, 10.f, "ratio = %.3f");
}

auto Ch7ScenePointSprite::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch7/pointsprite").lock()) == nullptr){
        return false;
    }

    // pointsSprites = std::make_unique<gl::CloudPointsDrawer>();

    return true;
}

auto Ch7ScenePointSprite::draw(tool::gl::BaseDrawer *drawer) -> void {

    // parent_draw(drawer);

    // std::mt19937 e2(rd());
    // std::uniform_real_distribution<float> dist(0.f, 10000.f);

    // std::vector<geo::Pt3f> locations;
    // locations.reserve(numSprites);
    // const auto max = dist.max();
    // for(int ii = 0; ii < numSprites; ++ii){
    //     locations.emplace_back(geo::Pt3f{
    //         (dist(e2)/max * 2.f) - 1.0f,
    //         (dist(e2)/max * 2.f) - 1.0f,
    //         (dist(e2)/max * 2.f) - 1.0f
    //     });
    // }
    // pointsSprites->init(std::span<geo::Pt3f>(locations.data(), locations.size()));

    // sampleShader->use();
    // sampleShader->set_uniform("Size2", sizeSprite);
    // gl::TBO::bind({texturesM->texture_id("flower")},0);

    // sampleShader->set_uniform_matrix("ProjectionMatrix", camera->projection().conv<float>());
    // draw_nb(sampleShader.get(), nullptr);//, pointsSprites.get());
}

auto Ch7ScenePointSprite::update_imgui() -> void{
    ImGui::SliderInt("num sprites", &numSprites, 1, 1000, "ratio = %.3f");
    ImGui::SliderFloat("size sprites", &sizeSprite, 0.01f, 10.f, "ratio = %.3f");
}

auto Ch7Silhouette::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch7/silhouette").lock()) == nullptr){
        return false;
    }

    sampleShader->set_uniform("EdgeWidth", 0.015f);
    sampleShader->set_uniform("PctExtend", 0.25f);
    sampleShader->set_uniform("LineColor", geo::Pt4f(0.05f,0.0f,0.05f,1.0f));
    sampleShader->set_uniform("Material.Kd", geo::Pt3f(0.7f, 0.5f, 0.2f));
    sampleShader->set_uniform("Material.Ka", geo::Pt3f(0.2f, 0.2f, 0.2f));
    sampleShader->set_uniform("Light.Intensity", geo::Pt3f(1.0f, 1.0f, 1.0f));

    return true;
}

auto Ch7Silhouette::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    GL::enable(GL_DEPTH_TEST);

    sampleShader->use();
    sampleShader->set_uniform("Light.Position", mobileLightPos1);// geo::Pt4f(0.0f,0.0f,0.0f,1.0f));

    //    GL::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camM.m = Mat4d(true);
    update_matrices();
    sampleShader->set_camera_matrices_uniforms(camM);

    // auto ogre = drawersM->get_drawer_ptr("ogre");
    // TODO: convert mesh to agency mode
    //    drawer->draw_adjacency();

    glFinish();
}


auto Ch8ShadowMap::init() -> bool {

    // if((sampleShader = shadersM->get_shader("ch8/shadowpcf").lock()) == nullptr){
    //     return false;
    // }

    // lightFrustum = dynamic_cast<gl::Frustum*>(commonDrawers["frustum"sv]->object());

    // // Set up the framebuffer object
    // {
    //     // Generate and bind the framebuffer
    //     shadowFBO.clean();
    //     shadowFBO.generate();
    //     shadowFBO.bind();

    //     // Create the depth buffer
    //     shadowTexture.clean();
    //     shadowTexture.initialize();

    //     glTextureStorage2D (
    //         shadowTexture.id(),    // GLuint texture
    //         1,                      // GLsizei levels
    //         GL_DEPTH_COMPONENT24,   // GLenum internalformat
    //         shadowMapWidth,         // GLsizei width
    //         shadowMapHeight         // GLsizei height
    //     );

    //     TextureOptions options;
    //     options.magFilter = TextureMagFilter::nearest;
    //     options.minFilter = TextureMinFilter::nearest;
    //     options.wrapS     = TextureWrapMode::clamp_to_border;
    //     options.wrapT     = TextureWrapMode::clamp_to_border;
    //     options.borderColor = {1.0f, 0.0f,0.0f,0.0f };
    //     shadowTexture.set_texture_options(options);
    //     glTextureParameteri(shadowTexture.id(), GL_TEXTURE_COMPARE_MODE,  GL_COMPARE_REF_TO_TEXTURE);
    //     glTextureParameteri(shadowTexture.id(), GL_TEXTURE_COMPARE_FUNC, GL_LESS);

    //     shadowFBO.attach_depth_texture(shadowTexture);

    //     // set colors buffers to be drawn
    //     shadowFBO.set_draw_buffers({FrameBuffer::None});
    //     shadowFBO.check_validity();
    //     shadowFBO.unbind();
    // }

    // // ##########

    // pass1Index = glGetSubroutineIndex(sampleShader->id(), GL_FRAGMENT_SHADER, "recordDepth");
    // pass2Index = glGetSubroutineIndex(sampleShader->id(), GL_FRAGMENT_SHADER, "shadeWithShadow");

    // shadowBias = Mat4f{
    //     0.5f,0.0f,0.0f,0.0f,
    //     0.0f,0.5f,0.0f,0.0f,
    //     0.0f,0.0f,0.5f,0.0f,
    //     0.5f,0.5f,0.5f,1.0f
    // };

    // sampleShader->set_uniform_sampler_2d_shadow("ShadowMap", 0);

    return true;
}

auto Ch8ShadowMap::draw(tool::gl::BaseDrawer *drawer) -> void {

    parent_draw(drawer);

    // update frustum
    lightFrustum->update(fov, aspectRatio, 1.0f, 25.0f);
    // lightFrustum->set_perspective(fov, aspectRatio, 1.0f, 25.0f);
//    lightFrustum->orient( lightPos, {0,0,0}, Vec3f{0.0f,1.0f,0.0f});
    lightFrustumV =  geo::transform(Vec3d{1,1,1},lightRot.conv<double>(),lightPos.conv<double>());
    // lightFrustumP = lightFrustum->projection_matrix().conv<double>();
//    lightFrustumP  = Mat4d::Ortho(-10.0, 10.0, -10.0, 10.0, 1., 10.);
    lightPV = inverse(lightFrustumV.conv<float>()) * lightFrustumP.conv<float>() * shadowBias;
//    lightPV = lightFrustumV.conv<float>() * lightFrustumP.conv<float>() * shadowBias;

    sampleShader->use();
    sampleShader->set_uniform_matrix("ProjectorMatrix", lightPV.conv<float>());


    // Pass 1 (shadow map generation)
    // # fbo
    shadowFBO.bind();
    // # textures
    gl::TBO::bind({shadowTexture.id()},0);
//     # clean
    GL::clear(GL_DEPTH_BUFFER_BIT);
    // # flags
    GL::enable(GL_DEPTH_TEST);
    GL::enable(GL_CULL_FACE);
    GL::enable(GL_POLYGON_OFFSET_FILL); // set the scale and units used to calculate depth values
    glCullFace(GL_FRONT); // specify whether front- or back-facing facets can be culled
    glPolygonOffset(2.5f,10.0f);
    // # viewport
    glViewport(0,0,shadowMapWidth,shadowMapHeight);
    // # uniforms
    glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &pass1Index);
    viewP = inverse(lightFrustumV.conv<float>());
    projP = lightFrustumP.conv<float>();
    sampleShader->set_uniform("Light.Intensity", lInfo.Ld);
    sampleShader->set_uniform("Light.Ambiant", lInfo.La);
    // # draw
    draw_scene();
    // # restore flags
    glCullFace(GL_BACK);
    // # wait
    glFlush();

    //  spitOutDepthBuffer(); // This is just used to get an image of the depth buffer

    // Pass 2 (render)
    // # fbo
    gl::FBO::unbind();
    // # clean
    GL::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // # viewport
    glViewport(0,0,camera->screen()->width(), camera->screen()->height());
    // # uniforms
    glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &pass2Index);
    viewP = camera->view().conv<float>();
    projP = camera->projection().conv<float>();
    sampleShader->set_uniform("Light.Position", camera->view().conv<float>().multiply_point(to_pt4(lightPos,1.0f)));
    // # draw
    draw_scene();

    // Draw the light's frustum
    // # shader
    commonShaders["ch8-solid"sv]->use();
    // # uniforms
    commonShaders["ch8-solid"sv]->set_uniform("Color", Vec4f{1.0f,0.0f,0.0f,1.0f});
    commonShaders["ch8-solid"sv]->set_uniform_matrix("MVP",((lightFrustumV*camera->view())*camera->projection()).conv<float>());
    // # draw
    commonDrawers["frustum"sv]->draw();
}


auto Ch8ShadowMap::spit_out_depth_buffer() -> void {
    int size = shadowMapWidth * shadowMapHeight;
    float * buffer = new float[size];
    unsigned char * imgBuffer = new unsigned char[size * 4];

    glGetTexImage(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT,GL_FLOAT,buffer);

    for( int i = 0; i < shadowMapHeight; i++ )
        for( int j = 0; j < shadowMapWidth; j++ )
        {
            int imgIdx = 4 * ((i*shadowMapWidth) + j);
            int bufIdx = ((shadowMapHeight - i - 1) * shadowMapWidth) + j;

            // This is just to make a more visible image.  Scale so that
            // the range (minVal, 1.0) maps to (0.0, 1.0).  This probably should
            // be tweaked for different light configurations.
            float minVal = 0.88f;
            float scale = (buffer[bufIdx] - minVal) / (1.0f - minVal);
            unsigned char val = (unsigned char)(scale * 255);
            imgBuffer[imgIdx] = val;
            imgBuffer[imgIdx+1] = val;
            imgBuffer[imgIdx+2] = val;
            imgBuffer[imgIdx+3] = 0xff;
        }

    Texture t;
    std::vector<unsigned char> data(shadowMapHeight*shadowMapWidth*4);
    std::copy(imgBuffer, imgBuffer + shadowMapHeight*shadowMapWidth*4 , data.begin());
    t.copy_2d_data(shadowMapWidth, shadowMapHeight, 4, data);
    t.write_2d_image_file_data("./depth.png");

    delete [] buffer;
    delete [] imgBuffer;

    //    exit(1);
}


auto Ch8ShadowMap::update_imgui() -> void{
    ImGui::SliderFloat("FOV###CH8SM1-1", &fov, 5.0f, 150.00f, "ratio = %.1f");
    ImGui::SliderFloat("Aspect ratio###CH8SM1-2", &aspectRatio, 0.0f, 5.00f, "ratio = %.3f");
    ImGui::DragFloat3("Light pos###CH8SM1-3", lightPos.array.data(), 0.05f, -50.0f, 50.00f, "ratio = %.2f");
    ImGui::DragFloat3("Light rot###CH8SM1-4", lightRot.array.data(), 1.f, -360.0f, 360.00f, "ratio = %.2f");
}


auto Ch8ShadowMap::draw_scene() -> void{

    Vec3f color ={0.7f,0.5f,0.3f};
    sampleShader->set_uniform("Material.Ka", color * 0.05f);
    sampleShader->set_uniform("Material.Kd", color);
    sampleShader->set_uniform("Material.Ks", Vec3f{0.9f,0.9f,0.9f});
    sampleShader->set_uniform("Material.Shininess", 150.0f);
    camM.m = geo::rotate(Mat4d(true), Vec3d{1,0,0}, -90.);
    sampleShader->set_uniform_matrix("ShadowMatrix", (camM.m.conv<float>() * lightPV));
    update_matrices_mvp(camM.m, viewP.conv<double>(), projP.conv<double>());
    sampleShader->set_camera_matrices_uniforms(camM);
    commonDrawers["teapot"sv]->draw();

    sampleShader->set_uniform("Material.Ka", color * 0.05f);
    sampleShader->set_uniform("Material.Kd", color);
    sampleShader->set_uniform("Material.Ks", Vec3f{0.9f,0.9f,0.9f});
    sampleShader->set_uniform("Material.Shininess", 150.0f);
    camM.m = geo::translate(Mat4d(true), Vec3d{0.0f,2.0f,5.0f});
    camM.m = geo::rotate(camM.m, Vec3d{1,0,0}, -45.);
    sampleShader->set_uniform_matrix("ShadowMatrix", (camM.m.conv<float>() * lightPV));
    update_matrices_mvp(camM.m, viewP.conv<double>(), projP.conv<double>());
    sampleShader->set_camera_matrices_uniforms(camM);
    commonDrawers["torus"sv]->draw();

    sampleShader->set_uniform("Material.Kd", Vec3f{0.25f, 0.25f, 0.25f});
    sampleShader->set_uniform("Material.Ks", Vec3f{0.0f, 0.0f, 0.0f});
    sampleShader->set_uniform("Material.Ka", Vec3f{0.05f, 0.05f, 0.05f});
    sampleShader->set_uniform("Material.Shininess", 1.0f);
    camM.m = Mat4d(true);
    sampleShader->set_uniform_matrix("ShadowMatrix", (camM.m.conv<float>() * lightPV));
    update_matrices_mvp(camM.m, viewP.conv<double>(), projP.conv<double>());
    sampleShader->set_camera_matrices_uniforms(camM);
    commonDrawers["notext-plane-40x40"sv]->draw();

    camM.m = geo::translate(Mat4d(true), Vec3d{-5.0f,5.0f,0.0f});
    camM.m = geo::rotate(camM.m, Vec3d{0,0,1}, -90.);
    sampleShader->set_uniform_matrix("ShadowMatrix", (camM.m.conv<float>() * lightPV));
    update_matrices_mvp(camM.m, viewP.conv<double>(), projP.conv<double>());
    sampleShader->set_camera_matrices_uniforms(camM);
    commonDrawers["notext-plane-40x40"sv]->draw();

    camM.m = geo::translate(Mat4d(true), Vec3d{0.0f,5.0f,-5.0f});
    camM.m = geo::rotate(camM.m, Vec3d{1,0,0}, 090.);
    sampleShader->set_uniform_matrix("ShadowMatrix", (camM.m.conv<float>() * lightPV));
    update_matrices_mvp(camM.m, viewP.conv<double>(), projP.conv<double>());
    sampleShader->set_camera_matrices_uniforms(camM);
    commonDrawers["notext-plane-40x40"sv]->draw();
}

auto Ch8ShadowMap2::init() -> bool {

    if((sampleShader = shadersM->get_shader("learn/3_1_1_shadow_mapping").lock()) == nullptr){
        return false;
    }

    // Set up the framebuffer object
    {
        // Generate and bind the framebuffer
        depthmapFBO.clean();
        depthmapFBO.initialize();
        depthmapFBO.bind();

        // Create the depth buffer
        depthMap.clean();
        depthMap.initialize();

        glTextureStorage2D (
            depthMap.id(),    // GLuint texture
            1,                      // GLsizei levels
            GL_DEPTH_COMPONENT24,   // GLenum internalformat
            SHADOW_WIDTH,         // GLsizei width
            SHADOW_HEIGHT         // GLsizei height
        );

        TextureOptions options;
        options.magFilter = TextureMagFilter::nearest;
        options.minFilter = TextureMinFilter::nearest;
        options.wrapS     = TextureWrapMode::clamp_to_border;
        options.wrapT     = TextureWrapMode::clamp_to_border;
        options.borderColor = {1.0f, 0.0f,0.0f,0.0f };
        depthMap.set_texture_options(options);

        // depthmapFBO.attach_depth_texture(depthMap);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap.id(), 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        // set colors buffers to be drawn
        depthmapFBO.set_draw_buffers({FrameBuffer::None});
        depthmapFBO.check_validity();

        depthmapFBO.unbind();
    }


//    shader->use();

//    shader->set_uniform("diffuseTexture", gl::Sampler2D{0});
//    shader->set_uniform("shadowMap", gl::Sampler2DShadow{1});

//    shader->set_uniform("shadowMap", gl::Sampler2DShadow{(int)depthMap});
    return true;
}

auto Ch8ShadowMap2::draw(tool::gl::BaseDrawer *) -> void{

    //parent_draw(drawer);

    glm::vec3 lightPos1,lookAt1;
    lightPos1.x = lightPos.x();
    lightPos1.y = lightPos.y();
    lightPos1.z = lightPos.z();
    lookAt1.x = lookAt.x();
    lookAt1.y = lookAt.y();
    lookAt1.z = lookAt.z();

    //    auto lightProjection  = Mat4f::Ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    //    auto lightView        = Mat4f::LookAt(mobileLightPos1.xyz(), {0.0f, 0.0f,  0.0f}, Pt3f{0.0, 1.0, 0.0});
    auto lightProjection = from_glm(glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane));
    auto lightView       = from_glm(glm::lookAt(lightPos1, lookAt1, glm::vec3(0.0, 1.0, 0.0)));
    auto lightSpaceMatrix = lightView * lightProjection;


    // 1. first render to depth map
    commonShaders["shadow_mapping_depth"sv]->use();
    commonShaders["shadow_mapping_depth"sv]->set_uniform_matrix("lightSpaceMatrix", lightSpaceMatrix);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    depthmapFBO.bind();
    GL::clear(GL_DEPTH_BUFFER_BIT);
    gl::TBO::bind({texturesM->texture_tbo("hardwood_diffuse")->id()},0);
    render_scene(commonShaders["shadow_mapping_depth"sv].get());

    // 2. then render scene as normal with shadow mapping (using depth map)
    sampleShader->use();
    sampleShader->set_uniform_matrix("projection", camera->projection().conv<float>());
    sampleShader->set_uniform_matrix("view", camera->view().conv<float>());
    sampleShader->set_uniform("viewPos", camera->position().conv<float>());
    sampleShader->set_uniform("lightPos", lightPos);//camera->view().conv<float>().multiply_point(Pt4f(lightPos,1.0f)).xyz());
    sampleShader->set_uniform_matrix("lightSpaceMatrix", lightSpaceMatrix);
    glViewport(0, 0, camera->screen()->width(), camera->screen()->height());
    gl::FBO::unbind();
    GL::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl::TBO::bind({texturesM->texture_tbo("hardwood_diffuse")->id(), depthMap.id()},0);
    render_scene(sampleShader.get());

    commonShaders["ch8-solid"sv]->use();
    camM.m = geo::transform(Vec3d{0.3,0.3,0.3},Vec3d{0,0,0},lightPos.conv<double>());
    update_matrices();
    commonShaders["ch8-solid"sv]->set_camera_matrices_uniforms(camM);
    commonShaders["ch8-solid"sv]->set_uniform("Color", Pt4f{1,0,0,1});
    commonDrawers["sphere"sv]->draw();

    camM.m = geo::transform(Vec3d{0.3,0.3,0.3},Vec3d{0,0,0},lookAt.conv<double>());
    update_matrices();
    commonShaders["ch8-solid"sv]->set_camera_matrices_uniforms(camM);
    commonShaders["ch8-solid"sv]->set_uniform("Color", Pt4f{0,1,0,1});
    commonDrawers["sphere"sv]->draw();
}


auto Ch8ShadowMap2::render_scene(gl::ShaderProgram *shader) -> void{

    // floor
//    gl::TBO::bind_textures({texturesM->texture_tbo("brick")->id(), depthMap.id()});
    camM.m = Mat4d::identity();
    sampleShader->set_uniform_matrix("model", camM.m.conv<float>());
    commonDrawers["notext-plane-40x40"sv]->draw();

    // cubes
//    gl::TBO::bind_textures({texturesM->texture_tbo("hardwood_diffuse")->id(), depthMap.id()});
    for(int ii = 0; ii < 10; ++ii){
        for(int jj = 0; jj < 10; ++jj){
            camM.m = geo::transform(Vec3d{0.5,0.5,0.5},Vec3d{ii*45.0,0.0,0.0},Vec3d{3.0*ii, 1.0, 3.0*jj});
            sampleShader->set_uniform_matrix("model", camM.m.conv<float>());
            commonDrawers["cube"sv]->draw();
        }
    }
}

auto Ch8ShadowMap2::update_imgui() -> void{
    ImGui::SliderFloat("near_plane###CH8SM2-1", &nearPlane, 1.f, 10.4f, "ratio = %.3f");
    ImGui::SliderFloat("far_plane###CH8SM2-2", &farPlane, 3.f, 200.4f, "ratio = %.3f");
    ImGui::SliderFloat("fov###CH8SM2-3", &fov, 15.f, 360.0f, "ratio = %.3f");
    ImGui::DragFloat3("light pos###CH8SM2-4", lightPos.array.data(), 0.1f, -100.f, 100.0, "ratio = %.3f");
    ImGui::DragFloat3("look at###CH8SM2-5", lookAt.array.data(), 0.1f, -100.f, 100.0, "ratio = %.3f");

}



auto Ch8ShadowPcf::init() -> bool {

    if((sampleShader = shadersM->get_shader("ch8/shadowpcf").lock()) == nullptr){
        return false;
    }


    // Set up the framebuffer object
    {

        // Generate and bind the framebuffer
        shadowFBO.clean();
        shadowFBO.initialize();
        shadowFBO.bind();

        // Create the depth buffer
        shadowTexture.clean();
        shadowTexture.initialize();

        glTextureStorage2D (
            shadowTexture.id(),    // GLuint texture
            1,                      // GLsizei levels
            GL_DEPTH_COMPONENT24,   // GLenum internalformat
            shadowMapWidth,         // GLsizei width
            shadowMapHeight         // GLsizei height
        );


        TextureOptions options;
        options.magFilter = TextureMagFilter::linear; // linear
        options.minFilter = TextureMinFilter::linear; // linear
        options.wrapS     = TextureWrapMode::clamp_to_border;
        options.wrapT     = TextureWrapMode::clamp_to_border;
        options.borderColor = {1.0f, 0.0f,0.0f,0.0f };
        shadowTexture.set_texture_options(options);
        glTextureParameteri(shadowTexture.id(), GL_TEXTURE_COMPARE_FUNC, GL_LESS);
        glTextureParameteri(shadowTexture.id(), GL_TEXTURE_COMPARE_MODE,  GL_COMPARE_REF_TO_TEXTURE);

        shadowFBO.attach_depth_texture(shadowTexture);

        // set colors buffers to be drawn
        shadowFBO.set_draw_buffers({FrameBuffer::None});
        shadowFBO.check_validity();
        shadowFBO.unbind();
    }


    pass1Index = glGetSubroutineIndex( sampleShader->id(), GL_FRAGMENT_SHADER, "recordDepth");
    pass2Index = glGetSubroutineIndex( sampleShader->id(), GL_FRAGMENT_SHADER, "shadeWithShadow");

    shadowScale = geo::Mat4f{
        0.5f,0.0f,0.0f,0.0f,
        0.0f,0.5f,0.0f,0.0f,
        0.0f,0.0f,0.5f,0.0f,
        0.5f,0.5f,0.5f,1.0f
    };
    sampleShader->set_uniform_sampler_2d_shadow("ShadowMap", 0);

    return true;
}

auto Ch8ShadowPcf::draw(tool::gl::BaseDrawer *drawer) -> void {

    // parent_draw(drawer);

    // auto lightFrustum = dynamic_cast<gl::Frustum*>(commonDrawers["frustum"sv]->object());

    // // update frustum
    // lightFrustum->set_perspective(fov, aspectRatio, 1.0f, 25.0f);
    // auto lightFrustumV =  geo::transform(Vec3d{1,1,1},lightRot.conv<double>(),lightPos.conv<double>());
    // auto lightFrustumP = lightFrustum->projection_matrix().conv<double>();
    // lightPV = inverse(lightFrustumV.conv<float>()) * lightFrustumP.conv<float>() * shadowScale;

    // sampleShader->use();

    // // Pass 1 (shadow map generation)
    // // # fbo
    // shadowFBO.bind();
    // // # textures
    // gl::TBO::bind({shadowTexture.id()},0);
    // // # clean
    // GL::clear(GL_DEPTH_BUFFER_BIT);
    // // # flags
    // GL::enable(GL_DEPTH_TEST);
    // GL::enable(GL_CULL_FACE);
    // GL::enable(GL_POLYGON_OFFSET_FILL);
    // glCullFace(GL_FRONT);
    // glPolygonOffset(2.5f,10.0f);
    // // # viewport
    // glViewport(0,0,shadowMapWidth,shadowMapHeight);
    // // # uniforms
    // glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &pass1Index);
    // viewP = inverse(lightFrustumV.conv<float>());
    // projP = lightFrustumP.conv<float>();
    // sampleShader->set_uniform("Light.Intensity", lInfo.Ld);
    // sampleShader->set_uniform("Material.Kd", mInfo.Kd);
    // sampleShader->set_uniform("Material.Ks", mInfo.Ks);
    // sampleShader->set_uniform("Material.Ka", mInfo.Ka);
    // sampleShader->set_uniform("Material.Shininess", mInfo.Shininess);

    // camM.m = Mat4d(true);
    // sampleShader->set_uniform_matrix("ShadowMatrix", (camM.m.conv<float>() * lightPV));
    // update_matrices_mvp(camM.m, viewP.conv<double>(), projP.conv<double>());
    // sampleShader->set_camera_matrices_uniforms(camM);
    // commonDrawers["notext-plane-40x40"sv]->draw();
    // //commonDrawers["building"sv]->draw(sampleShader.get());

    // glCullFace(GL_BACK);
    // GL::disable(GL_POLYGON_OFFSET_FILL);
    // glFlush();

    // auto lp = geo::to_pt4(lightPos,1.f);
    // sampleShader->set_uniform("Light.Position", Pt4f{camera->view().multiply_point(lp.conv<double>()).conv<float>()});

    // gl::FBO::unbind();
    // glViewport(0,0,camera->screen()->width(),camera->screen()->height());
    // glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &pass2Index);

    // update_matrices_mvp(camM.m, camera->view(), camera->projection());
    // sampleShader->set_camera_matrices_uniforms(camM);
    // commonDrawers["notext-plane-40x40"sv]->draw();
    // //commonDrawers["building"sv]->draw(sampleShader.get());

    // commonShaders["ch8-solid"sv]->use();
    // camM.m = geo::transform({{0.3,0.3,0.3}},{{0,0,0}},lightPos.conv<double>());
    // update_matrices();
    // commonShaders["ch8-solid"sv]->set_camera_matrices_uniforms(camM);
    // commonShaders["ch8-solid"sv]->set_uniform("Color", Pt4f{1,0,0,1});
    // commonDrawers["sphere"sv]->draw();

    // // Draw the light's frustum
    // // # shader
    // commonShaders["ch8-solid"sv]->use();
    // // # uniforms
    // commonShaders["ch8-solid"sv]->set_uniform("Color", Vec4f{1.0f,0.0f,0.0f,1.0f});
    // commonShaders["ch8-solid"sv]->set_uniform_matrix("MVP",((lightFrustumV*camera->view())*camera->projection()).conv<float>());
    // // # draw
    // commonDrawers["frustum"sv]->draw();

    // glFinish();
}

auto Ch8ShadowPcf::update_imgui() -> void{

//    ImGui::SliderFloat("FOV###CH8SMP-1", &fov, 5.0f, 150.00f, "ratio = %.1f");
//    ImGui::SliderFloat("Aspect ratio###CH8SMP-2", &aspectRatio, 0.0f, 5.00f, "ratio = %.3f");
    ImGui::DragFloat3("Light pos###CH8SMP-3", lightPos.array.data(), 0.05f, -50.0f, 50.00f, "ratio = %.2f");
    ImGui::DragFloat3("Light rot###CH8SMP-4", lightRot.array.data(), 1.f, -360.0f, 360.00f, "ratio = %.2f");
}

// Function to generate a random float between min and max
float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(max-min)));
}

auto CloudSample::init() -> bool{

    if((sampleShader = shadersM->get_shader("colored-cloud").lock()) == nullptr){
        return false;
    }

    return true;
}

auto CloudSample::draw(gl::BaseDrawer *drawer) -> void{

    parent_draw(drawer);

    sampleShader->use();    
    sampleShader->set_uniform_matrix("view",       camera->view().conv<float>());
    sampleShader->set_uniform_matrix("projection", camera->projection().conv<float>());
    sampleShader->set_uniform_matrix("model",      geo::transform(Vec3f{scale,scale,scale}, modelRot, modelPos).conv<float>());
    sampleShader->set_uniform("enable_unicolor", enableUnicolor);
    sampleShader->set_uniform("factor_unicolor", factorUnicolor);
    sampleShader->set_uniform("size_pt", pointSize);
    // sampleShader->set_uniform("camera_position",camera->position().conv<float>());
    commonDrawers["cloud"sv]->draw();
    glPointSize(pointSize);
}

auto CloudSample::update_imgui() -> void{
    ImGui::Checkbox("enable unicolor:", &enableUnicolor);
    ImGui::SliderFloat("factor unicolor:", &factorUnicolor, 0.f, 1.f, "r = %.3f");
    ImGui::SliderFloat("point size:", &pointSize, 0.1f, 10.f, "r = %.3f");
}
