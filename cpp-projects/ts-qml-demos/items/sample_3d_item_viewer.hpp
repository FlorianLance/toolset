


#pragma once

// local
#include "base_3d_item_viewer.hpp"
#include "samples/base_sample.hpp"

class Sample3dItemViewer : public Base3dItemViewer{
    Q_OBJECT
public:

    Sample3dItemViewer(QQuickItem *parent = nullptr) : Base3dItemViewer(parent){}

    Renderer *createRenderer() const override;

    Q_INVOKABLE void set_sample(tool::gl::BaseGlSample *sample);

    bool newSample = false;
    tool::gl::BaseGlSample *currentSample = nullptr;
};

