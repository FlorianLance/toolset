/*******************************************************************************
** Toolset-3d-engine                                                          **
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

#pragma once


// std
#include <chrono>
#include <optional>

// glew
#include <GL/glew.h>

// sfml
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

// base
#include "thirdparty/sigslot/signal.hpp"
#include "graphics/camera/camera.hpp"


namespace tool::graphics {

class BaseSfmlGlWindow{

public:

    using TimePoint = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;

    BaseSfmlGlWindow(
        std::string_view title,
        graphics::Screen screen,
        std::optional<sf::ContextSettings> context
    );
    virtual ~BaseSfmlGlWindow();

    auto init() -> bool;
    auto start() -> void;

    auto elapsed_secondes() const -> float{
        using namespace std::chrono;
        return duration_cast<milliseconds>(currentFrame-startL).count()*0.001f;
    }

    auto quit() -> void{
        running = false;
    }

public:

    // signals
    sigslot::signal<> pre_update_signal;
    sigslot::signal<> update_signal;
    sigslot::signal<> post_update_signal;
    sigslot::signal<> draw_gl_signal;
    sigslot::signal<geo::Pt2f> draw_imgui_signal;
    sigslot::signal<> draw_sfml_signal;

protected:

    // init
    virtual auto initialize_gl() -> bool = 0;
    virtual auto initialize_imgui() -> void{}

    // resize
    virtual auto resize_windows() -> void{}

    // draw
    virtual auto draw_gl() -> void{}
    virtual auto draw_imgui() -> void{}
    virtual auto draw_sfml() -> void{}    

    // update
    virtual auto pre_update() -> void{}
    virtual auto update() -> void{}
    virtual auto post_update() -> void{}

    // clean
    virtual auto clean() -> void {};

    // sfml events
    // # mouse
    virtual auto mouse_button_pressed_event(sf::Event::MouseButtonEvent event) -> void;
    virtual auto mouse_button_released_event(sf::Event::MouseButtonEvent event) -> void;
    virtual auto mouse_moved_event(sf::Event::MouseMoveEvent event) -> void;
    virtual auto mouse_wheel_scroll_event(sf::Event::MouseWheelScrollEvent event) -> void;
    // # key
    virtual auto keyboard_keypress_event(sf::Event::KeyEvent event) -> void;
    virtual auto keyboard_keyrelease_event(sf::Event::KeyEvent event) -> void {static_cast<void>(event);}

    // camera
    virtual auto update_camera_with_mouse_button_event(sf::Event::MouseButtonEvent event, bool pressed) -> void;;
    virtual auto update_camera_with_keyboardpress_event(sf::Event::KeyEvent event) -> void;;
    virtual auto update_camera_with_mouse_scroll_event(sf::Event::MouseWheelScrollEvent event) -> void;;
    virtual auto update_camera_with_mouse_moved_event(sf::Event::MouseMoveEvent event) -> void;;

private:

    auto init_sfml_window() -> bool;
    auto base_resize_windows(sf::Event::SizeEvent size) -> void;

protected:

    // loop
    bool running = false;

    // opengl    
    bool m_glInitialized = false;
    sf::RenderWindow m_scene;
    sf::ContextSettings glContext;

    // window
    std::string m_title = "Base SFML GL window";
    std::string m_imguiWindowTitle = "Default";
    graphics::Screen m_screen;

    // camera
    double cameraMovingSpeed   = 0.2;
    double cameraScrollSpeed   = 0.05;
    double cameraRotationSpeed = 0.05;
    graphics::Camera m_camera;

    // time
    TimePoint startL;
    TimePoint currentFrame;
    int framerate = 60;
    sf::Clock deltaClock;

    std::chrono::duration<double, std::milli> frameDuration;
    std::chrono::duration<double, std::milli> timePerFrame{1000./framerate};

    // inputs
    // # mouse
    bool mouseLeftClickPressed = false;
    bool mouseRightClickPressed = false;
    bool mouseMiddleClickPressed = false;
    int lastX=-1, lastY=-1;

    bool imguiMouse = false;
    bool imguiKeyboard = false;
};



}

