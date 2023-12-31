
#include "draw_samples_window.hpp"

// std
#include <filesystem>

// base
#include "utility/paths.hpp"

// local
#include "imgui-tb/imgui_ui_drawer.hpp"
#include "shaders_tests.hpp"
#include "utility/logger.hpp"

using namespace tool::gl;
using namespace tool::graphics;

DrawSampleWindow::DrawSampleWindow(std::string_view title, graphics::Screen screen, std::optional<sf::ContextSettings> context) :
    BaseSfmlGlWindow(title, screen, context){

    Logger::init("./", "draw_sample.html", false);
    Logger::get()->message_signal.connect([=](std::string message){
        std::cout << message;
        std::flush(std::cout);
    });
    Logger::get()->warning_signal.connect([=](std::string warning){
        std::cout << "[W] " <<  warning;
        std::flush(std::cout);
    });
    Logger::get()->error_signal.connect([=](std::string error){
        std::cerr << "[E] " << error;
        std::flush(std::cerr);
    });

}


bool DrawSampleWindow::init_shaders(){

    const std::vector<Shader::Type> VS_FS         = {Shader::Type::vertex,Shader::Type::fragment};
    const std::vector<Shader::Type> VS_FS_GS      = {Shader::Type::vertex,Shader::Type::fragment, Shader::Type::geometry};
    const std::vector<Shader::Type> VS_FS_TES_TCS = {Shader::Type::vertex,Shader::Type::fragment, Shader::Type::tess_eval, Shader::Type::tess_control};

    std::vector<std::pair<std::string, const std::vector<Shader::Type>*>> shadersNames={
        {"others/unicolor",&VS_FS},{"others/skybox",&VS_FS},{"others/screen-quad",&VS_FS}, // ch1
        {"ch2/1",&VS_FS}, // ch2
        {"ch3/diffuse",&VS_FS},{"ch3/phong",&VS_FS},{"ch3/twoside",&VS_FS},{"ch3/flat",&VS_FS},{"ch3/discard",&VS_FS}, // ch3
        {"ch4/phong-multi-lights",&VS_FS},{"ch4/phong-directional-light",&VS_FS},{"ch4/phong-per-fragment",&VS_FS},{"ch4/blinn-phong",&VS_FS}, // ch4
        {"ch4/cartoon",&VS_FS},{"ch4/pbr",&VS_FS},
        {"ch5/scene-texture",&VS_FS},{"ch5/scene-multi-textures",&VS_FS},{"ch5/discard-pixels",&VS_FS},{"ch5/normal-map",&VS_FS}, // ch5
        {"ch5/parallax-mapping",&VS_FS},{"ch5/steep-parallax-mapping",&VS_FS},{"ch5/reflect-cubemap",&VS_FS},{"ch5/refract-cubemap",&VS_FS},
        {"ch5/projected-texture",&VS_FS},{"ch5/render-to-texture",&VS_FS},{"ch5/sampler-objects",&VS_FS},{"ch5/diffuse-image-based-lighting",&VS_FS},
        {"ch6/edge-detection-filter",&VS_FS},{"ch6/gaussian-filter",&VS_FS},{"ch6/hdr-lighting-tone-mapping",&VS_FS},{"ch6/hdr-bloom",&VS_FS}, // ch6
        {"ch6/deferred",&VS_FS},{"ch6/ssao",&VS_FS},{"ch6/oit",&VS_FS},
        {"ch7/solid",&VS_FS},{"ch7/bezcurve", &VS_FS_TES_TCS}, {"ch7/shadewire", &VS_FS_GS}, {"ch7/pointsprite", &VS_FS_GS}, // ch7
        {"ch7/silhouette", &VS_FS_GS},
        {"ch8/shadowmap", &VS_FS},{"ch8/shadowpcf", &VS_FS},{"ch8/solid", &VS_FS}, // ch8
        {"learn/3_1_1_shadow_mapping",          &VS_FS}, // learnopengl
        {"learn/3_1_1_shadow_mapping_depth",    &VS_FS},
        {"learn/3_1_1_debug_quad",              &VS_FS}
    };

    // find shaders dir path
    const auto currentPath = std::filesystem::current_path();
    const auto shadersPaths = currentPath / "resources" / "shaders" / "samples";
    for(const auto &shaderName : shadersNames){

        std::vector<std::string> paths;
        paths.reserve(shaderName.second->size());
        for(const auto &shaderType : *shaderName.second){
//            paths.push_back((shadersPaths / Paths::get_shader(shaderName.first, Shader::get_ext(shaderType))).string());
            switch(shaderType){
            case Shader::Type::vertex:
                paths.push_back((shadersPaths / (shaderName.first + ".vs")).string());
                break;
            case Shader::Type::fragment:
                paths.push_back((shadersPaths / (shaderName.first + ".fs")).string());
                break;
            case Shader::Type::tess_eval:
                paths.push_back((shadersPaths / (shaderName.first + ".tes")).string());
                break;
            case Shader::Type::tess_control:
                paths.push_back((shadersPaths / (shaderName.first + ".tcs")).string());
                break;
            case Shader::Type::geometry:
                paths.push_back((shadersPaths / (shaderName.first + ".gs")).string());
                break;
            case Shader::Type::compute:
                // ..paths.emplace_back((shadersPaths / (shaderName.first + ".cs")).string());
                break;
            default:
                break;
            }
        }

        if(!Managers::shaders->load_shader(shaderName.first, paths)){
            Logger::error("Fail to load all shaders.\n");
            Managers::shaders->unbind();
            return false;
        }
        Logger::message(std::format("Shader [{}] loaded.\n", shaderName.first));
    }

    Managers::shaders->add_shader("colored-cloud", std::move(gl::ColoredCloudShader()));
    //    shadersPaths = currentPath / ".." / ".." / ".." / "cpp" / "toolset" / "resources" / "shaders";
    //    shaderInit &= colorMeshShader.init();
    //    shaderInit &= texturedMeshShader.init();
    //    shaderInit &= texturedColorMeshShader.init();
    //    shaderInit &= texturedNormalsMeshShader.init();
    Managers::shaders->unbind();

    return true;
}


