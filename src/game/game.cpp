#include <iostream>
#include <functional>
#include <utility>
#include <stdexcept>

#include "config.hpp"
#include "vfs.hpp"
#include "game.hpp"

GameManager::GameManager()
: m_song("/data/songs/testsong")
{
    m_width = 800;
    m_height = 600;
    m_fullscreen = false;
    m_title = "Game";

    m_window = std::make_unique<MgCore::Window>(m_width, m_height, m_fullscreen, m_title);
    m_context = std::make_unique<MgCore::Context>(3, 2, 0);
    m_events = std::make_unique<MgCore::Events>();
    m_clock = std::make_unique<MgCore::FpsTimer>();
    m_running = true;

    m_logger = spdlog::get("default");


    m_window->make_current(m_context.get());

    //VFS.AddLoader(new ttvfs::DiskLoader);

    //
    // AppPath gets mounted on osx
    // BasePath gets mounted and overwrites any files similar to those in AppPath (data)
    // HomePath gets mounted and overwrites any files similar to those in BasePath (configs)
    // std::cout << MgCore::GetBasePath() << std::endl;
#if OSX_APP_BUNDLE
    MgCore::mount(MgCore::GetAppPath(), "");
#endif
    MgCore::mount(MgCore::GetBasePath(), "/bob");
    std::vector<std::string> paths = MgCore::resolveSystemPath("/bob");
    for (auto i: paths) {
        auto bob = MgCore::sysGetPathContents(i);
        std::cout << i << " " << bob.size() << std::endl;
    }
    //VFS.Mount( MgCore::GetBasePath().c_str(), "" );
    //VFS.Mount( MgCore::GetHomePath().c_str(), "" );

    //MgCore::mount( "./data", "data" );

    m_song.add(MgGame::TrackType::Guitar, MgGame::Difficulty::Expert);

    m_song.load();

    //std::cout << "Song: " << (m_song.length() / 1000) / 60 << " minutes long" << std::endl;

    //MgCore::Track *track = m_song.getTrack( MgCore::TrackType::Guitar, MgCore::Difficulty::Expert );
    //std::cout << "Song: loaded track for " << MgCore::TrackNameForType( track->info().type ) << std::endl;

    m_tempoTrack = m_song.getTempoTrack();

    //std::vector<MgCore::TrackNote*> v = track->getNotesInFrame(0, 10000);

    //std::cout << "Song: " << v.size() << " notes in first 10 seconds, first note is " << NoteNameForType(v[0]->type()) << std::endl;

    if(!gladLoadGL())
    {
        throw std::runtime_error("Error: GLAD failed to load.");
    }

    m_lis.handler = std::bind(&GameManager::event_handler, this, std::placeholders::_1);
    m_lis.mask = MgCore::EventType::EventAll;


    m_events->add_listener(m_lis);

    m_fpsTime = 0.0;

    m_ss = std::cout.precision();

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    MgCore::ShaderInfo vertInfo {GL_VERTEX_SHADER, "./data/shaders/main.vs"};
    MgCore::ShaderInfo fragInfo {GL_FRAGMENT_SHADER, "./data/shaders/main.fs"};

    MgCore::Shader vertShader(vertInfo);
    MgCore::Shader fragShader(fragInfo);

    m_program = std::make_unique<MgCore::ShaderProgram>(&vertShader, &fragShader);

    m_program->check_error();
    m_program->use();
    m_orthoID = m_program->uniform_attribute("ortho");

    m_texture = std::make_unique<MgCore::Texture>("data/icon.png", m_program.get());

    std::cout << (m_width / 37) * (m_height / 37) << std::endl;


    MeshPtr mesh = std::make_unique<MgCore::Mesh2D>(m_program.get(), m_texture.get());
    mesh->scale(512.0f, 8.0f);
    mesh->translate((m_width/2.0f)-256, (m_height/2.0f) - 4); // center the line on the screen
    m_meshes.push_back(std::move(mesh));


    m_ortho = glm::ortho(0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, -1.0f, 1.0f);

    m_program->set_uniform(m_orthoID, m_ortho);

    glClearColor(0.5, 0.5, 0.5, 1.0);
}

GameManager::~GameManager()
{

    glDeleteVertexArrays(1, &m_vao);
    m_window->make_current(nullptr);

}

void GameManager::start()
{

    while (m_running)
    {
        m_fpsTime += m_clock->tick();
        m_events->process();

        update();
        render();

        m_window->flip();
        if (m_fpsTime >= 2.0) {
            std::cout.precision (5);
            std::cout << "FPS: " << m_clock->get_fps() << std::endl;
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
        mesh->translate((m_width/2.0f)-256, (m_height/2.0f) - 4); // center the line on the screen
    }
}

bool GameManager::event_handler(MgCore::Event &event)
{
    MgCore::EventType type = event.type;
    switch(type) {
        case MgCore::EventType::Quit:
            m_running = false;
            break;
        case MgCore::EventType::MouseMove:
            m_mouseX = event.event.mouseMove.x;
            m_mouseY = event.event.mouseMove.y;
            break;
        case MgCore::EventType::WindowSized:
            resize(event.event.windowSized.width, event.event.windowSized.height);
            break;
        default:
            break;

    }
    return true;
}

void GameManager::handle_song()
{
    double songTime = m_clock->get_current_time();
    std::vector<MgGame::TempoEvent*> temoChanges;
    temoChanges = m_tempoTrack->getEventsInFrame(songTime-5.0, songTime+5.0);
    //m_logger->info("Time: {}", songTime);

    for (auto i : temoChanges)
    {
        m_logger->info("Tempo change at: {}", i->time);
    }
}

void GameManager::update()
{
    for (auto& mesh : m_meshes) {
        mesh->update();
    }
    handle_song();
}

void GameManager::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->use();
    for (auto& mesh : m_meshes) {
        mesh->render();
    }

}
