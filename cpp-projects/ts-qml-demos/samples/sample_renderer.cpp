

#include "sample_renderer.hpp"

// Qt
#include <QFile>

// qt
#include "qt_logger.hpp"

// local
#include "items/sample_3d_item_viewer.hpp"

using namespace tool;
using namespace tool::gl;
using namespace tool::geo;
// using namespace tool::cam;
using namespace Qt::Literals::StringLiterals;


SampleRenderer::SampleRenderer() : cSreen(0,0){

}

SampleRenderer::~SampleRenderer(){
    // solidShaderP.clean();
    // cloudShaderP.clean();

    if(currentSample){
        currentSample->clean();
    }
}

auto SampleRenderer::initialize_gl() -> void{

    // init glew
    GL::init_glew();
    GL::display_glew_info();

    // load shaders
    // ...


    // shaders
    QString solidVsStr;
    QString solidFsStr;
    // QString cloudVsStr;
    // QString cloudFsStr;


    // fill managers
    // ...



    {
        QFile file(":/shader/lines.vert");
        file.open(QIODevice::ReadOnly);
        solidVsStr = file.readAll();
    }
    {
        QFile file(":/shader/lines.frag");
        file.open(QIODevice::ReadOnly);
        solidFsStr = file.readAll();
    }
    // {
    //     QFile file(":/shader/cloud.vs");
    //     file.open(QIODevice::ReadOnly);
    //     cloudVsStr = file.readAll();
    // }
    // {
    //     QFile file(":/shader/cloud.fs");
    //     file.open(QIODevice::ReadOnly);
    //     cloudFsStr = file.readAll();
    // }

    QtLog::log(u"Load shaders"_s);

    std::array solidShaderSource = {
        std::make_tuple(ShaderType::vertex,   solidVsStr.toStdString()),
        std::make_tuple(ShaderType::fragment, solidFsStr.toStdString())
    };
    if(!solidShaderP.load_from_source_code(solidShaderSource)){
        QtLog::error(u"Cannot load solid shader."_s);
    }

    // std::array cloudShaderSource = {
    //     std::make_tuple(ShaderType::vertex,   cloudVsStr.toStdString()),
    //     std::make_tuple(ShaderType::fragment, cloudFsStr.toStdString())
    // };
    // if(!cloudShaderP.load_from_source_code(cloudShaderSource)){
    //     QtLog::error(u"Cannot load cloud shader."_s);
    // }

    // drawers
    QtLog::log(u"Init drawers"_s);
    // frustumD.initialize(true, 120.0f,1.0f, 0.25f, 2.88f);
    gridD.initialize(0.2f, 0.2f, 1000.f, 1000.f);

    // sphereD.initialize(0.025f);

    // std::vector<Pt3f> initData(640*576);
    // cloudD.initialize(true, initData, initData, {});
    // cloudD.set_indice_count(0);

    // oobD.initialize(true, OBB3<float>());

    // lineD.initialize(true);

    m_initialized = true;
}


void SampleRenderer::synchronize(QQuickFramebufferObject *item) {

    auto sampleViewer = dynamic_cast<Sample3dItemViewer*>(item);
    cUniforms.projection  = sampleViewer->screen().projection().conv<float>();
    cUniforms.view        = sampleViewer->camera().view_matrix().conv<float>();
    cUniforms.camPosition = sampleViewer->camera().position().conv<float>();


    bool resize = false;
    if(sampleViewer->newSample){
        currentSample = sampleViewer->currentSample;
        initSample = true;
        resize = true;
        sampleViewer->newSample = false;
    }

    auto nScreen = sampleViewer->screen();
    if(cSreen.width() != nScreen.width() || cSreen.height() != nScreen.height()){
        resize = true;
    }

    // call resize if necessary
    if(resize){
        currentSample->resize(nScreen.width(), nScreen.height());
    }
}