bool DrawSampleWindow::init_textures(){

    // paths
    std::string path = std::filesystem::current_path().string() + "/resources/textures";

    // texture 2d
    std::vector<TextureLoadingInfo> texturesInfo= {
        {"container1",        "container.jpg"},
        {"container2",        "container2.png"},
        {"container2_spec",   "container2_specular.png"},
        {"brick",             "brick1.jpg"},
        {"wall",              "wall.jpg"},
        {"flower",            "flower.png"},
        {"smiley",            "smiley.png"},
        {"moss",              "moss.png"},
        {"fire",              "fire.png"},
        {"cement",            "cement.jpg"},
        {"ogre_diffuse",      "ogre_diffuse.png"},
        {"ogre_normalmap",    "ogre_normalmap.png"},
        {"mybrick-color",     "mybrick-color.png"},
        {"mybrick-normal",    "mybrick-normal.png"},
        {"mybrick-height",    "mybrick-height.png"},
        {"spot_texture",      "spot_texture.png"},
        {"me_textile",        "me_textile.png"},
        {"hardwood_diffuse",  "hardwood2_diffuse.jpg"},
    };

    Logger::message(std::format("# Load textures from path [{}].\n", path));
    if(!Managers::textures->load_textures_from_directory(path, texturesInfo)){
        return false;
    }

    // cubemaps
    Logger::message("# Load cubemaps\n");
    bool loaded = true;
    loaded &= Managers::textures->load_cube_map(path + "/pisa/pisa_", {"posx.png","negx.png","posy.png","negy.png","posz.png","negz.png"},  "pisa", false);
    loaded &= Managers::textures->load_cube_map(path + "/grace/grace_",{"posx.hdr","negx.hdr","posy.hdr","negy.hdr","posz.hdr","negz.hdr"}, "grace", false);
    loaded &= Managers::textures->load_cube_map(path + "/grace-diffuse/grace-diffuse_",{"posx.hdr","negx.hdr","posy.hdr","negy.hdr","posz.hdr","negz.hdr"}, "grace-diffuse", false);
    if(!loaded){
        Logger::error("Error during cubemaps loading.\n");
        return false;
    }

    // tbo
    // # texture 2d
    Logger::message("# Generate texture 2d TBO\n");
    loaded = true;
    loaded &= Managers::textures->generate_texture2d_tbo("me_textile",      "me_textile");
    loaded &= Managers::textures->generate_texture2d_tbo("cement",          "cement");
    loaded &= Managers::textures->generate_texture2d_tbo("brick",           "brick");
    loaded &= Managers::textures->generate_texture2d_tbo("moss",            "moss");
    loaded &= Managers::textures->generate_texture2d_tbo("mybrick-color",   "mybrick-color");
    loaded &= Managers::textures->generate_texture2d_tbo("mybrick-normal",  "mybrick-normal");
    loaded &= Managers::textures->generate_texture2d_tbo("mybrick-height",  "mybrick-height");
    loaded &= Managers::textures->generate_texture2d_tbo("spot_texture",    "spot_texture");
    loaded &= Managers::textures->generate_texture2d_tbo("ogre_diffuse",    "ogre_diffuse");
    loaded &= Managers::textures->generate_texture2d_tbo("ogre_normalmap",  "ogre_normalmap");
    loaded &= Managers::textures->generate_texture2d_tbo("hardwood_diffuse","hardwood_diffuse");
    loaded &= Managers::textures->generate_texture2d_tbo("flower",          "flower");

    if(!loaded){
        Logger::error("Error during texture 2d TBO generation.\n");
        return false;
    }
    // # cubemaps
    Logger::message("# Generate cubemaps TBO\n");
    loaded = true;
    loaded &= Managers::textures->generate_cubemap_tbo("grace-diffuse", "grace-diffuse");
    loaded &= Managers::textures->generate_cubemap_tbo("grace", "grace");
    if(!loaded){
        Logger::error("Error during cubemap TBO generation.\n");
        return false;
    }

    // others
    Logger::message("# Other\n");
    loaded &= Managers::textures->generate_texture2d_tbo("flower-projected",  "flower");

    return true;
}

