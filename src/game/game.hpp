#pragma once
#include <vector>
#include <ios>

#include "window.hpp"
#include "context.hpp"
#include "events.hpp"
#include "timing.hpp"
#include "shader.hpp"
#include "mesh2d.hpp"
#include "texture.hpp"
#include "parser.hpp"


class GameManager
{
public:
    GameManager();
    ~GameManager();
    void start();
    bool event_handler(const ORCore::Event &event);
    void handle_song();
    void update();
    void prep_render_bars();
    void render();
    void resize(int width, int height);
private:
    bool m_running;
    double m_fpsTime;
    int m_width;
    int m_height;
    bool m_fullscreen;
    std::string m_title;

    int m_mouseX = 0;
    int m_mouseY = 0;

    std::vector<ORGame::TempoTrackEvent> m_barsForRender;
    ORGame::TempoTrack *m_tempoTrack;
    double m_songTime;

    ORGame::Song m_song;
    ORCore::FpsTimer m_clock;

    std::unique_ptr<ORCore::Window> m_window;
    std::unique_ptr<ORCore::Context> m_context;
    std::unique_ptr<ORCore::EventManager> m_eventManager;
    std::unique_ptr<ORCore::EventPumpSDL2> m_eventPump;
    std::unique_ptr<ORCore::ShaderProgram> m_program;

    std::vector<ORCore::Mesh2D> m_meshes;
    std::unique_ptr<ORCore::Texture> m_texture;
    ORCore::Listener m_lis;

    std::shared_ptr<spdlog::logger> m_logger;

    GLuint m_vao;
    std::streamsize m_ss;
    glm::mat4 m_ortho;
    int m_orthoID;
};
