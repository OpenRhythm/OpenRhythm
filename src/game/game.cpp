// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "config.hpp"
#include "game.hpp"

#include <glad/glad.h>
#include <iostream>
#include <stdexcept>

#include "filesystem.hpp"
namespace ORGame
{
    const float neck_speed_divisor = 0.5;
    const float neck_board_length = 2.0f;

    GameManager::GameManager()
    :m_width(1920),
    m_height(1080),
    m_fullscreen(false),
    m_title("OpenRhythm"),
    m_context(3, 2, 8),
    m_window(m_width, m_height, m_fullscreen, m_title),
    m_eventManager(),
    m_eventPump(&m_eventManager),
    m_song("data/songs/testsong")
    {

        m_running = true;

        m_logger = spdlog::get("default");

        m_window.make_current(&m_context);

        m_window.disable_sync();

    #if OSX_APP_BUNDLE
        m_logger->info("OSX app path: {}", ORCore::get_app_path());
    #endif
        auto basePath = ORCore::get_path_contents(ORCore::get_base_path());
        m_logger->info("Game root info: {} {}", ORCore::get_base_path(), basePath.size());
        m_logger->info("Home path: {}", ORCore::get_home_path());

        m_song.load();

        m_tempoTrack = m_song.get_tempo_track();

        m_song.load_tracks();
        m_playerTrack = &(*m_song.get_tracks())[0];

        if (!gladLoadGL())
        {
            throw std::runtime_error(_("Error: GLAD failed to load."));
        }

        m_lis.handler = std::bind(&GameManager::event_handler, this, std::placeholders::_1);
        m_lis.mask = ORCore::EventType::EventAll;


        m_eventManager.add_listener(m_lis);

        m_fpsTime = 0.0;

        m_videoOffset = 0.015;
        m_audioOffset = 0.0;

        m_ss = std::cout.precision();
        m_renderer.init_gl();

        ORCore::ShaderInfo vertInfo {GL_VERTEX_SHADER, "./data/shaders/main.vs"};
        ORCore::ShaderInfo fragInfo {GL_FRAGMENT_SHADER, "./data/shaders/main.fs"};
        ORCore::ShaderInfo neckVertInfo {GL_VERTEX_SHADER, "./data/shaders/neck.vs"};

        m_program = m_renderer.add_program(ORCore::Shader(vertInfo), ORCore::Shader(fragInfo));
        m_neckProgram = m_renderer.add_program(ORCore::Shader(neckVertInfo), ORCore::Shader(fragInfo));

        m_texture = m_renderer.add_texture(ORCore::loadSTB("data/icon.png"));
        m_tailTexture = m_renderer.add_texture(ORCore::loadSTB("data/tail.png"));
        m_fretsTexture = m_renderer.add_texture(ORCore::loadSTB("data/frets.png"));
        m_soloNeckTexture = m_renderer.add_texture(ORCore::loadSTB("data/soloNeck.png"));
        m_neckTexture = m_renderer.add_texture(ORCore::loadSTB("data/neck.png"));

        resize(m_width, m_height);

        ORCore::RenderObject obj;

        // Background Neck
        obj.set_program(m_neckProgram);

        obj.set_primitive_type(ORCore::Primitive::triangle);
        obj.set_texture(m_neckTexture);

        obj.set_scale(glm::vec3{1.0f, 1.0f, -neck_board_length});
        obj.set_translation(glm::vec3{0.0f, 0.0f, 0.0f});
        obj.set_geometry(ORCore::create_rect_z_mesh(glm::vec4{1.0f,1.0f,1.0f,1.0f}));
        m_neckObj = m_renderer.add_object(obj);


        // Solos
        obj.set_program(m_program);
        obj.set_texture(m_soloNeckTexture);

        auto &events = m_playerTrack->get_events();

        glm::vec4 solo_color = glm::vec4{0.0f,1.0f,1.0f,0.75f};
        for (auto &event : events)
        {

            if (event.type == EventType::solo) {
                float z = event.time / neck_speed_divisor;
                float length = event.length / neck_speed_divisor;

                obj.set_scale(glm::vec3{1.125f, 1.0f, -length});
                obj.set_translation(glm::vec3{-0.0625f, 0.0f, -z});
                obj.set_geometry(ORCore::create_rect_z_mesh(solo_color));
                m_renderer.add_object(obj);
            }
        }

        // drive
        glm::vec4 drive_color = glm::vec4{1.5f,1.5f,1.5f,0.75f};

        for (auto &event : events)
        {
            if (event.type == EventType::drive)
            {
                float z = event.time / neck_speed_divisor;
                float length = event.length / neck_speed_divisor;

                obj.set_scale(glm::vec3{1.125f, 1.0f, -length});
                obj.set_translation(glm::vec3{-0.0625f, 0.0f, -z});
                obj.set_geometry(ORCore::create_rect_z_mesh(drive_color));
                m_renderer.add_object(obj);
            }
        }

        // Frets
        obj.set_texture(m_fretsTexture);
        obj.set_scale(glm::vec3{1.0f, 1.0f, 0.05f});
        obj.set_translation(glm::vec3{0.0f, 0.0f, -1.0f}); // center the line on the screen
        obj.set_primitive_type(ORCore::Primitive::triangle);
        obj.set_geometry(ORCore::create_rect_z_center_mesh(glm::vec4{1.0f,1.0f,1.0f,1.0f}));
        m_fretObj = m_renderer.add_object(obj);

        prep_render_bars();

        prep_render_notes();

        m_renderer.commit();

        GLint  iMultiSample = 0;
        GLint  iNumSamples = 0;
        glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
        glGetIntegerv(GL_SAMPLES, &iNumSamples);

        m_logger->info("GL_SAMPLE_BUFFERS: {}, GL_SAMPLES: {} ", iMultiSample, iNumSamples);

        glClearColor(0.5, 0.5, 0.5, 1.0);
        m_song.start();
    }