bool DrawSampleWindow::init_models(){

    std::string mesh = Paths::resourcesDir.value() + "/meshes";
    bool loaded = true;
    {
//        Bench::start("models");

        Logger::message("Add models.\n");
        loaded &= Managers::models->add({
                {"crysis",  mesh + "/crysis-nano-suit-2/scene.fbx"},
                {"pig",     mesh + "/pig/pig_triangulated.obj"},
                {"dragon",  mesh + "/dragon/dragon.obj"},
                {"ogre",    mesh + "/ogre/bs_ears.obj"},
                {"spot",    mesh + "/spot/spot_triangulated.obj"},
                {"fox",     mesh + "/low-poly-fox/source/animations.FBX"},
                {"bob",     mesh + "/bob/boblampclean.md5mesh"},
                {"bdragon", mesh + "/bdragon/source/bdragon.fbx"},
                {"alex",    mesh + "/alex/alex_breahting_idle.fbx"},
                {"rabbit",  mesh + "/rabbit/stanford_rabbit.obj"},
                {"storm",   mesh + "/storm/source/Storm_Ani.fbx"},
                {"building",mesh + "/building.obj"},
            });

//        Bench::stop();
//        Bench::display();
    }


    if(auto dragon = Managers::models->get("dragon").lock()){
        dragon->display_hierarchy();
    }


    // loaded = loaded && modelsM.add_model("fox",     mesh + "low-poly-fox-by-pixelmannen-animated/source/animations.FBX");
    // loaded = loaded && modelsM.add_model("bob",     mesh + "bob/boblampclean.md5mesh");
    // loaded = loaded && modelsM.add_model("bdragon", mesh + "assimp/bdragon/source/bdragon.fbx");
    // loaded = loaded && modelsM.add_model("alex",    mesh + "alex/alex_breahting_idle.fbx");

    // loaded = loaded && modelsM.add_model2("lord", mesh + "assimp/lord-inquisitor-servo-skull/source/lord.fbx");
    // loaded = loaded && modelsM.add_model2("phoenix", mesh + "assimp/phoenix-bird/source/fly.fbx");
    // loaded = loaded && modelsM.add_model2("ruby", mesh + "assimp/ruby-rose/source/rubyAnimated002.fbx");
    // loaded = loaded && modelsM.add_model2("sci", mesh + "assimp/sci-fi-girl-v02-walkcycle-test/source/girl_walkcycle_test01.fbx");
    // loaded = loaded && modelsM.add_model2("wolf", mesh + "assimp/wolf-with-animations/source/wolf.fbx"); // BAD FBX
    // loaded = loaded && modelsM.add_model("bob",  mesh + "low-poly-fox-by-pixelmannen-animated/source/animations.FBX");

    return loaded;
}

