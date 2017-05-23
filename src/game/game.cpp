#include "config.hpp"
#include "game.hpp"

#include <iostream>
#include <stdexcept>

#include "vfs.hpp"
namespace ORGame
{
    const float neck_speed_divisor = 0.5;

    GameManager::GameManager()
    :m_width(800),
    m_height(600),
    m_fullscreen(false),
    m_title("OpenRhythm"),
    m_context(3, 2, 8),
    m_window(m_width, m_height, m_fullscreen, m_title),
    m_eventManager(),
    m_eventPump(&m_eventManager),
    m_song("/data/songs/testsong")
    {
        m_running = true;

        m_logger = spdlog::get("default");


        m_window.make_current(&m_context);

        m_window.disable_sync();

        //VFS.AddLoader(new ttvfs::DiskLoader);

        //
        // AppPath gets mounted on osx
        // BasePath gets mounted and overwrites any files similar to those in AppPath (data)
        // HomePath gets mounted and overwrites any files similar to those in BasePath (configs)
        // std::cout << ORCore::GetBasePath() << std::endl;
    #if OSX_APP_BUNDLE
        ORCore::mount(ORCore::GetAppPath(), "");
    #endif
        ORCore::mount(ORCore::GetBasePath(), "/bob");
        std::vector<std::string> paths = ORCore::resolveSystemPath("/bob");
        for (auto &i: paths) {
            auto bob = ORCore::sysGetPathContents(i);
            std::cout << i << " " << bob.size() << std::endl;
        }
        //VFS.Mount( ORCore::GetBasePath().c_str(), "" );
        //VFS.Mount( ORCore::GetHomePath().c_str(), "" );

        //ORCore::mount( "./data", "data" );
        m_song.load();

        //std::cout << "Song: " << (m_song.length() / 1000) / 60 << " minutes long" << std::endl;

        //ORCore::Track *track = m_song.getTrack( ORCore::TrackType::Guitar, ORCore::Difficulty::Expert );
        //std::cout << "Song: loaded track for " << ORCore::TrackNameForType( track->info().type ) << std::endl;

        m_tempoTrack = m_song.get_tempo_track();

        m_song.load_tracks();
        m_playerTrack = &(*m_song.get_tracks())[0];

        if(!gladLoadGL())
        {
            throw std::runtime_error(_("Error: GLAD failed to load."));
        }

        m_lis.handler = std::bind(&GameManager::event_handler, this, std::placeholders::_1);
        m_lis.mask = ORCore::EventType::EventAll;


        m_eventManager.add_listener(m_lis);

        m_fpsTime = 0.0;

        m_ss = std::cout.precision();
        m_renderer.init_gl();

        ORCore::ShaderInfo vertInfo {GL_VERTEX_SHADER, "./data/shaders/main.vs"};
        ORCore::ShaderInfo fragInfo {GL_FRAGMENT_SHADER, "./data/shaders/main.fs"};

        m_program = m_renderer.add_program(ORCore::Shader(vertInfo), ORCore::Shader(fragInfo));
        m_texture = m_renderer.add_texture(ORCore::loadSTB("data/icon.png"));
        m_tailTexture = m_renderer.add_texture(ORCore::loadSTB("data/tail.png"));
        m_fretsTexture = m_renderer.add_texture(ORCore::loadSTB("data/frets.png"));
        m_soloNeckTexture = m_renderer.add_texture(ORCore::loadSTB("data/soloNeck.png"));

        resize(m_width, m_height);
        // glEnable(GL_DEPTH_TEST);

        ORCore::RenderObject obj;
        obj.set_program(m_program);
        obj.set_primitive_type(ORCore::Primitive::triangle);
        obj.set_texture(m_soloNeckTexture);

        auto *events = m_playerTrack->get_events();

        // Solos
        glm::vec4 solo_color = glm::vec4{0.0f,1.0f,1.0f,0.75f};
        for (auto &event : *events)
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

        for (auto &event : *events)
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

        prep_render_bars();

        prep_render_notes();

        obj.set_texture(m_fretsTexture);
        obj.set_scale(glm::vec3{1.0f, 1.0f, 0.05f});
        obj.set_translation(glm::vec3{0.0f, 0.0f, -1.0f}); // center the line on the screen
        obj.set_primitive_type(ORCore::Primitive::triangle);
        obj.set_geometry(ORCore::create_rect_z_center_mesh(glm::vec4{1.0f,1.0f,1.0f,1.0f}));
        m_fretObj = m_renderer.add_object(obj);

        m_renderer.commit();

        GLint  iMultiSample = 0;
        GLint  iNumSamples = 0;
        glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
        glGetIntegerv(GL_SAMPLES, &iNumSamples);

        m_logger->info("GL_SAMPLE_BUFFERS: {}, GL_SAMPLES: {} ", iMultiSample, iNumSamples);

        glClearColor(0.5, 0.5, 0.5, 1.0);
    }

    GameManager::~GameManager()
    {
        m_window.make_current(nullptr);
    }

