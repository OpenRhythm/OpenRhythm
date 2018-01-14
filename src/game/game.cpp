// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "config.hpp"
#include "game.hpp"

#include <glad/glad.h>
#include <iostream>
#include <stdexcept>

#include "filesystem.hpp"
#include "resolver.hpp"
namespace ORGame
{
    const float neck_speed_divisor = 0.5;
    const float neck_board_length = 2.0f;

    const float hit_window_front = 0.085;
    const float hit_window_back = 0.085;

    GameManager::GameManager()
    :m_width(1920),
    m_height(1080),
    m_fullscreen(false),
    m_title("OpenRhythm"),
    m_context(3, 3, 8),
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

        // setup buttons
        for (auto i = 0; i < 5; i++)
        {
           m_buttons.push_back(0);
           m_buttonIsUpdate.push_back(false);
        }
        //m_buttons.resize(static_cast<int>(ORCore::GuitarController::MAX_BUTTON));


        m_fpsTime = 0.0;

        m_videoOffset = 0.015f;
        m_audioOffset = 0.0;


        // Store class instances into resolver.
        ORCore::Resolver::set(m_song);
        ORCore::Resolver::set(m_renderer);

        m_ss = std::cout.precision();
        m_renderer.init_gl();

        ORCore::ShaderInfo vertInfo {GL_VERTEX_SHADER, "./data/shaders/main.vs"};
        ORCore::ShaderInfo fragInfo {GL_FRAGMENT_SHADER, "./data/shaders/main.fs"};
        ORCore::ShaderInfo tailGeoShader {GL_GEOMETRY_SHADER, "./data/shaders/tail.gs"};

        ORCore::ShaderProgram program;
        program.add_shader(ORCore::Shader(vertInfo));
        program.add_shader(ORCore::Shader(fragInfo));
        program.link();
        m_program = m_renderer.add_program(std::move(program));

        ORCore::ShaderProgram tailProgram;
        //tailProgram.add_shader(ORCore::Shader(tailGeoShader));
        tailProgram.add_shader(ORCore::Shader(vertInfo));
        tailProgram.add_shader(ORCore::Shader(fragInfo));
        tailProgram.link();
        m_tailProgram = m_renderer.add_program(std::move(tailProgram));

        m_tailTexture = m_renderer.add_texture(ORCore::loadSTB("data/tail.png"));
        m_fretsTexture = m_renderer.add_texture(ORCore::loadSTB("data/frets.png"));

        // Create cameras and register them with the renderer
        ORCore::CameraObject camera;