bool DrawSampleWindow::init_drawers(){

    // add drawers
    auto dm = Managers::drawers;
    auto tm = Managers::textures;
    auto mm = Managers::models;

    // # procedural
    dm->add_drawer<gl::PlaneDrawer>("notext-plane-10x10",   Scale{1.f},    10.f,10.f);
    dm->add_drawer<gl::PlaneDrawer>("notext-plane-20x10",   Scale{1.f},    20.f,10.f);
    dm->add_drawer<gl::PlaneDrawer>("notext-plane-40x40",   Scale{1.f},    40.f,40.f);
    dm->add_drawer<gl::PlaneDrawer>("floor",                Scale{1.f},    8,8, TextureNames{tm->texture_id("cement")});
    dm->add_drawer<gl::PlaneDrawer>("grid-floor",           Scale{0.5f},   8,8, TextureNames{tm->texture_id("mybrick-me_textile")});
    dm->add_drawer<gl::PlaneDrawer>("multi-tex-plane",      Scale{0.5f},   8,8, TextureNames{tm->texture_id("mybrick-color"), tm->texture_id("mybrick-normal"), tm->texture_id("mybrick-height")});
    dm->add_drawer<gl::TorusDrawer>("torus",                Scale{0.4f});
    dm->add_drawer<gl::CubeDrawer>("cube",                  Scale{0.3f},   2.f);
    dm->add_drawer<gl::CubeDrawer>("brick-cube",            Scale{0.3f},   2.f, TextureNames{tm->texture_id("brick")});
    dm->add_drawer<gl::CubeDrawer>("brick-moss-cube",       Scale{0.3f},   2.f, TextureNames{tm->texture_id("brick"), tm->texture_id("moss")});
    dm->add_drawer<gl::CubeDrawer>("cement-moss-cube",      Scale{0.3f},   2.f, TextureNames{tm->texture_id("cement"), tm->texture_id("moss")});
    dm->add_drawer<gl::FullscreenQuadDrawer>("screen-quad", Scale{1.f});
    dm->add_drawer<gl::TeapotDrawer>("teapot",              Scale{0.3f});
    dm->add_drawer<gl::SphereDrawer>("sphere",              Scale{0.7f});
    dm->add_drawer<gl::AxesDrawer>("axes",                  Scale{1.f});
    dm->add_drawer<gl::FrustumDrawer>("frustum",            Scale{1.f});
    dm->add_drawer<gl::SkyboxDrawer>("skybox",              Scale{1.f}, 100.f, std::nullopt);

    // # loaded models
    dm->add_drawer<tool::gl::ModelDrawer>("spot",           Scale{1.f},    mm->get("spot"),TexturesInfo{tm->get_texture_info("spot_texture",{})});
    dm->add_drawer<tool::gl::ModelDrawer>("notext-spot",    Scale{1.f},    mm->get("spot"));
    dm->add_drawer<tool::gl::ModelDrawer>("ogre",           Scale{1.f},    mm->get("ogre"),TexturesInfo{tm->get_texture_info("ogre_diffuse",{}), tm->get_texture_info("ogre_normalmap",{})});
    dm->add_drawer<tool::gl::ModelDrawer>("pig",            Scale{1.f},    mm->get("pig"));
    dm->add_drawer<tool::gl::ModelDrawer>("dragon",         Scale{2.f},    mm->get("dragon"));
    dm->add_drawer<tool::gl::ModelDrawer>("building",       Scale{1.f},    mm->get("building"));
    dm->add_drawer<tool::gl::ModelDrawer>("crysis",         Scale{0.1f},   mm->get("crysis"));
    dm->add_drawer<tool::gl::ModelDrawer>("alex",           Scale{0.01f},  mm->get("alex"));
    dm->add_drawer<tool::gl::ModelDrawer>("rabbit",         Scale{3.f},    mm->get("rabbit"));
    dm->add_drawer<tool::gl::ModelDrawer>("storm",          Scale{0.01f},    mm->get("storm"));

//    auto storm = std::make_shared<tool::gl::ModelDrawer>(mm->get("storm"));
////    storm->update_animation(mm->get_animation_name("storm",0), 0.f);
//    dm->add_drawer("storm", storm, 0.01f);

    // ui drawers
    uiDrawers = dm->sub({
        "cube","torus","crysis","storm","alex","dragon","pig","sphere","rabbit","teapot","spot","ogre"
    });

    dynamic_cast<gl::ModelDrawer*>(uiDrawers.get_element_ptr("storm"))->model()->display_hierarchy();

    Logger::message(("Drawers loaded.\n"));
    return true;
}