    GameManager::~GameManager()
    {
        m_window.make_current(nullptr);
    }

    void GameManager::prep_render_bars()
    {

        std::vector<BarEvent> &bars = m_tempoTrack->get_bars();
        std::cout << "Bar Count: " << bars.size() << " Song Length: " << m_song.length() << std::endl;

        // reuse the same container when creating bars as add_obj wont modify the original.
        ORCore::RenderObject obj;
        obj.set_texture(m_texture);
        obj.set_program(m_program);

        for (size_t i = 0; i < bars.size(); i++) {
            float z = (bars[i].time / neck_speed_divisor);

            if (bars[i].type == BarType::measure)
            {
                obj.set_scale(glm::vec3{1.0f, 1.0f, 0.021});
            }
            else if (bars[i].type == BarType::upbeat)
            {
                obj.set_scale(glm::vec3{1.0f, 1.0f, 0.003});
            }
            else 
            {
                obj.set_scale(glm::vec3{1.0f, 1.0f, 0.01});
            }

            obj.set_translation(glm::vec3{0.0, 0.0f, -z}); // center the line on the screen
            obj.set_primitive_type(ORCore::Primitive::triangle);
            obj.set_geometry(ORCore::create_rect_z_center_mesh(glm::vec4{1.0f,1.0f,1.0f,1.0f}));

            m_renderer.add_object(obj);
        }
    }

    void GameManager::prep_render_notes()
    {
        std::vector<TrackNote> &notes = m_playerTrack->get_notes();
        std::cout << "Note Count: " << notes.size() << std::endl;

        // reuse the same container when creating notes as add_obj wont modify the original.
        ORCore::RenderObject obj;
        obj.set_program(m_program);
        obj.set_primitive_type(ORCore::Primitive::triangle);

        float noteWidth = 1.0f/5.0f;
        float tailWidth = noteWidth/3.0f;

        bool firstTailMarked = false;

        for (auto &note : notes)
        {
            float z = note.time / neck_speed_divisor;
            glm::vec4 color;
            try
            {
                color = noteColorMap.at(note.type);
            }
            catch (std::out_of_range &err) {
                color = glm::vec4{1.0f,1.0f,1.0f,1.0f};
            }

            float noteLength = note.length/neck_speed_divisor;

            obj.set_scale(glm::vec3{tailWidth, 1.0f, -noteLength});
            obj.set_translation(glm::vec3{(static_cast<int>(note.type)*noteWidth) - noteWidth+tailWidth, 0.0f, -z}); // center the line on the screen
            obj.set_geometry(ORCore::create_rect_z_mesh(color));
            obj.set_texture(m_tailTexture);

            note.objTailID = m_renderer.add_object(obj);
            firstTailMarked = true;

            obj.set_texture(-1); // -1 gets set to the default texture.

            obj.set_scale(glm::vec3{noteWidth, tailWidth/2.0f, tailWidth/2.0f});
            obj.set_translation(glm::vec3{(static_cast<int>(note.type)*noteWidth) - noteWidth, 0.0f, -z}); // center the line on the screen
            obj.set_geometry(ORCore::create_cube_mesh(color));

            note.objNoteID = m_renderer.add_object(obj);

        }
    }

    void GameManager::start()
    {
        while (m_running)
        {
            m_fpsTime += m_clock.tick();
            m_eventPump.process();

            update();
            render();


            m_renderer.check_error();

            m_window.flip();
            if (m_fpsTime >= 2.0) {
                std::cout.precision (5);
                std::cout << "FPS: " << m_clock.get_fps() << std::endl;
                std::cout << "Song Time: " << m_songTime << std::endl;
                std::cout << "Audio Time: " << m_song.get_audio_time() << std::endl;
                std::cout.precision (m_ss);
                m_fpsTime = 0;
            }
        }
    }

    void GameManager::resize(int width, int height)
    {
        m_width = width;
        m_height = height;
        glViewport(0, 0, m_width, m_height);
        m_ortho = glm::ortho(0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, -1.0f, 2.0f);
        m_perspective = glm::perspective(glm::radians(70.0f), m_width/static_cast<float>(m_height), 0.001f, 2.0f);
        m_rotPerspective = glm::rotate(m_perspective, glm::radians(45.0f), glm::vec3(1.0f,0.0f,0.0f));
    }

