
/*******************************************************************************
** Toolset-scaner-manager                                                     **
** MIT License                                                                **
** Copyright (c) [2018] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/

#include "multi_depth_camera_gl_widget.hpp"

// qt-utility
#include "qt_logger.hpp"

using namespace tool;
using namespace tool::ui;
using namespace tool::geo;
using namespace tool::cam;

void MultiDepthCameraGlWidget::init(size_t nbCameras){

    models.resize(nbCameras);
    visibility.resize(nbCameras);
    colors.resize(nbCameras);
    forceColors.resize(nbCameras);
    sizePts.resize(nbCameras);
    kinectsBodies.resize(nbCameras);

    meshes.resize(nbCameras);
    for(size_t ii = 0; ii < nbCameras; ++ii){
        // kinectClouds.emplace_back(std::make_unique<gl::CloudPointsDrawer>());
    }
    for(size_t ii = 0; ii < nbCameras; ++ii){
        // kinectMeshes.emplace_back(std::make_unique<gl::MeshDrawer>());
    }

    std::fill(std::begin(visibility), std::end(visibility), true);
    std::fill(std::begin(forceColors), std::end(forceColors), false);
    std::fill(std::begin(colors), std::end(colors), Qt::red);
    std::fill(std::begin(sizePts), std::end(sizePts), 3.f);
}

void MultiDepthCameraGlWidget::update_cloud(size_t id, K2CloudDisplayData *cloudData){

    cloudData->dataLocker.lock();
    // kinectClouds[id]->init(cloudData->sizePts, cloudData->points->data(), cloudData->colors->data());
    cloudData->dataLocker.unlock();

    vaoToDisplay = 1;
    update();
}

void MultiDepthCameraGlWidget::update_raw_cloud(size_t id, std::vector<Pt3f> *points, std::vector<Pt3f> *colors){

    // kinectClouds[id]->init(points, colors);
    vaoToDisplay = 1;
    update();
}

void MultiDepthCameraGlWidget::update_mesh(size_t id, K2MeshDisplayData *meshData){

    meshData->dataLocker.lock();
        meshes[id].vertices.resize(meshData->sizePts);
        meshes[id].normals.resize(meshData->sizePts);
        meshes[id].colors.resize(meshData->sizePts);
        meshes[id].triIds.resize(meshData->sizeTris);

        std::copy(std::begin(*meshData->points),  std::begin(*meshData->points) + meshData->sizePts, std::begin(meshes[id].vertices));
        std::copy(std::begin(*meshData->normals), std::begin(*meshData->normals) + meshData->sizePts, std::begin(meshes[id].normals));

        for(size_t ii = 0; ii < meshData->sizePts; ++ii){
            meshes[id].colors[ii] = geo::to_pt4((*meshData->colors)[ii], 1.f);
        }
        for(size_t ii = 0; ii < meshData->sizeTris; ++ii){
            // const auto &tri = (*meshData->triangles)[ii];
            // meshes[id].triIds[ii].ids = geo::Pt3<size_t>{tri.x(), tri.y(), tri.z()};
        }
    meshData->dataLocker.unlock();

    // kinectMeshes[id]->init(&meshes[id]);
    vaoToDisplay = 2;

    update();
}

void MultiDepthCameraGlWidget::init_joints(){
    // jointsMesh.init();
}

void MultiDepthCameraGlWidget::update_grabber_cloud_display_parameters(size_t id, DisplayOptions p){
    sizePts[id]     = p.sizePtsCloud;
    visibility[id]  = p.displayCloud;
    forceColors[id] = p.forceCloudColor;
    colors[id]      = p.colorCloud;
    models[id]      = p.model;
}

void MultiDepthCameraGlWidget::update_bodies(size_t id,  std::array<tool::cam::K2BodyInfos, 6> *bodies){
    kinectsBodies[id] = *bodies;
}

void MultiDepthCameraGlWidget::OnInit(){

    if(!meshShader.init()){
        QtLogger::error("MultiDepthCameraGlWidget::Cannot load mesh shader.");
        return;
    }

    if(!cloudShader.init()){
        QtLogger::error("MultiDepthCameraGlWidget::Cannot load cloud shader.");
        return;
    }

    init_axes();
    init_grid();
    init_joints();
//    frustum.init(); // crash

    // enable / disable
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
}

void MultiDepthCameraGlWidget::OnUpdate(){

    // time
    currrentTime = clock.getElapsedTime();

    float currentFrame = currrentTime.asMilliseconds();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // clean
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // viewport
    glViewport(0,0, width(), height());

    // set polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // init model
    if(displayGrid){
        draw_grid();
    }
    if(displayAxes){
        draw_axes();
    }


    for(size_t idC = 0; idC < 10; ++idC){

        if(!visibility[idC]){ // display only visible
            continue;
        }

        if(vaoToDisplay == 1){ // draw cloud

            // cloud shader
            cloudShader.use();
            cloudShader.set_uniform_matrix("view"sv, camera.view().conv<float>());
            cloudShader.set_uniform_matrix("model"sv, (models[idC]).conv<float>());
            cloudShader.set_uniform_matrix("projection"sv, camera.projection().conv<float>());
            cloudShader.set_uniform("enable_unicolor"sv, forceColors[idC]);
            cloudShader.set_uniform("unicolor"sv, Pt4d{colors[idC].redF(),colors[idC].blueF(),colors[idC].greenF(),1.}.conv<float>());
            cloudShader.set_uniform("size_pt"sv, sizePts[idC]);
            cloudShader.set_uniform("camera_position"sv, camera.position().conv<float>());
            // kinectClouds[idC]->draw();

//            auto f = dynamic_cast<gl::Frustum*>(frustum.object());
//            f->set_perspective(60.0f, 1.207f, -0.3f, -3.5f);
//            cloudShader.set_uniform("model", models[idC].conv<float>());
//            frustum.draw();

        }else if (vaoToDisplay == 2){

            // mesh shader
            meshShader.use();
            meshShader.set_uniform_matrix("view"sv,         camera.view().conv<float>());
            meshShader.set_uniform_matrix("model"sv,        (models[idC]).conv<float>());
            meshShader.set_uniform_matrix("projection"sv,   camera.projection().conv<float>());
            meshShader.set_uniform("enable_unicolor"sv, false);
            // kinectMeshes[idC]->draw();

//            auto f = dynamic_cast<gl::Frustum*>(frustum.object());
//            f->set_perspective(60.0f, 1.0f, -1.0f, -25.0f);
//            cloudShader.set_uniform("model", models[idC].conv<float>());
//            frustum.draw();
        }

        if(displayJoints){

            meshShader.use();
            meshShader.set_uniform_matrix("view"sv,         camera.view().conv<float>());
            meshShader.set_uniform_matrix("projection"sv,   camera.projection().conv<float>());
            meshShader.set_uniform("enable_unicolor"sv, true);

            for(const auto &kinectBodies : kinectsBodies){
                for(const auto &body : kinectBodies){
                    if(!body.tracked){
                        continue;
                    }
                    if(m_bodiesColors.count(body.id) == 0){
                        m_bodiesColors[body.id] = geo::Pt4f{0.001f*(rand()%1000),0.001f*(rand()%1000),0.001f*(rand()%1000),1};
                    }
                    meshShader.set_uniform("unicolor"sv, m_bodiesColors[body.id]);

                    for(const auto &joint : body.joints){
                        if(joint.second.state == K2TrackingStateT::not_tracked){
                            continue;
                        }

                        const auto &p = joint.second.pos;
                        const auto q = joint.second.rotQuaternion.conv<double>();

                        geo::Quatd quat(q.x(), q.y(), q.z(), q.w());
                        geo::Mat4d jointsModel = geo::transform( // geo::transform2
                            geo::Vec3d{0.02,0.02,0.02},
                            euler_angles(quat) * d180_PI<double>,
                            translation_v3(models[idC])+geo::Pt3d{static_cast<double>(p.x()),static_cast<double>(p.y()),static_cast<double>(p.z())}
                        );

                        meshShader.set_uniform_matrix("model"sv, (jointsModel).conv<float>());
                        // jointsMesh.draw();
                    }
                }
            }
        }
    }




}

void MultiDepthCameraGlWidget::onResize(){

}