bool DrawSampleWindow::init_samples(){

    // ch3
    uiSamples.add_element("ch3Diffuse", std::make_unique<Ch3Diffuse>(&m_camera));
    uiSamples.add_element("ch3TwoSide",  std::make_unique<Ch3TwoSide>(&m_camera));
    uiSamples.add_element("ch3Flat",     std::make_unique<Ch3Flat>(&m_camera));
    uiSamples.add_element("ch3Discard", std::make_unique<Ch3Discard>(&m_camera));
    uiSamples.add_element("ch3Phong",   std::make_unique<Ch3Phong>(&m_camera));
    // ch4
    uiSamples.add_element("ch4PhongDirectionnalLight",  std::make_unique<Ch4PhongDirectionnalLight>(&m_camera));
    uiSamples.add_element("ch4PhongMultiLights",  std::make_unique<Ch4PhongMultiLights>(&m_camera));
    uiSamples.add_element("ch4PhongPerFragment",  std::make_unique<Ch4PhongPerFragment>(&m_camera));
    uiSamples.add_element("ch4BlinnPhong",  std::make_unique<Ch4BlinnPhong>(&m_camera));
    uiSamples.add_element("ch4Cartoon",  std::make_unique<Ch4Cartoon>(&m_camera));
    uiSamples.add_element("ch4PBR",  std::make_unique<Ch4PBR>(&m_camera));
    // ch5
    uiSamples.add_element("ch5DiscardPixels",  std::make_unique<Ch5DiscardPixels>(&m_camera));
    uiSamples.add_element("ch5SceneTexture",  std::make_unique<Ch5SceneTexture>(&m_camera));
    uiSamples.add_element("ch5SceneMutliTexture",  std::make_unique<Ch5SceneMutliTexture>(&m_camera));
    uiSamples.add_element("ch5NormalMap",  std::make_unique<Ch5NormalMap>(&m_camera));
    uiSamples.add_element("ch5ParallaxMapping",  std::make_unique<Ch5ParallaxMapping>(&m_camera));
    uiSamples.add_element("ch5SteepParallaxMapping",  std::make_unique<Ch5SteepParallaxMapping>(&m_camera));
    uiSamples.add_element("ch5ReflectCubeMap",  std::make_unique<Ch5ReflectCubeMap>(&m_camera));
    uiSamples.add_element("ch5RefractCubeMap",  std::make_unique<Ch5RefractCubeMap>(&m_camera));
    uiSamples.add_element("ch5ProjectTexture",  std::make_unique<Ch5ProjectTexture>(&m_camera));
    uiSamples.add_element("ch5DiffuseImageBasedLighting",  std::make_unique<Ch5DiffuseImageBasedLighting>(&m_camera));
    uiSamples.add_element("ch5SamplerObject",  std::make_unique<Ch5SamplerObject>(&m_camera));
    uiSamples.add_element("ch5RenderToTexture",  std::make_unique<Ch5RenderToTexture>(&m_camera));
    uiSamples.add_element("ch6EdgeDetectionFilter",  std::make_unique<Ch6EdgeDetectionFilter>(&m_camera));
    uiSamples.add_element("ch6GaussianFilter",  std::make_unique<Ch6GaussianFilter>(&m_camera));
    uiSamples.add_element("ch6HdrLightingToneMapping",  std::make_unique<Ch6HdrLightingToneMapping>(&m_camera));
    uiSamples.add_element("ch6HdrBloom",  std::make_unique<Ch6HdrBloom>(&m_camera));
    uiSamples.add_element("ch6Deferred",  std::make_unique<Ch6Deferred>(&m_camera));
    uiSamples.add_element("ch6SSAO",  std::make_unique<Ch6SSAO>(&m_camera));
    uiSamples.add_element("ch6OIT",  std::make_unique<Ch6OIT>(&m_camera));
    uiSamples.add_element("ch7BezCurve",  std::make_unique<Ch7BezCurve>(&m_camera));
//    uiSamples.add_element("ch7ShadeWire",  std::make_unique<Ch7ShadeWire>(&m_camera));
    uiSamples.add_element("ch7ScenePointSprite",  std::make_unique<Ch7ScenePointSprite>(&m_camera));
    uiSamples.add_element("ch7Silhouette",  std::make_unique<Ch7Silhouette>(&m_camera));

    uiSamples.add_element("ch8ShadowMap",  std::make_unique<Ch8ShadowMap>(&m_camera));
    uiSamples.add_element("ch8ShadowMap2",  std::make_unique<Ch8ShadowMap2>(&m_camera));
    uiSamples.add_element("ch8ShadowPcf",  std::make_unique<Ch8ShadowPcf>(&m_camera));

    for(auto &sample : uiSamples.get_span()){
        Logger::message(std::format("Init sample {}\n", std::get<0>(sample)));
        if(!std::get<1>(sample)->parent_init()){
            Logger::error(std::format("Cannot init sample {}\n", std::get<0>(sample)));
            return false;
        }
    }

    uiSamples.set_current_id(0);

    Logger::message("End sample init.\n");
    return true;
}