    bool GameManager::event_handler(const ORCore::Event &event)
    {
        switch (event.type)
        {
            case ORCore::Quit:
            {
                m_running = false;
                break;
            }
            case ORCore::MouseMove:
            {
                auto ev = ORCore::event_cast<ORCore::MouseMoveEvent>(event);
                //std::cout << "mouse x: " << ev.x << " mouse y" << ev.y << std::endl;
                m_mouseX = ev.x;
                m_mouseY = ev.y;
                break;
            }
            case ORCore::WindowSize:
            {
                auto ev = ORCore::event_cast<ORCore::WindowSizeEvent>(event);
                resize(ev.width, ev.height);
                break;
            }
            case ORCore::KeyDown: {
                auto ev = ORCore::event_cast<ORCore::KeyDownEvent>(event);
                switch(ev.key)
                {
                    case ORCore::KeyCode::KEY_F:
                        std::cout << "Key F" << std::endl;
                        break;
                    case ORCore::KeyCode::KEY_P:
                        m_song.set_pause(true);
                        m_heldNotes.clear();
                        break;
                    case ORCore::KeyCode::KEY_R:
                        m_song.set_pause(false);
                        break;
                    default:
                        std::cout << "Other Key" << std::endl;
                        break;
                }
            }
            default:
                break;
        }
        return true;
    }

    void GameManager::update()
    {
        m_songTime = m_song.get_song_time(); 

        // Notes will effectively be hit m_videoOffset into the future so we need to go m_videoOffset into the past in order to get the proper notes.
        auto &notesInWindow = m_playerTrack->get_notes_in_frame((m_songTime-m_videoOffset)-0.100, (m_songTime-m_videoOffset)+0.100);
        
        glm::vec4 color;

        for (auto *note : notesInWindow)
        {
            if (!note->played && note->time + m_videoOffset <= m_songTime)
            {

                try
                {
                    color = noteColorMapActive.at(note->type);
                }
                catch (std::out_of_range &err)
                {
                    color = glm::vec4{1.0f,1.0f,1.0f,1.0f};
                }
                color[3] = 0.0f; // Dissapear notes

                auto *noteObj = m_renderer.get_object(note->objNoteID);
                noteObj->set_geometry(ORCore::create_cube_mesh(color));
                m_renderer.update_object(note->objNoteID);

                note->played = true;
                m_heldNotes.push_back(note);
            }
        }


        // Todo - do we need to think about video cal here if we are talking about scoring?
        m_heldNotes.erase(std::remove_if(m_heldNotes.begin(), m_heldNotes.end(),
                [&](TrackNote *note){return (note->time + note->length) <= m_songTime;}),
                m_heldNotes.end());

        for (auto *note : m_heldNotes)
        {
            try
            {
                color = noteColorMapActive.at(note->type);
            }
            catch (std::out_of_range &err)
            {
                color = glm::vec4{1.0f,1.0f,1.0f,1.0f};
            }

            auto *tailObj = m_renderer.get_object(note->objTailID);

            float noteWidth = 1.0f/5.0f;
            float tailWidth = noteWidth/3.0f;

            float z = m_songTime / neck_speed_divisor;
            float noteLength = (note->length - (m_songTime-note->time))/neck_speed_divisor;

            tailObj->set_scale(glm::vec3{tailWidth, 1.0f, -noteLength});
            tailObj->set_translation(glm::vec3{(static_cast<int>(note->type)*noteWidth) - noteWidth+tailWidth, 0.0f, -z}); // center the line on the screen
            tailObj->set_geometry(ORCore::create_rect_z_mesh(color));

            m_renderer.update_object(note->objTailID);
        }

        float boardPos = (m_songTime/neck_speed_divisor);

        // Static aways in view objects to be transformed with the camera
        auto frets = m_renderer.get_object(m_fretObj);
        frets->set_translation(glm::vec3(0.0f, 0.0f, -boardPos));
        m_renderer.update_object(m_fretObj);

        auto neck = m_renderer.get_object(m_neckObj);
        neck->set_translation(glm::vec3(0.0f, 0.0f, -boardPos + 0.35f));
        m_renderer.update_object(m_neckObj);

        m_renderer.commit();

        // TODO - Allow renderer to be able to specify uniforms and set them per batch/shader
        auto neckProgram = m_renderer.get_program(m_neckProgram);

        neckProgram->use();
        
        // TODO - FIX ME No gl calls outside of renderer.
        glUniform1f(
            glGetUniformLocation(*neckProgram, "neckPos"),
            boardPos/neck_board_length);

        neckProgram->disuse();

        // m_renderer.set_camera_transform("ortho", glm::translate(m_ortho, glm::vec3(0.0f, 1.0f, (-m_songTime)/3.0f))); // translate projection with song
        m_renderer.set_camera_transform("ortho", glm::translate(m_rotPerspective, glm::vec3(-0.5f, -1.0f, boardPos-0.5))); // translate projection with song

    }

    void GameManager::render()
    {
        glClear(GL_COLOR_BUFFER_BIT);
        m_renderer.render();
    }
}