    void GameManager::prep_render_bars()
    {

        std::vector<BarEvent*> bars = m_tempoTrack->get_bars();
        std::cout << "Bar Count: " << bars.size() << " Song Length: " << m_song.length() << std::endl;

        // reuse the same container when creating bars as add_obj wont modify the original.
        ORCore::RenderObject obj;
        obj.set_texture(m_texture);
        obj.set_program(m_program);

        for (size_t i = 0; i < bars.size(); i++) {
            float z = (bars[i]->time / neck_speed_divisor);

            obj.set_scale(glm::vec3{1.0f, 1.0f, 0.007});
            obj.set_translation(glm::vec3{0.0, 0.0f, -z}); // center the line on the screen
            obj.set_primitive_type(ORCore::Primitive::triangle);
            obj.set_geometry(ORCore::create_rect_z_center_mesh(glm::vec4{1.0f,1.0f,1.0f,1.0f}));

            m_renderer.add_object(obj);
        }
    }

    void GameManager::prep_render_notes()
    {
        std::vector<TrackNote*> notes = m_playerTrack->get_notes();
        std::cout << "Note Count: " << notes.size() << std::endl;

        // reuse the same container when creating notes as add_obj wont modify the original.
        ORCore::RenderObject obj;
        obj.set_program(m_program);

        float noteWidth = 1.0f/5.0f;
        float tailWidth = noteWidth/3.0f;

        for (auto &note : notes)
        {
            float z = note->time / neck_speed_divisor;
            glm::vec4 color;
            try {
                color = noteColorMap.at(note->type);
            } catch (std::out_of_range &err) {
                color = glm::vec4{1.0f,1.0f,1.0f,1.0f};
            }

            float noteLength = note->length/neck_speed_divisor;

            obj.set_scale(glm::vec3{tailWidth, 1.0f, -noteLength});
            obj.set_translation(glm::vec3{(static_cast<int>(note->type)*noteWidth) - noteWidth+tailWidth, 0.0f, -z}); // center the line on the screen
            obj.set_primitive_type(ORCore::Primitive::triangle);
            obj.set_geometry(ORCore::create_rect_z_mesh(color));
            obj.set_texture(m_tailTexture);

            note->objTailID = m_renderer.add_object(obj);
            obj.set_texture(-1); // -1 gets set to the default texture.

            obj.set_scale(glm::vec3{noteWidth, tailWidth/2.0f, tailWidth/2.0f});
            obj.set_translation(glm::vec3{(static_cast<int>(note->type)*noteWidth) - noteWidth, 0.0f, -z}); // center the line on the screen
            obj.set_primitive_type(ORCore::Primitive::triangle);
            obj.set_geometry(ORCore::create_cube_mesh(color));

            note->objNoteID = m_renderer.add_object(obj);

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
            if (m_fpsTime >= 2000.0) {
                std::cout.precision (5);
                std::cout << "FPS: " << m_clock.get_fps() << std::endl;
                std::cout << "Song Time: " << m_songTime << std::endl;
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
        switch(event.type) {
            case ORCore::Quit: {
                m_running = false;
                break;
            }
            case ORCore::MouseMove: {
                auto ev = ORCore::event_cast<ORCore::MouseMoveEvent>(event);
                //std::cout << "mouse x: " << ev.x << " mouse y" << ev.y << std::endl;
                m_mouseX = ev.x;
                m_mouseY = ev.y;
                break;
            }
            case ORCore::WindowSize: {
                auto ev = ORCore::event_cast<ORCore::WindowSizeEvent>(event);
                resize(ev.width, ev.height);
                break;
            }
            case ORCore::KeyDown: {
                auto ev = ORCore::event_cast<ORCore::KeyDownEvent>(event);
                switch(ev.key) {
                    case ORCore::KeyCode::KEY_F:
                        std::cout << "Key F" << std::endl;
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

    void GameManager::handle_song()
    {
    }

    void GameManager::update()
    {
        // TODO - move songtime to song class, and create a new timer type which can be started and stopped/paused/rewound etc\.
        m_songTime = m_clock.get_current_time()/1000.0;

        auto notesInWindow = m_playerTrack->get_notes_in_frame(m_songTime-0.020, m_songTime+0.100);
        for (auto *note : notesInWindow)
        {
            if (!note->played && note->time <= m_songTime)
            {
                auto *tailObj = m_renderer.get_object(note->objTailID);
                auto *noteObj = m_renderer.get_object(note->objNoteID);

                glm::vec4 color;
                try {
                    color = noteColorMapActive.at(note->type);
                } catch (std::out_of_range &err) {
                    color = glm::vec4{1.0f,1.0f,1.0f,1.0f};
                }

                tailObj->set_geometry(ORCore::create_rect_z_mesh(color));
                // noteObj->set_geometry(ORCore::create_cube_mesh(color));
                //tailObj->set_scale(glm::vec3(1.0f,1.0f,1.0f));
                //noteObj->set_scale(glm::vec3(1.0f,1.0f,1.0f));

                m_renderer.update_object(note->objTailID);
                m_renderer.update_object(note->objNoteID);
                note->played = true;
            }
        }

        auto frets = m_renderer.get_object(m_fretObj);

        frets->set_translation(glm::vec3(0.0f, 0.0f, -(m_songTime/neck_speed_divisor)));

        m_renderer.update_object(m_fretObj);

        m_renderer.commit();

        // m_renderer.set_camera_transform("ortho", glm::translate(m_ortho, glm::vec3(0.0f, 1.0f, (-m_songTime)/3.0f))); // translate projection with song
        m_renderer.set_camera_transform("ortho", glm::translate(m_rotPerspective, glm::vec3(-0.5f, -1.0f, (m_songTime/neck_speed_divisor)-0.5))); // translate projection with song

    }

    void GameManager::render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_renderer.render();
    }
}