        // the first parameter is the FOV, second is the aspect ratio, third is near plane fourth is the far plane
        // This is how I currently implement the cut off fretboard, later on this cutoff will be improved once I implement
        // framebuffers and can implement a proper neck fade out mask.
        camera.set_translation(glm::vec3(0.5f, 1.0f, -0.5));
        camera.set_rotation(45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        camera.set_uniform_name("proj");

        // The add_camera method copies the camera object internally so its simple to create multiple cameras
        // off of a single temporary object.
        // This static camera is what will be used for track
        m_cameraStatic = m_renderer.add_camera(camera);

        // This dynamic camera is what will be updated each frame in order to scroll the notes/beat lines.
        m_cameraDynamic = m_renderer.add_camera(camera);

        resize(m_width, m_height);

        ORCore::RenderObject obj;

        obj.set_camera(m_cameraStatic);

        // Background Neck
        obj.set_program(m_neckProgram);

        obj.set_primitive_type(ORCore::Primitive::triangle);
        obj.set_texture(m_neckTexture);

        obj.set_scale(glm::vec3{1.0f, 1.0f, neck_board_length});
        obj.set_translation(glm::vec3{0.0f, 0.0f, -0.35f});
        obj.set_geometry(ORCore::create_rect_z_mesh(glm::vec4{1.0f,1.0f,1.0f,1.0f}));
        m_neckObj = m_renderer.add_object(obj);

        // Solos
        obj.set_camera(m_cameraDynamic);
        obj.set_program(m_program);
        obj.set_texture(m_soloNeckTexture);

        auto &events = m_playerTrack->get_events();

        glm::vec4 solo_color = glm::vec4{0.0f,1.0f,1.0f,0.75f};
        for (auto &event : events)
        {

            if (event.type == EventType::solo) {
                float z = event.time / neck_speed_divisor;
                float length = event.length / neck_speed_divisor;

                obj.set_scale(glm::vec3{1.125f, 1.0f, length});
                obj.set_translation(glm::vec3{-0.0625f, 0.0f, z});
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

                obj.set_scale(glm::vec3{1.125f, 1.0f, length});
                obj.set_translation(glm::vec3{-0.0625f, 0.0f, z});
                obj.set_geometry(ORCore::create_rect_z_mesh(drive_color));
                m_renderer.add_object(obj);
            }
        }

        // Frets
        obj.set_camera(m_cameraStatic);
        obj.set_texture(m_fretsTexture);
        obj.set_scale(glm::vec3{1.0f, 1.0f, 0.05f});
        obj.set_translation(glm::vec3{0.0f, 0.0f, 0.0f}); // center the line on the screen
        obj.set_primitive_type(ORCore::Primitive::triangle);
        obj.set_geometry(ORCore::create_rect_z_center_mesh(glm::vec4{1.0f,1.0f,1.0f,1.0f}));
        m_fretObj = m_renderer.add_object(obj);

        // Top of frets
        obj.set_texture(-1);

        float laneWidth = 1.0f/5.0f;
        float objheight = laneWidth/3.0f;

        for (int i = 0; i < 5; i++)
        {
            obj.set_scale(glm::vec3{laneWidth, objheight/7.0f, objheight/2.5f});
            obj.set_translation(glm::vec3{i*laneWidth, 0.0f, 0.0f});
            obj.set_geometry(ORCore::create_cube_mesh(glm::vec4{1.0f,1.0f,1.0f,0.0f}));
            auto objID = m_renderer.add_object(obj);
            m_buttonRender.push_back(objID);
        }

        m_trackElements.init_bars(m_tempoTrack->get_bars());
        prep_render_notes();


        ORCore::RenderObject objLines;
        objLines.set_camera(m_cameraStatic);
        objLines.set_program(m_program);
        objLines.set_primitive_type(ORCore::Primitive::line);
        objLines.set_scale(glm::vec3{1.0f, 1.0f, 1.0f});
        objLines.set_translation(glm::vec3{0.0f, 0.0f, 0.0f});

        float hitwPosF = hit_window_front-m_videoOffset / neck_speed_divisor;
        float hitwPosB = (-hit_window_back)-m_videoOffset / neck_speed_divisor;

        glm::vec4 line_color = glm::vec4{1.0f,1.0f,1.0f,1.0f};

        objLines.set_geometry({
            // Vertex2           UV            Color
            {{0.0f, 0.0f, hitwPosF}, {0.0f, 0.0f}, line_color},
            {{1.0f, 0.0f, hitwPosF}, {0.0f, 0.0f}, line_color},

            {{0.0f, 0.0f, -(m_videoOffset/ neck_speed_divisor)}, {0.0f, 0.0f}, line_color},
            {{1.0f, 0.0f, -(m_videoOffset/ neck_speed_divisor)}, {0.0f, 0.0f}, line_color},

            {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, line_color},
            {{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, line_color},

            {{0.0f, 0.0f, hitwPosB}, {0.0f, 0.0f}, line_color},
            {{1.0f, 0.0f, hitwPosB}, {0.0f, 0.0f}, line_color}
        });

        m_renderer.add_object(objLines);



        ORCore::RenderObject objPoints;
        objPoints.set_camera(m_cameraStatic);
        objPoints.set_program(m_tailProgram);
        objPoints.set_primitive_type(ORCore::Primitive::point);
        objPoints.set_scale(glm::vec3{1.0f, 1.0f, 1.0f});
        objPoints.set_translation(glm::vec3{0.0f, 0.0f, 0.0f});

        // std::vector<ORCore::Vertex> points;

        // for (int i; i <=100; ++i)
        // {
        //     points.push_back({{0.5f, 0.0f, i * 0.001}, {0.0f, 0.0f}, line_color});
        // }

        objPoints.set_geometry({
            // Vertex2           UV            Color
            {{1.15f, 0.0f, 0.0f}, {0.0f, 0.0f}, line_color},
            {{1.15f, 0.0f, 0.01f}, {0.0f, 0.0f}, line_color},
            {{1.15f, 0.0f, 0.02f}, {0.0f, 0.0f}, line_color},
            {{1.15f, 0.0f, 0.03f}, {0.0f, 0.0f}, line_color},
            {{1.15f, 0.0f, 0.04f}, {0.0f, 0.0f}, line_color},
            {{1.15f, 0.0f, 0.05f}, {0.0f, 0.0f}, line_color},
        });

        m_renderer.add_object(objPoints);

        m_renderer.commit();

        auto neckProgramTemp = m_renderer.get_program(m_neckProgram);

        m_boardPosID = glGetUniformLocation(*neckProgramTemp, "neckPos");

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

    void GameManager::prep_render_notes()
    {
        std::vector<TrackNote> &notes = m_playerTrack->get_notes();
        std::cout << "Note Count: " << notes.size() << std::endl;

        // reuse the same container when creating notes as add_obj wont modify the original.
        ORCore::RenderObject obj;
        obj.set_camera(m_cameraDynamic);
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

            obj.set_scale(glm::vec3{tailWidth, 1.0f, noteLength});
            obj.set_translation(glm::vec3{(static_cast<int>(note.type)*noteWidth) - noteWidth+tailWidth, 0.0f, z}); // center the line on the screen
            obj.set_geometry(ORCore::create_rect_z_mesh(color));
            obj.set_texture(m_tailTexture);

            note.objTailID = m_renderer.add_object(obj);
            firstTailMarked = true;

            obj.set_texture(-1); // -1 gets set to the default texture.

            if (note.isHopo)
            {
                obj.set_scale(glm::vec3{noteWidth*0.75, tailWidth/2.5f, tailWidth/2.5f});
                obj.set_translation(glm::vec3{(static_cast<int>(note.type)*noteWidth) - (noteWidth*0.875), 0.0f, z}); // center the line on the screen
                obj.set_geometry(ORCore::create_cube_mesh(glm::vec4{1.0f,1.0f,1.0f,1.0f}));
            }
            else
            {
                obj.set_scale(glm::vec3{noteWidth, tailWidth/2.0f, tailWidth/2.0f});
                obj.set_translation(glm::vec3{(static_cast<int>(note.type)*noteWidth) - noteWidth, 0.0f, z}); // center the line on the screen
                obj.set_geometry(ORCore::create_cube_mesh(color));
            }

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

        auto cameraDynamic = m_renderer.get_camera(m_cameraDynamic);
        auto cameraStatic = m_renderer.get_camera(m_cameraStatic);

        cameraStatic->set_projection(glm::perspective(glm::radians(70.0f), m_width/static_cast<float>(m_height), 0.001f, 2.0f));
        cameraDynamic->set_projection(glm::perspective(glm::radians(70.0f), m_width/static_cast<float>(m_height), 0.001f, 2.0f));

        m_renderer.update_camera(m_cameraDynamic);
        m_renderer.update_camera(m_cameraStatic);
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
            case ORCore::GuitarControllerDown:
            {
                auto ev = ORCore::event_cast<ORCore::GuitarControllerDownEvent>(event);
                int index = static_cast<int>(ev.action);
                std::cout << "BUTT!!" << index << " " << m_buttons.size();
                m_buttons[index] = index+1;
                m_buttonIsUpdate[index] = true;
                break;
            }
            case ORCore::GuitarControllerUp:
            {
                auto ev = ORCore::event_cast<ORCore::GuitarControllerUpEvent>(event);
                int index = static_cast<int>(ev.action);
                std::cout << "FOO!!" << index;
                m_buttons[index] = 0;
                m_buttonIsUpdate[index] = true;
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

                    case ORCore::KeyCode::KEY_F11:
                        if (m_fullscreen)
                        {
                            m_fullscreen = false;
                            m_window.set_fullscreen(false);
                        }
                        else
                        {
                            m_fullscreen = true;
                            m_window.set_fullscreen(true);
                        }
                        break;
                    default:
                        std::cout << "Other Key " << std::endl;
                        break;
                }
            }
            default:
                break;
        }
        return true;
    }

