
/*******************************************************************************
** Toolset-imgui-opengl-engine                                                **
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

#include "base_sfml_gl_window.hpp"

// std
#include <string>
#include <thread>

// sfml
#include "imgui-sfml/imgui-SFML.h"

// imgui
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"

// base
#include "utility/logger.hpp"
#include "utility/time.hpp"

// opengl-utility
#include "opengl/vao.hpp"
#include "opengl/gl_functions.hpp"

using namespace tool::gl;
using namespace tool::geo;
using namespace tool::graphics;
using namespace std::string_view_literals;

BaseSfmlGlWindow::BaseSfmlGlWindow(std::string_view title, graphics::Screen screen, std::optional<sf::ContextSettings> context) ://, std::optional<sf::Style> style) :
      m_title(title), m_screen(screen), m_camera(&m_screen, {0,0,0}, {0,0,1}) {

    if(context.has_value()){
        glContext = context.value();
    }else{
        glContext.depthBits         = 24;
        glContext.stencilBits       = 8;
        glContext.antialiasingLevel = 4;
        glContext.majorVersion      = 4;
        glContext.minorVersion      = 6;
        glContext.attributeFlags    = sf::ContextSettings::Attribute::Debug;
    }
}

BaseSfmlGlWindow::~BaseSfmlGlWindow(){
    m_scene.close();
}

auto BaseSfmlGlWindow::init() -> bool{

    // sfml
    init_sfml_window();

    // glew
    if(!GL::init_glew()){
        m_scene.close();
        return false;
    }
    GL::display_glew_info();

    // init imgui
    m_scene.setActive(true);

    if(!initialize_gl()){
        return false;
    }

    if(!ImGui::SFML::Init(m_scene)){
        return false;
    }
    initialize_imgui();

    resize_windows();

    return true;
}

auto BaseSfmlGlWindow::start() -> void{
    
    auto lg = LogG("BaseSfmlGlWindow::start"sv);
    if(!m_glInitialized){
        return;
    }

    m_scene.setActive();

    startL = Time::nanoseconds_since_epoch();

    running = true;
    Log::log("Start GL loop\n"sv);
    bool firstLoop = true;

    ImGui::GetIO().MouseDrawCursor = true;

    while(running){

        const auto &io = ImGui::GetIO();
        imguiMouse    = io.WantCaptureMouse;
        imguiKeyboard = io.WantCaptureKeyboard;

        currentFrame = Time::nanoseconds_since_epoch();

        // retrieve sfml events
        while (auto event = m_scene.pollEvent()){

            ImGui::SFML::ProcessEvent(m_scene, event.value());

            if (const auto* sE = event->getIf<sf::Event::Closed>()){ // The window requested to be closed
                Log::message("Close window.\n");
                running = false;
                break;
            }else if(const auto* sE = event->getIf<sf::Event::Resized>()){ // The window was resized
                base_resize_windows(*sE);
            }else if(const auto* sE = event->getIf<sf::Event::KeyPressed>()){ // A key was pressed
                keyboard_keypress_event(*sE);
            }else if(const auto* sE = event->getIf<sf::Event::KeyReleased>()){ // A key was released
                keyboard_keyrelease_event(*sE);
            }else if(const auto* sE = event->getIf<sf::Event::MouseButtonPressed>()){ // A mouse button was pressed
                mouse_button_pressed_event(*sE);
            }else if(const auto* sE = event->getIf<sf::Event::MouseButtonReleased>()){ // A mouse button was released
                mouse_button_released_event(*sE);
            }else if(const auto* sE = event->getIf<sf::Event::MouseMoved>()){ // The mouse cursor moved
                mouse_moved_event(*sE);
            }else if(const auto* sE = event->getIf<sf::Event::MouseWheelScrolled>()){ // The mouse wheel was scrolled
                mouse_wheel_scroll_event(*sE);
            }else if(const auto* sE = event->getIf<sf::Event::MouseEntered>()){ // The mouse cursor entered the area of the window

            }else if(const auto* sE = event->getIf<sf::Event::MouseLeft>()){ // The mouse cursor left the area of the window

            }else if(const auto* sE = event->getIf<sf::Event::JoystickButtonPressed>()){ // A joystick button was pressed

            }else if(const auto* sE = event->getIf<sf::Event::JoystickButtonReleased>()){ // A joystick button was released

            }else if(const auto* sE = event->getIf<sf::Event::JoystickMoved>()){ // The joystick moved along an axis

            }else if(const auto* sE = event->getIf<sf::Event::JoystickConnected>()){ // A joystick was connected

            }else if(const auto* sE = event->getIf<sf::Event::JoystickDisconnected>()){ // A joystick was disconnected

            }

            // LostFocus,     < The window lost the focus (no data)
            // GainedFocus,   < The window gained the focus (no data)
            // TextEntered,   < A character was entered (data in event.text)
            // TouchBegan,    < A touch event began (data in event.touch)
            // TouchMoved,    < A touch moved (data in event.touch)
            // TouchEnded,    < A touch event ended (data in event.touch)
            // SensorChanged, < A sensor value changed (data in event.sensor)
        }

        if(imguiMouse){
            mouseLeftClickPressed   = false;
            mouseRightClickPressed  = false;
            mouseMiddleClickPressed = false;
        }
        if(imguiKeyboard){

        }

        // update
        if(firstLoop){
            Log::log("Pre-update\n"sv);
        }
        pre_update();
        pre_update_signal();
        if(firstLoop){
            Log::log("Update\n"sv);
        }
        update();
        update_signal();
        if(firstLoop){
            Log::log("Post-Update\n"sv);
        }
        post_update();
        post_update_signal();

        m_scene.clear(sf::Color::White);

        // draw opengl
        if(firstLoop){
            Log::log("Draw-GL\n"sv);
        }
        draw_gl();
        draw_gl_signal();

        // ubind vao after drawing opengl
        VAO::unbind();        

        // store gl states
        m_scene.pushGLStates();
        {

            // update sfml
            ImGui::SFML::Update(m_scene, deltaClock.restart());

            if(firstLoop){
                Log::log("Draw-ImGUI\n"sv);
            }

            // imgui
            imguiMouse = false;
            draw_imgui();
            draw_imgui_signal(geo::Pt2f{
                1.f*m_camera.screen()->width(),
                1.f*m_camera.screen()->height()
            });

            ImGui::EndFrame();

            if(firstLoop){
                Log::log("Draw-SFML\n"sv);
            }

            // render sfml scene
            draw_sfml();
            draw_sfml_signal();
            ImGui::SFML::Render(m_scene);
            m_scene.display();
        }
        // restore gl states
        m_scene.popGLStates();

        if(firstLoop){
            Log::log("Sleep\n"sv);
        }

        // sleep for fps
        frameDurationMs = Time::difference_ms(currentFrame, Time::nanoseconds_since_epoch());
        std::chrono::milliseconds sleepTime = msPerFrame-frameDurationMs;
        if(sleepTime.count() > 0){
            std::this_thread::sleep_for(sleepTime);
        }
        mainThreadDurationMSA.add_value(1.0*frameDurationMs.count());

        firstLoop = false;

        if(firstLoop){
            Log::log("End loop\n"sv);
        }
    }

    clean();

    ImGui::SFML::Shutdown();
}

auto BaseSfmlGlWindow::elapsed_secondes() const -> float{
    return Time::difference_ms(startL, currentFrame).count()*0.001f;
}

auto BaseSfmlGlWindow::main_thread_duration_ms() const -> double{
    return mainThreadDurationMSA.get();
}

auto BaseSfmlGlWindow::quit() -> void{
    running = false;
}

auto BaseSfmlGlWindow::init_sfml_window() -> bool{
    
    auto lg = LogG("BaseSfmlGlWindow::init_sfml_window"sv);
    // close previously opened window
    if(m_scene.isOpen()){
        m_scene.close();
    }

    // create window
    m_scene.create(sf::VideoMode(sf::Vector2u(m_screen.width(), m_screen.height())), m_title, sf::Style::Default, sf::State::Windowed, glContext);
    m_scene.setFramerateLimit(framerate);
    m_scene.setPosition(sf::Vector2i{m_screen.x_pos(),m_screen.y_pos()});

    return true;
}

auto BaseSfmlGlWindow::base_resize_windows(sf::Event::Resized event) -> void{
    
    auto lg = LogG("BaseSfmlGlWindow::base_resize_windows"sv);
    m_screen = graphics::Screen{event.size.x, event.size.y};
    m_camera.update_projection();
    glViewport(0, 0, static_cast<GLsizei>(m_screen.width()), static_cast<GLsizei>(m_screen.height()));

    resize_windows();
}

auto BaseSfmlGlWindow::mouse_button_pressed_event(sf::Event::MouseButtonPressed event) -> void{
    if(!imguiMouse){
        update_camera_with_mouse_button_pressed_event(event);
    }
}

auto BaseSfmlGlWindow::mouse_button_released_event(sf::Event::MouseButtonReleased event) -> void{
    if(!imguiMouse){
        update_camera_with_mouse_button_released_event(event);
    }
}

auto BaseSfmlGlWindow::mouse_moved_event(sf::Event::MouseMoved event) -> void{
    if(!imguiMouse){
        update_camera_with_mouse_moved_event(event);
    }
}

auto BaseSfmlGlWindow::mouse_wheel_scroll_event(sf::Event::MouseWheelScrolled event) -> void{
    if(!imguiMouse){
        if(!mouseMiddleClickPressed){
            update_camera_with_mouse_scroll_event(event);
        }
    }
}

auto BaseSfmlGlWindow::keyboard_keypress_event(sf::Event::KeyPressed event) -> void{

    if(!imguiMouse && !imguiKeyboard){
        update_camera_with_keyboardpress_event(event);
    }
}

auto BaseSfmlGlWindow::update_camera_with_mouse_button_pressed_event(sf::Event::MouseButtonPressed event) -> void{

    switch (event.button) {
    case sf::Mouse::Button::Left:
        mouseLeftClickPressed = true;
        break;
    case sf::Mouse::Button::Right:
        mouseRightClickPressed = true;
        break;
    case sf::Mouse::Button::Middle:
        mouseMiddleClickPressed = true;
        break;
    default:
        break;
    }
}

auto BaseSfmlGlWindow::update_camera_with_mouse_button_released_event(sf::Event::MouseButtonReleased event) -> void{

    bool mouseLeftClickReleasedEvent = false;

    switch (event.button) {
    case sf::Mouse::Button::Left:
        if(mouseLeftClickPressed){
            mouseLeftClickReleasedEvent = true;
        }
        mouseLeftClickPressed = false;
        break;
    case sf::Mouse::Button::Right:
        mouseRightClickPressed = false;
        break;
    case sf::Mouse::Button::Middle:
        mouseMiddleClickPressed = false;
        break;
    default:
        break;
    }

    lastX = -1;
    lastY = -1;

    if(mouseLeftClickReleasedEvent){
        m_camera.screen_raycast({lastX, lastY});
    }
}

auto BaseSfmlGlWindow::update_camera_with_keyboardpress_event(sf::Event::KeyPressed event)  -> void{

    switch (event.code) {
    case sf::Keyboard::Key::Up:
        m_camera.move_front(cameraMovingSpeed);
        break;
    case sf::Keyboard::Key::Left:
        m_camera.move_left(cameraMovingSpeed);
        break;
    case sf::Keyboard::Key::Right:
        m_camera.move_right(cameraMovingSpeed);
        break;
    case sf::Keyboard::Key::Down:
        m_camera.move_back(cameraMovingSpeed);
        break;
    case sf::Keyboard::Key::R:
        m_camera.reset_init_values();
        m_camera.set_direction(0.,0.,0.);
        break;
    case sf::Keyboard::Key::O:
        m_camera.set_mode(Camera::Mode::Orhtographic);
        break;
    case sf::Keyboard::Key::P:
        m_camera.set_mode(Camera::Mode::Perspective);
        break;
    default:
        break;
    }
}

auto BaseSfmlGlWindow::update_camera_with_mouse_scroll_event(sf::Event::MouseWheelScrolled event) -> void{

    if(event.wheel == sf::Mouse::Wheel::Vertical){
        m_camera.move_front(static_cast<double>(event.delta) * cameraScrollSpeed);
    }
}

auto BaseSfmlGlWindow::update_camera_with_mouse_moved_event(sf::Event::MouseMoved event) -> void{

    if(lastX < 0.){
        lastX = event.position.x;
        lastY = event.position.y;
    }

    double xoffset = event.position.x - lastX;
    double yoffset = event.position.y - lastY;
    lastX = event.position.x;
    lastY = event.position.y;

    double sensitivity = cameraRotationSpeed;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    if(mouseLeftClickPressed){
        m_camera.set_direction(xoffset,-yoffset,0.);
    }else if(mouseMiddleClickPressed){
        m_camera.move_up(-0.1*yoffset);
        m_camera.move_right(0.1*xoffset);
    }else if(mouseRightClickPressed){
        m_camera.set_direction(0.,0.,xoffset);
    }


}