bool DrawSampleWindow::initialize_gl(){

    // flags
    glEnable(GL_MULTISAMPLE); // msaa
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // implot
    imPlotContext = ImPlot::CreateContext();

    // managers
    Managers::initialize();

    // models
    Logger::message("Init models\n");
    if(!init_models()){
        return m_glInitialized = false;
    }

    // textures
    Logger::message("Init textures\n");
    if(!init_textures()){
        return m_glInitialized = false;
    }

    // shaders
    Logger::message("Init shaders\n");
    if(!init_shaders()){
        return m_glInitialized = false;
    }

    // camera
    Logger::message("Init camera\n");
    m_camera.set_position({0.,0.,5.});
    m_camera.set_direction({0.,0.,-1.},{0.,1.,0.});

    Logger::message("Init drawers\n");
    init_drawers();

    Logger::message("Init samples\n");
    if(!init_samples()){
        return m_glInitialized = false;
    }

    VAO::unbind();

    return m_glInitialized = true;
}


void DrawSampleWindow::draw_gl(){

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    // clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    gl::FBO::unbind();

    if(auto sample = uiSamples.get_current_element_ptr(); sample != nullptr){
        if(auto drawer = uiDrawers.get_current_element_ptr(); drawer != nullptr){
            sample->draw(drawer);
        }
    }
}

void DrawSampleWindow::update(){

    // update current sample
    if(auto sample = uiSamples.get_current_element_ptr(); sample != nullptr){
        sample->update(elapsed_secondes());
    }
}


