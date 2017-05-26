#pragma once
#include <vector>
#include <ios>
#include <map>

#include "window.hpp"
#include "context.hpp"
#include "events.hpp"
#include "timing.hpp"
#include "renderer/shader.hpp"
#include "renderer/renderer.hpp"
#include "renderer/texture.hpp"
#include "song.hpp"

#include <spdlog/spdlog.h>

namespace ORGame
{


    const std::map<NoteType, glm::vec4> noteColorMap = {
        {NoteType::Green, {0.0f,1.0f,0.0f,1.0f}},
        {NoteType::Red, {1.0f,0.0f,0.0f,1.0f}},
        {NoteType::Yellow, {1.0f,1.0f,0.0f,1.0f}},
        {NoteType::Blue, {0.0f,0.0f,1.0f,1.0f}},
        {NoteType::Orange, {1.0f,0.5f,0.0f,1.0f}},
    };
    
    const std::map<NoteType, glm::vec4> noteColorMapActive = {
        {NoteType::Green, {0.25f,1.5f,0.25f,1.0f}},
        {NoteType::Red, {1.5f,0.25f,0.25f,1.0f}},
        {NoteType::Yellow, {1.5f,1.5f,0.25f,1.0f}},
        {NoteType::Blue, {0.25f,1.5f,4.5f,1.0f}},
        {NoteType::Orange, {3.0f,1.5f,0.5f,1.0f}},
    };

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
        void prep_render_notes();
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

        TempoTrack *m_tempoTrack;
        Track *m_playerTrack;
        double m_songTime;

        Song m_song;
        ORCore::FpsTimer m_clock;

        ORCore::Context m_context;
        ORCore::Window m_window;
        ORCore::EventManager m_eventManager;
        ORCore::EventPumpSDL2 m_eventPump;
        ORCore::Renderer m_renderer;
        ORCore::Listener m_lis;
        int m_texture;
        int m_tailTexture;
        int m_fretsTexture;
        int m_neckTexture;
        int m_soloNeckTexture;
        int m_program;
        int m_neckProgram;
        int m_fretObj;
        int m_neckObj;

        std::shared_ptr<spdlog::logger> m_logger;

        std::streamsize m_ss;
        glm::mat4 m_ortho;
        glm::mat4 m_perspective;
        glm::mat4 m_rotPerspective;
    };
}