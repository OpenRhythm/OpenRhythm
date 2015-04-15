#include <iostream>
#include <functional>
#include <utility>

#include "config.hpp"
#include "parser.hpp"
#include "vfs.hpp"
#include "game.hpp"

GameManager::GameManager()
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

    m_window->make_current(m_context.get());

    VFS.AddLoader(new ttvfs::DiskLoader);

    //
    // AppPath gets mounted on osx
    // BasePath gets mounted and overwrites any files similar to those in AppPath (data)
    // HomePath gets mounted and overwrites any files similar to those in BasePath (configs)
    // std::cout << MgCore::GetBasePath() << std::endl;
#if OSX_APP_BUNDLE
    VFS.Mount( MgCore::GetAppPath().c_str(), "" );
#endif
    VFS.Mount( MgCore::GetBasePath().c_str(), "" );
    VFS.Mount( MgCore::GetHomePath().c_str(), "" );

    VFS.Mount( "data", "" );

    MgCore::Song song( "songs/testsong" ); // dirname of song

    song.add( MgCore::TrackType::Drums, MgCore::Difficulty::Hard );

    song.load();

    std::cout << "Song: " << (song.length() / 1000) / 60 << " minutes long" << std::endl;

    MgCore::Track *track = song.getTrack( MgCore::TrackType::Drums, MgCore::Difficulty::Hard );
    std::cout << "Song: loaded track for " << MgCore::TrackNameForType( track->info().type ) << std::endl;

//    track->listNotesInTrack();

    std::vector<MgCore::TrackNote*> v = track->getNotesInFrame(0, 10000);

    std::cout << "Song: " << v.size() << " notes in first 10 seconds, first note is " << NoteNameForType(v[0]->type()) << std::endl;

    if(ogl_LoadFunctions() == ogl_LOAD_FAILED)
    {
        std::cout << "Error: glLoadGen failed to load.";
    }
    
    m_lis.handler = std::bind(&GameManager::event_handler, this, std::placeholders::_1);
    m_lis.mask = MgCore::EventType::EventAll;


    m_events->add_listener(m_lis);

    m_fpsTime = 0.0;

    m_ss = std::cout.precision();

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    MgCore::ShaderInfo vertInfo {GL_VERTEX_SHADER, "shaders/main.vs"};
    MgCore::ShaderInfo fragInfo {GL_FRAGMENT_SHADER, "shaders/main.fs"};

    MgCore::Shader vertShader(vertInfo);
    MgCore::Shader fragShader(fragInfo);

    m_program = std::make_unique<MgCore::ShaderProgram>(&vertShader, &fragShader);
    m_program->use();
    m_orthoID = m_program->uniform_attribute("ortho");

    m_texture = std::make_unique<MgCore::Texture>("icon.png", m_program.get());

    std::cout << (m_width / 37) * (m_height / 37) << std::endl;

    for (int x = 0; x < (m_width / 37); x++) {
        for (int y = 0; y < (m_height / 37); y++) {
            MeshPtr mesh = std::make_unique<MgCore::Mesh2D>(m_program.get(), m_texture.get());
            mesh->scale(32.0f, 32.0f);
            mesh->translate(x * 37.0f, y * 37.0f);
            m_meshes.push_back(std::move(mesh));
        }
    }


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

    int size = m_meshes.size();
    int newSize = (m_width/37) * (m_height/37);
    if (newSize > size) {
        m_meshes.resize(newSize);
    } else if (newSize < size) {
        while (m_meshes.size() != newSize) {
            m_meshes.pop_back();
        }
    }

    int i = 0;
    for (int y = 0; y < (m_height/37); y++) {
        for (int x = 0; x < (m_width/37); x++) {
            i = (y * (m_width/37) + x);
            if (i >= size) {
                MeshPtr mesh = std::make_unique<MgCore::Mesh2D>(m_program.get(), m_texture.get());
                m_meshes[i] = std::move(mesh);
            }
            m_meshes[i]->scale(32.0f, 32.0f);
            m_meshes[i]->translate(x * 37.0f, y * 37.0f);
        }
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

void GameManager::update()
{
    for (auto& mesh : m_meshes) {
        mesh->update();
    }

}

void GameManager::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->use();
    for (auto& mesh : m_meshes) {
        mesh->render();
    }

}
