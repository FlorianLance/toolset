
#include "sample_3d_item_viewer.hpp"

// local
#include "samples/sample_renderer.hpp"

using namespace tool;
using namespace tool::geo;
using namespace Qt::Literals::StringLiterals;

QQuickFramebufferObject::Renderer *Sample3dItemViewer::createRenderer() const {
    return new SampleRenderer();
}

void Sample3dItemViewer::set_sample(tool::gl::BaseGlSample *sample){
    QtLog::message(u"SET SAMPLE");
    currentSample = sample;
    newSample = true;
}
