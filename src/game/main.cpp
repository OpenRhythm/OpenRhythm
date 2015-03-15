#include <ios>
#include <iostream>

#include "gl.hpp"

#include "window.hpp"
#include "context.hpp"
#include "events.hpp"
#include "config.hpp"
#include "timing.hpp"

bool running = true;

bool handleEvents(MgCore::Event &event)
{
    MgCore::EventType type = event.type;
    switch(type) {
        case MgCore::EventType::Quit:
            running = false;
            break;
        case MgCore::EventType::WindowSized:
            std::cout << event.event.windowSized.width  << " "
                      << event.event.windowSized.height << " "
                      << event.event.windowSized.id     << std::endl;
            break;

    }
    return true;
}

int main()
{

    MgCore::Window win;
    MgCore::Context con(3, 2, 0);
    MgCore::Events eve;
    MgCore::FpsTimer tim;

    win.make_current(&con);

    if(ogl_LoadFunctions() == ogl_LOAD_FAILED)
    {
        std::cout << "Error: glLoadGen failed to load.";
    }

    MgCore::Listener lis;
    lis.handler = handleEvents;
    lis.mask = MgCore::EventType::Quit | MgCore::EventType::WindowSized;

    eve.add_listener(lis);

    double fpsTime;

    std::streamsize ss = std::cout.precision();

    while (running) {
        //std::cout << "cow";
        fpsTime += tim.tick();
        eve.process();
        glClearColor(0.5, 0.5, 0.5, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        win.flip();
        if (fpsTime >= 2.0) {
            std::cout.precision (5);
            std::cout << "FPS: " << tim.get_fps() << std::endl;
            std::cout.precision (ss);
            fpsTime = 0;
        }

    }



    win.make_current(nullptr);
    return 0;
}
