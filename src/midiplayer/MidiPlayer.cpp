#include "config.hpp"
#include "MidiPlayer.hpp"

#include <iostream>
#include <stdexcept>

#include "vfs.hpp"
namespace MidiPlayer
{
    MidiDisplayManager::MidiDisplayManager()
    :m_width(800),
    m_height(600),
    m_fullscreen(false),
    m_title("MidiPlayer"),
    m_window(m_width, m_height, m_fullscreen, m_title),
    m_context(3, 2, 0),
    m_eventManager(),
    m_eventPump(&m_eventManager),
    m_boardSpeed(0.001),
    m_maxNotes(127)
    {
        m_running = true;

        m_logger = spdlog::get("default");

        m_window.make_current(&m_context);
        
        m_window.disable_sync();

        
        m_song.load();

        if(!gladLoadGL())
        {
            throw std::runtime_error(_("Error: GLAD failed to load."));
        }

        m_lis.handler = std::bind(&MidiDisplayManager::event_handler, this, std::placeholders::_1);
        m_lis.mask = ORCore::EventType::EventAll;

        m_eventManager.add_listener(m_lis);

        m_fpsTime = 0.0;

        m_ss = std::cout.precision();

        ORCore::ShaderInfo vertInfo {GL_VERTEX_SHADER, "./data/shaders/main.vs"};
        ORCore::ShaderInfo fragInfo {GL_FRAGMENT_SHADER, "./data/shaders/main.fs"};

        m_program = m_renderer.add_program(ORCore::Shader(vertInfo), ORCore::Shader(fragInfo));
        m_texture = m_renderer.add_texture(ORCore::loadSTB("data/blank.png"));

        resize(m_width, m_height);

        m_logger->info(_("Preping notes for render"));
        //m_renderer->mesh_clear();
        prep_render_notes(m_songTime-1000, 1000.0);
        m_logger->info(_("Notes prep'd"));

        m_logger->info(_("Committing geometry."));
        m_renderer.commit();
        m_logger->info(_("Starting rendering."));

        glClearColor(0.5, 0.5, 0.5, 1.0);

        std::cout << "Done INIT " << std::endl;
    }

    MidiDisplayManager::~MidiDisplayManager()
    {
        m_window.make_current(nullptr);

    }

    // multithread this k!
    void MidiDisplayManager::prep_render_notes(double time, double length)
    {
        int trackColorIndex = 0;
        ORCore::RenderObject obj;
        obj.set_texture(m_texture);
        obj.set_program(m_program);
        for (auto track: *m_song.get_tracks())
        {
            glm::vec4 color = m_colorArray[trackColorIndex];
            m_colorArray[trackColorIndex] += m_colorMutator[trackColorIndex];
            trackColorIndex++;
            auto noteInfo = track.get_notes();//get_notes_in_frame(time, time+length);
            for(int i = noteInfo.start; i < noteInfo.end; i++) {


                auto &note = (*noteInfo.notes)[i];
                float z = note.time;
                //std::cout << "Length: " << note.length << std::endl;

                float noteLength = note.length*m_boardSpeed;

                obj.set_scale(glm::vec3{0.005f, noteLength, 0.0f});
                obj.set_translation(glm::vec3{(static_cast<int>(note.noteValue)/(m_maxNotes*1.0)), -(z*m_boardSpeed), 0.0f}); // center the line on the screen
                obj.set_primitive_type(ORCore::Primitive::triangle);
                obj.set_geometry(ORCore::create_rect_mesh(color));

                m_renderer.add_object(obj);
            } 
        }

    }

    void MidiDisplayManager::start()
    {
        GLenum error;
        while (m_running)
        {
            m_fpsTime += m_clock.tick();
            m_eventPump.process();
            update();
            render();

            do
            {
                error = glGetError();
                if (error != GL_NO_ERROR)
                {
                    m_logger->info(_("GL error: {}"), error);
                }
            } while(error != GL_NO_ERROR);

            m_window.flip();
            if (m_fpsTime >= 2000.0) {
                std::cout.precision (5);
                std::cout << "FPS: " << m_clock.get_fps() << std::endl;
                std::cout << "Song Time: " << m_songTime << std::endl;
                std::cout.precision (m_ss);
                m_fpsTime = 0;
            }
        }
    }

    void MidiDisplayManager::resize(int width, int height)
    {
        m_width = width;
        m_height = height;
        glViewport(0, 0, m_width, m_height);
        m_ortho = glm::ortho(0.0f, static_cast<float>(1.0), static_cast<float>(1.0), 0.0f, -1.0f, 1.0f);
    }

    bool MidiDisplayManager::event_handler(const ORCore::Event &event)
    {
        switch(event.type) {
            case ORCore::Quit: {
                m_running = false;
                break;
            }
            case ORCore::WindowSize: {
                auto ev = ORCore::event_cast<ORCore::WindowSizeEvent>(event);
                resize(ev.width, ev.height);
                break;
            }
            default:
                break;
        }
        return true;
    }

    void MidiDisplayManager::handle_song()
    {
    }

    void MidiDisplayManager::update()
    {
        m_songTime = m_clock.get_current_time();

        m_renderer.set_camera_transform("ortho", glm::translate(m_ortho, glm::vec3(0.0f, m_songTime*m_boardSpeed, 0.0f))); // translate projection with song

        // m_renderer->mesh_clear();
        // prep_render_notes(m_songTime-1000, 1000.0);
        // m_renderer->mesh_commit();
    }

    void MidiDisplayManager::render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_renderer.render();
    }
}