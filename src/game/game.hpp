#pragma once
#include <ios>
#include <string>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include "window.hpp"
#include "context.hpp"
#include "events.hpp"
#include "config.hpp"
#include "timing.hpp"
#include "shader.hpp"
#include "mesh2d.hpp"
#include "texture.hpp"
#include "parser.hpp"


class GameManager
{
private:
    bool m_running;
    double m_fpsTime;
    int m_width;
    int m_height;
    bool m_fullscreen;
    std::string m_title;

    int m_mouseX = 0;
    int m_mouseY = 0;

    std::vector<MgGame::TempoTrackEvent> m_barsForRender;
    MgGame::TempoTrack *m_tempoTrack;
    double m_songTime;

    MgGame::Song m_song;
    MgCore::FpsTimer m_clock;

    std::unique_ptr<MgCore::Window> m_window;
    std::unique_ptr<MgCore::Context> m_context;
    std::unique_ptr<MgCore::Events> m_events;
    std::unique_ptr<MgCore::ShaderProgram> m_program;

    std::vector<MgCore::Mesh2D> m_meshes;
    std::unique_ptr<MgCore::Texture> m_texture;
    MgCore::Listener m_lis;

    std::shared_ptr<spdlog::logger> m_logger;

    GLuint m_vao;
    std::streamsize m_ss;
    glm::mat4 m_ortho;
    int m_orthoID;

public:
    GameManager();
    ~GameManager();
    void start();
    bool event_handler(MgCore::Event &event);
    void handle_song();
    void update();
    void prep_render_bars();
    void render();
    void resize(int width, int height);

};
