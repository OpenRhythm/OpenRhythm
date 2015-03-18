#include <iostream>
#include <functional>

#include "config.hpp"
#include "vfs.hpp"
#include "game.hpp"

GameManager::GameManager()
{
    m_width = 800;
    m_height = 600;
    m_fullscreen = false;
    m_title = "Game";

    m_window = std::unique_ptr<MgCore::Window>(new MgCore::Window(m_width, m_height, m_fullscreen, m_title));
    m_context = std::unique_ptr<MgCore::Context>(new MgCore::Context(3, 2, 0));
    m_events = std::unique_ptr<MgCore::Events>(new MgCore::Events());
    m_clock = std::unique_ptr<MgCore::FpsTimer>(new MgCore::FpsTimer());
    m_running = true;

    m_window->make_current(m_context.get());

    VFS.AddLoader(new ttvfs::DiskLoader);
    VFS.AddArchiveLoader(new ttvfs::VFSZipArchiveLoader);

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

    if(ogl_LoadFunctions() == ogl_LOAD_FAILED)
    {
        std::cout << "Error: glLoadGen failed to load.";
    }
    
    m_lis.handler = std::bind(&GameManager::event_handler, this, std::placeholders::_1);
    m_lis.mask = MgCore::EventType::Quit | MgCore::EventType::WindowSized | MgCore::EventType::MouseMove;


    m_events->add_listener(m_lis);

    m_fpsTime = 0.0;

    m_ss = std::cout.precision();

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    MgCore::ShaderInfo vertInfo {GL_VERTEX_SHADER, "shaders/main.vs"};
    MgCore::ShaderInfo fragInfo {GL_FRAGMENT_SHADER, "shaders/main.fs"};

    MgCore::Shader vertShader(vertInfo);
    MgCore::Shader fragShader(fragInfo);

    m_program = std::unique_ptr<MgCore::ShaderProgram>(new MgCore::ShaderProgram(&vertShader, &fragShader));
    m_program->use();
    m_orthoID = m_program->uniform_attribute("ortho");

    m_texture = std::unique_ptr<MgCore::Texture>(new MgCore::Texture("icon.png", m_program.get()));
    m_mesh = std::unique_ptr<MgCore::Mesh2D>(new MgCore::Mesh2D(m_program.get(), m_texture.get()));
    m_mesh->scale(32.0f, 32.0f);
    m_mesh->translate(0.0f, 0.0f);

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
            m_mesh->translate(static_cast<float>(m_mouseX), static_cast<float>(m_mouseY));
            break;
        case MgCore::EventType::WindowSized:
            resize(event.event.windowSized.width, event.event.windowSized.height);
            //std::cout << event.event.windowSized.width  << " "
            //          << event.event.windowSized.height << " "
            //          << event.event.windowSized.id     << std::endl;
            break;

    }
    return true;
}

void GameManager::update()
{
    m_mesh->update();

}

void GameManager::render()
{
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_program->use();
        m_mesh->render();


}