    // void GameManager::guitar_button_up()
    // {

    // }

    void GameManager::update()
    {
        m_songTime = m_song.get_song_time(); 

        // Notes will effectively be hit m_videoOffset into the future so we need to go m_videoOffset into the past in order to get the proper notes.
        auto &notesInWindow = m_playerTrack->get_notes_in_frame((m_songTime-m_videoOffset)-hit_window_front, (m_songTime-m_videoOffset)+hit_window_back);
        
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
                [&](TrackNote *note)
                {
                    // Hide notes
                    auto *tailObj = m_renderer.get_object(note->objTailID);
                    tailObj->set_geometry(ORCore::create_rect_z_mesh(glm::vec4{1.0f,1.0f,1.0f,0.0f}));
                    m_renderer.update_object(note->objTailID);
                    return (note->time + note->length) <= m_songTime;
                }),
                m_heldNotes.end());

        for (int i = 0; i < m_buttons.size(); ++i)
        {
            if (m_buttons[i] != 0)
            {
                if (m_buttonIsUpdate[i])
                {
                    auto *button = m_renderer.get_object(m_buttonRender[i]);

                    button->set_geometry(ORCore::create_cube_mesh(glm::vec4{1.0f,1.0f,1.0f,0.7f}));
                    m_renderer.update_object(m_buttonRender[i]);

                    m_buttonIsUpdate[i] = false;
                }
            }
            else
            {
                if (m_buttonIsUpdate[i])
                {
                    auto *button = m_renderer.get_object(m_buttonRender[i]);
                    button->set_geometry(ORCore::create_cube_mesh(glm::vec4{1.0f,1.0f,1.0f,0.0f}));
                    m_renderer.update_object(m_buttonRender[i]);
                    m_buttonIsUpdate[i] = false;
                }
            }
        }

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
            float noteLength = (note->length - (m_songTime-note->time)) / neck_speed_divisor;

            tailObj->set_scale(glm::vec3{tailWidth, 1.0f, noteLength});
            tailObj->set_translation(glm::vec3{(static_cast<int>(note->type)*noteWidth) - noteWidth+tailWidth, 0.0f, z}); // center the line on the screen

            tailObj->set_geometry(ORCore::create_rect_z_mesh(color));

            m_renderer.update_object(note->objTailID);
        }

        m_renderer.commit();

        // translate projection with song
        auto cam = m_renderer.get_camera(m_cameraDynamic);
        cam->set_translation(glm::vec3(0.5f, 1.0f, boardPos-0.5));
        m_renderer.update_camera(m_cameraDynamic);
    }

    void GameManager::render()
    {
        glClear(GL_COLOR_BUFFER_BIT);
        m_renderer.render();
    }
}