void SampleRenderer::render() {

    if(!m_initialized){
        initialize_gl();
    }

    if(initSample && currentSample != nullptr){
        currentSample->initialize();
        initSample = false;
    }

    // enable
    GL::enable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    // GL::enable(GL_MULTISAMPLE);
    // GL::enable(GL_STENCIL_TEST);
    GL::enable(GL_DEPTH_TEST);

    // GL::enable(GL_PROGRAM_POINT_SIZE);

    // glLineWidth(4.f);

    // clear
    GL::clear_color(m_backgoundColor.x(), m_backgoundColor.y(), m_backgoundColor.z(), m_backgoundColor.w());
    GL::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    cUniforms.model = geo::Mat4f::identity();
    cUniforms.update();

    solidShaderP.use();


    solidShaderP.set_uniform_matrix("view",         cUniforms.view, true);
    solidShaderP.set_uniform_matrix("model",        geo::Mat4f::identity(), true);
    solidShaderP.set_uniform_matrix("projection",   cUniforms.projection, true);

    // draw grid
    solidShaderP.set_uniform("enable_unicolor", true);
    if(true){
        glLineWidth(1.f);
        solidShaderP.set_uniform("unicolor",geo::Pt4f{1.f,1.f,1.f,1.f});
        gridD.draw();
    }

    if(currentSample){
        currentSample->draw(cUniforms);
    }


    // // draw frustum
    // if(true){
    //     glLineWidth(5.f);
    //     solidShaderP.set_uniform("unicolor",geo::Pt4f{0.5f,0.5f,0.5f,1.f});
    //     frustumD.draw();
    // }

    // // draw filter obb
    // // if(true){
    // //     glLineWidth(2.f);
    // //     solidShaderP.set_uniform("unicolor",geo::Pt4f{0.5f,0.0f,0.0f,1.f});
    // //     oobD.draw();
    // // }


    // // draw spheres
    // solidShaderP.use();
    // solidShaderP.set_uniform("unicolor",geo::Pt4f{1.0f,0.5f,0.5f,1.f});
    // // for(size_t id = 0; id < spheresPos.size()/2; ++id){
    // //     auto p1 = spheresPos[id*2+0];
    // //     auto p2 = spheresPos[id*2+1];

    // //     auto vec = p2 - p1;
    // //     for(size_t ii = 0; ii < 10; ++ii){
    // //         auto pt = p1 + vec *(0.1f * ii);
    // //         solidShaderP.set_uniform_matrix("model",  geo::transform<float>(geo::Pt3f(1.f,1.f,1.f), geo::Pt3f{0.f,0.f,0.f}, pt), true);
    // //         sphereD.draw();
    // //     }


    // // }

    // for(size_t idR = 0; idR < rayStart.size(); ++idR){
    //     solidShaderP.set_uniform("unicolor",geo::Pt4f{1.0f,0.5f,0.5f,1.f});
    //     solidShaderP.set_uniform_matrix("model",  geo::transform<float>(geo::Pt3f(1.f,1.f,1.f), geo::Pt3f{0.f,0.f,0.f},rayStart[idR]), true);
    //     sphereD.draw();

    //     solidShaderP.set_uniform("unicolor",geo::Pt4f{0.5f,1.0f,0.5f,1.f});
    //     solidShaderP.set_uniform_matrix("model",  geo::transform<float>(geo::Pt3f(1.f,1.f,1.f), geo::Pt3f{0.f,0.f,0.f},rayEnd[idR]), true);
    //     sphereD.draw();

    //     solidShaderP.set_uniform("unicolor",geo::Pt4f{0.5f,0.5f,0.5f,1.f});
    //     solidShaderP.set_uniform_matrix("model", geo::Mat4f::identity(), true);
    //     std::array<tool::geo::Pt3f,2> pts = {rayStart[idR],rayEnd[idR]};
    //     lineD.update(pts);
    //     lineD.draw();

    // }

    // // for(auto &pos : spheresPos){
    // //     solidShaderP.set_uniform_matrix("model",  geo::transform<float>(geo::Pt3f(1.f,1.f,1.f), geo::Pt3f{0.f,0.f,0.f},(pos)*(1.f)), true);
    // //     sphereD.draw();
    // //     // for(size_t id = 0; id < 1; ++id){

    // //     // }
    // // }

    // cloudShaderP.use();
    // cloudShaderP.set_uniform_matrix("view", m_view, true);
    // cloudShaderP.set_uniform_matrix("projection", m_projection, true);
    // cloudShaderP.set_uniform_matrix("model", m_model, true);
    // cloudShaderP.set_uniform("size_pt", 5.f);
    // cloudShaderP.set_uniform("camera_position", m_camPosition);
    // cloudD.draw();

    GL::bind_framebuffer(GL_FRAMEBUFFER, 0);
}

QOpenGLFramebufferObject *SampleRenderer::createFramebufferObject(const QSize &size){
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);
    // optionally enable multisampling by doing format.setSamples(4);
    return new QOpenGLFramebufferObject(size, format);
}

