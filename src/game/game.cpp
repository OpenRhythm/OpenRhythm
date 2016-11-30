#include "config.hpp"
#include "game.hpp"

#include <iostream>
#include <stdexcept>

#include "vfs.hpp"
namespace ORGame
{
    GameManager::GameManager()
    :m_width(800),
    m_height(600),
    m_fullscreen(false),
    m_title("OpenRhythm"),
    m_window(m_width, m_height, m_fullscreen, m_title),
    m_context(3, 2, 0),
    m_eventManager(),
    m_eventPump(&m_eventManager),
    m_song("/data/songs/testsong")
    {
        m_running = true;

        m_logger = spdlog::get("default");


        m_window.make_current(&m_context);

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

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        ORCore::ShaderInfo vertInfo {GL_VERTEX_SHADER, "./data/shaders/main.vs"};
        ORCore::ShaderInfo fragInfo {GL_FRAGMENT_SHADER, "./data/shaders/main.fs"};

        ORCore::Shader vertShader(vertInfo);
        ORCore::Shader fragShader(fragInfo);

        m_program = std::make_unique<ORCore::ShaderProgram>(&vertShader, &fragShader);

        m_program->check_error();
        m_program->use();
        m_orthoID = m_program->uniform_attribute("ortho");

        m_texture = std::make_unique<ORCore::Texture>("data/icon.png", m_program.get());

        std::cout << (m_width / 37) * (m_height / 37) << std::endl;

        m_ortho = glm::ortho(0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, -1.0f, 1.0f);

        m_program->set_uniform(m_orthoID, m_ortho);

        glClearColor(0.5, 0.5, 0.5, 1.0);
    }

    GameManager::~GameManager()
    {

        glDeleteVertexArrays(1, &m_vao);
        m_window.make_current(nullptr);

    }

    void GameManager::start()
    {

        while (m_running)
        {
            m_fpsTime += m_clock.tick();
            m_eventPump.process();

            update();
            render();

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
        m_ortho = glm::ortho(0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, -1.0f, 1.0f);
        m_program->set_uniform(m_orthoID, m_ortho);

        for (auto &mesh : m_meshes) {
            mesh.translate((m_width/2.0f)-256, (m_height/2.0f) - 4); // center the line on the screen
        }
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
                std::cout << "mouse x: " << ev.x << " mouse y" << ev.y << std::endl;
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
        m_meshes.clear();
        m_songTime = m_clock.get_current_time();
        prep_render_bars();
        prep_render_notes();

        for (auto &mesh : m_meshes) {
            mesh.update();
        }
    }

    void GameManager::prep_render_bars()
    {
        m_barsForRender = m_tempoTrack->get_events(m_songTime, m_songTime+2500.0, ORGame::EventType::Bar);
        for (size_t i = 0; i < m_barsForRender.size(); i++) {
            float z = ((m_barsForRender[i].bar->time - m_songTime) / 1000.0) * 225.0;

            ORCore::Mesh2D mesh(m_program.get(), m_texture.get());
            mesh.scale(512.0f, 4.0f);
            mesh.translate((m_width/2.0f)-256, 100 + z); // center the line on the screen

            m_meshes.push_back(mesh);
        }
    }

    void GameManager::prep_render_notes()
    {
        m_notesForRender = m_playerTrack->get_notes_in_frame(m_songTime, m_songTime+2500.0);
        for (size_t i = 0; i < m_notesForRender.size(); i++) {
            float z = ((m_notesForRender[i]->time - m_songTime) / 1000.0) * 225.0;

            ORCore::Mesh2D mesh(m_program.get(), m_texture.get());
            mesh.scale(30.0f, 4.0f);
            mesh.translate(static_cast<int>(m_notesForRender[i]->type)*40, 100 + z);

            m_meshes.push_back(mesh);
        }
    }


    void GameManager::render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_program->use();
        for (auto &mesh : m_meshes) {
            mesh.render();
        }


    }
}