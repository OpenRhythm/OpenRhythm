#include <iostream>
#include <functional>

#include "game.hpp"

GameManager::GameManager()
{
    m_width = 800;
    m_height = 600;
    m_fullscreen = false;
    m_title = "Game";

    m_window = new MgCore::Window(m_width, m_height, m_fullscreen, m_title);
    m_context = new MgCore::Context(3, 2, 0);
    m_events = new MgCore::Events();
    m_clock = new MgCore::FpsTimer();
    m_running = true;

    m_window->make_current(m_context);

    if(ogl_LoadFunctions() == ogl_LOAD_FAILED)
    {
        std::cout << "Error: glLoadGen failed to load.";
    }

    
    m_lis.handler = std::bind(&GameManager::event_handler, this, std::placeholders::_1);
    m_lis.mask = MgCore::EventType::Quit | MgCore::EventType::WindowSized;


    m_events->add_listener(m_lis);

    m_fpsTime = 0.0;

    m_ss = std::cout.precision();

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    MgCore::ShaderInfo vertInfo {GL_VERTEX_SHADER, "../data/shaders/main.vs"};
    MgCore::ShaderInfo fragInfo {GL_FRAGMENT_SHADER, "../data/shaders/main.fs"};

    MgCore::Shader vertShader(vertInfo);
    MgCore::Shader fragShader(fragInfo);

    m_program = new MgCore::ShaderProgram(&vertShader, &fragShader);
    m_program->use();
    m_orthoID = m_program->uniform_attribute("ortho");

    m_mesh = new MgCore::Mesh2D(m_program);
    m_mesh->scale(32.0f, 32.0f);
    m_mesh->translate(10.0f, 10.0f);

    m_ortho = glm::ortho(0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, -1.0f, 1.0f);

    m_program->set_uniform(m_orthoID, m_ortho);

    glClearColor(0.5, 0.5, 0.5, 1.0);
}

GameManager::~GameManager()
{

    glDeleteVertexArrays(1, &m_vao);
    m_window->make_current(nullptr);
    delete m_mesh;
    delete m_window;
    delete m_context;
    delete m_events;
    delete m_clock;
    delete m_program;

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
