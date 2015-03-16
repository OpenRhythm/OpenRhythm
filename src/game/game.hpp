#pragma once
#include <ios>
#include <string>
#include "window.hpp"
#include "context.hpp"
#include "events.hpp"
#include "config.hpp"
#include "timing.hpp"
#include "shader.hpp"
#include "mesh2d.hpp"
#include "gl.hpp"
#include "glm/glm.hpp"
#include "glm/ext.hpp"


class GameManager
{
private:
    bool m_running;
    double m_fpsTime;
    int m_width;
    int m_height;
    bool m_fullscreen;
    std::string m_title;

    MgCore::Window *m_window;
    MgCore::Context *m_context;
    MgCore::Events *m_events;
    MgCore::FpsTimer *m_clock;
    MgCore::ShaderProgram *m_program;
    MgCore::Listener m_lis;
    MgCore::Mesh2D *m_mesh;

    GLuint m_vao;
    std::streamsize m_ss;
    glm::mat4 m_ortho;
    int m_orthoID;

public:
    GameManager();
    ~GameManager();
    void start();
    bool event_handler(MgCore::Event &event);
    void update();
    void render();
    void resize(int width, int height);

};
