#pragma once
#include <ios>
#include "window.hpp"
#include "context.hpp"
#include "events.hpp"
#include "config.hpp"
#include "timing.hpp"
#include "shader.hpp"
#include "gl.hpp"


class GameManager
{
private:
    bool m_running;
    double m_fpsTime;

    MgCore::Window *m_window;
    MgCore::Context *m_context;
    MgCore::Events *m_events;
    MgCore::FpsTimer *m_clock;
    MgCore::ShaderProgram *m_program;
    MgCore::Listener m_lis;

    GLuint m_vbo;
    GLuint m_vertLoc;
    GLuint m_vao;
    std::streamsize m_ss;

public:
    GameManager();
    ~GameManager();
    void start();
    bool event_handler(MgCore::Event &event);
    void update();
    void render();

};
