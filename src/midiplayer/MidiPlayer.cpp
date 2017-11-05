// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "config.hpp"
#include "MidiPlayer.hpp"

#include <iostream>
#include <stdexcept>

#include "filesystem.hpp"
namespace MidiPlayer
{
    MidiDisplayManager::MidiDisplayManager()
    :m_width(800),
    m_height(600),
    m_fullscreen(false),
    m_title("MidiPlayer"),
    m_context(3, 3, 0),
    m_window(m_width, m_height, m_fullscreen, m_title),
    m_eventManager(),
    m_eventPump(&m_eventManager),
    m_boardSpeed(0.5),
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

        m_renderer.init_gl();

        ORCore::ShaderInfo vertInfo {GL_VERTEX_SHADER, "./data/shaders/main.vs"};
        ORCore::ShaderInfo fragInfo {GL_FRAGMENT_SHADER, "./data/shaders/main.fs"};

        ORCore::ShaderProgram program;
        program.add_shader(ORCore::Shader(vertInfo));
        program.add_shader(ORCore::Shader(fragInfo));
        program.link();
        m_program = m_renderer.add_program(std::move(program));

        // Create cameras and register them with the renderer
        ORCore::CameraObject camera;
        camera.set_translation(glm::vec3(0.5f, 1.0f, -0.5));
        camera.set_rotation(0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        camera.set_uniform_name("proj");
        m_orthoCamera = m_renderer.add_camera(camera);

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
        obj.set_camera(m_orthoCamera);
        obj.set_program(m_program);
        obj.set_primitive_type(ORCore::Primitive::triangle);
        for (auto track: *m_song.get_tracks())
        {
            glm::vec4 color = m_colorArray[trackColorIndex];
            m_colorArray[trackColorIndex] += m_colorMutator[trackColorIndex];
            trackColorIndex++;
            obj.set_geometry(ORCore::create_rect_mesh(color));
            auto noteInfo = track.get_notes();//get_notes_in_frame(time, time+length);

            for(int i = noteInfo.start; i < noteInfo.end; i++)
            {
                auto &note = (*noteInfo.notes)[i];
                float z = note.time;

                float noteLength = note.length*m_boardSpeed;

                obj.set_scale(glm::vec3{0.005f, noteLength, 0.0f});
                obj.set_translation(glm::vec3{(static_cast<int>(note.noteValue)/(m_maxNotes*1.0)), -(z*m_boardSpeed), 0.0f}); // center the line on the screen

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
            if (m_fpsTime >= 2.0) {
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

        auto orthoCamera = m_renderer.get_camera(m_orthoCamera);
        orthoCamera->set_projection(glm::ortho(0.0f, static_cast<float>(1.0), static_cast<float>(1.0), 0.0f, -1.0f, 1.0f));
        m_renderer.update_camera(m_orthoCamera);
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

        auto orthoCamera = m_renderer.get_camera(m_orthoCamera);
        orthoCamera->set_translation(glm::vec3(0.0f, m_songTime*m_boardSpeed, 0.0f));
        m_renderer.update_camera(m_orthoCamera);

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