void DrawSampleWindow::draw_imgui(){

    // menu bar
    if (ImGui::BeginMainMenuBar()){

        //if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
        //if (ImGui::MenuItem("Save", "Ctrl+S"))   { /* Do stuff */ }
        if (ImGui::MenuItem("Close", "Ctrl+W"))  {
            running = false;
        }
        if (ImGui::BeginMenu("Developer")){
            ImGui::MenuItem("Show demo window", nullptr, &m_showDemoWindow);
            ImGui::MenuItem("Show metrics window", nullptr, &m_showMetricsWindow);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();        
    }



    // show demo
    if (m_showDemoWindow){
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
        ImGui::ShowDemoWindow(&m_showDemoWindow);
    }

    // show metrics
    if (m_showMetricsWindow){
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
        ImGui::ShowMetricsWindow(&m_showMetricsWindow);
    }

//    ImGuiWindowFlags settingsWindowsFlags =
//        ImGuiWindowFlags_NoMove  |
//        ImGuiWindowFlags_MenuBar |
//        ImGuiWindowFlags_AlwaysAutoResize;

    ImGui::Begin(m_imguiWindowTitle.c_str()); // begin window
    ImGui::Text(
        "Frame time generation [%lld ms]",
        std::chrono::duration_cast<std::chrono::milliseconds>(frameDuration).count()
    );

    ImGui::Text("Drawer");
    ImGui::SameLine();

    if(ImGui::BeginCombo("###Drawer", uiDrawers.get_current_alias().data())){

        for(size_t id = 0; id < uiDrawers.count(); ++id){
            bool selected = id == uiDrawers.get_current_id();
            if (ImGui::Selectable(uiDrawers.get_alias(id).value().data(), selected)){
                uiDrawers.set_current_id(id);
            }
            if(selected){
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }
    ImGui::SameLine();

    if(ImGui::Button("^###d^")){
        uiDrawers.decrement_current_id();
    }
    ImGui::SameLine();

    if(ImGui::Button("v###dv")){
        uiDrawers.increment_current_id();
    }

    ImGui::Text("Sample");
    ImGui::SameLine();

    if(ImGui::BeginCombo("###Sample", uiSamples.get_current_alias().data())){

        for(size_t id = 0; id < uiSamples.count(); ++id){
            bool selected = id == uiSamples.get_current_id();
            if (ImGui::Selectable(uiSamples.get_alias(id).value().data(), selected)){
                uiSamples.set_current_id(id);
            }
            if(selected){
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();

    if(ImGui::Button("^###s^")){
        uiSamples.decrement_current_id();
    }
    ImGui::SameLine();

    if(ImGui::Button("v###sv")){
        uiSamples.increment_current_id();
    }
    ImGui::SameLine();
    ImGuiUiDrawer::text(std::to_string(uiSamples.get_current_id()));

    if(auto sample = uiSamples.get_current_element_ptr(); sample != nullptr){
        sample->parent_update_imgui();
    }

    ImGui::End();


    //    if (ImGui::CollapsingHeader("Scatter Plots")) { // crash
    //        srand(0);
    //        static float xs1[100], ys1[100];
    //        for (int i = 0; i < 100; ++i) {
    //            xs1[i] = i * 0.01f;
    //            ys1[i] = xs1[i] + 0.1f * ((float)rand() / (float)RAND_MAX);
    //        }
    //        static float xs2[50], ys2[50];
    //        for (int i = 0; i < 50; i++) {
    //            xs2[i] = 0.25f + 0.2f * ((float)rand() / (float)RAND_MAX);
    //            ys2[i] = 0.75f + 0.2f * ((float)rand() / (float)RAND_MAX);
    //        }

    //        if (ImPlot::BeginPlot("Scatter Plot", NULL, NULL)) {
    //            ImPlot::PlotScatter("Data 1", xs1, ys1, 100);
    //            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
    //            ImPlot::SetNextMarkerStyle(ImPlotMarker_Square, 6, ImVec4(0,1,0,0.5f), IMPLOT_AUTO, ImVec4(0,1,0,1));
    //            ImPlot::PlotScatter("Data 2", xs2, ys2, 50);
    //            ImPlot::PopStyleVar();
    //            ImPlot::EndPlot();
    //        }
    //    }
    //    if (ImGui::CollapsingHeader("Stairstep Plots")) {
    //        static float ys1[101], ys2[101];
    //        for (int i = 0; i < 101; ++i) {
    //            ys1[i] = 0.5f + 0.4f * sinf(50 * i * 0.01f);
    //            ys2[i] = 0.5f + 0.2f * sinf(25 * i * 0.01f);
    //        }

    //        if (ImPlot::BeginPlot("Stairstep Plot", "x", "f(x)")) {
    //            ImPlot::PlotStairs("Signal 1", ys1, 101, 0.01f);
    //            ImPlot::SetNextMarkerStyle(ImPlotMarker_Square, 2.0f);
    //            ImPlot::PlotStairs("Signal 2", ys2, 101, 0.01f);
    //            ImPlot::EndPlot();
    //        }
    //    }

}


void DrawSampleWindow::resize_windows(){
    // resize current sample
    if(auto sample = uiSamples.get_current_element_ptr(); sample != nullptr){
        sample->update_screen_size();
    }
}
