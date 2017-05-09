#pragma once
#include <vector>
#include <ios>

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
        int m_texture2;
        int m_program;

        std::shared_ptr<spdlog::logger> m_logger;

        std::streamsize m_ss;
        glm::mat4 m_ortho;
        glm::mat4 m_perspective;
    };
}