#include <ios>
#include <iostream>

#include "gl.hpp"

#include "window.hpp"
#include "context.hpp"
#include "events.hpp"
#include "config.hpp"
#include "timing.hpp"
#include "shader.hpp"

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

    double fpsTime = 0.0;

    std::streamsize ss = std::cout.precision();

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    MgCore::ShaderInfo vertInfo {GL_VERTEX_SHADER, "../data/shaders/main.vs"};
    MgCore::ShaderInfo fragInfo {GL_FRAGMENT_SHADER, "../data/shaders/main.fs"};

    MgCore::Shader vertShader(vertInfo);
    MgCore::Shader fragShader(fragInfo);

    MgCore::ShaderProgram program(&vertShader, &fragShader);
    program.use();

    static const GLfloat vertData[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
    };

    GLuint vertLoc = program.vertex_attribute("position");

    glClearColor(0.5, 0.5, 0.5, 1.0);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertData), vertData, GL_STATIC_DRAW);

    while (running) {
        fpsTime += tim.tick();
        eve.process();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.use();

        glEnableVertexAttribArray(vertLoc);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer( vertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr );
         
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glDisableVertexAttribArray(vertLoc);

        win.flip();
        if (fpsTime >= 2.0) {
            std::cout.precision (5);
            std::cout << "FPS: " << tim.get_fps() << std::endl;
            std::cout.precision (ss);
            fpsTime = 0;
        }

    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    win.make_current(nullptr);
    return 0;
}
