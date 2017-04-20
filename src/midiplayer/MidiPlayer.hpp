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
#include "midi_song.hpp"

#include <spdlog/spdlog.h>

namespace MidiPlayer
{

    class MidiDisplayManager
    {
    public:
        MidiDisplayManager();
        ~MidiDisplayManager();
        void start();
        bool event_handler(const ORCore::Event &event);
        void handle_song();
        void update();
        void prep_render_notes(double time, double length);
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

        double m_songTime;
        double m_boardSpeed;
        int m_maxNotes;

        ORCore::FpsTimer m_clock;
        Song m_song;

        ORCore::Window m_window;
        ORCore::Context m_context;
        ORCore::EventManager m_eventManager;
        ORCore::EventPumpSDL2 m_eventPump;
        ORCore::Renderer m_renderer;
        
        int m_texture;
        int m_program;

        ORCore::Listener m_lis;

        std::shared_ptr<spdlog::logger> m_logger;

        std::streamsize m_ss;
        glm::mat4 m_ortho;

        std::vector<glm::vec4> m_colorArray = {glm::vec4{1.0, 0.0, 0.0, 1.0},
                                glm::vec4{0.0, 0.0, 1.0, 1.0},
                                glm::vec4{0.0, 0.0, 1.0, 1.0},
                                glm::vec4{1.0, 0.5, 0.0, 1.0},
                                glm::vec4{1.0, 0.0, 1.0, 1.0},
                                glm::vec4{0.0, 1.0, 1.0, 1.0}};

        std::vector<glm::vec4> m_colorMutator = {glm::vec4{-0.0001, 0.001, 0.01, 1.0},
                                glm::vec4{0.01, 0.002, -0.01, 1.0},
                                glm::vec4{0.002, 0.0, -0.001, 1.0},
                                glm::vec4{-0.001, -0.0001, 0.0, 1.0},
                                glm::vec4{0.002, 0.0, -0.001, 1.0},
                                glm::vec4{0.002, -0.001, -0.001, 1.0}};

    };
}