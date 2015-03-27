#include <iostream>
#include <functional>
#include <utility>

#include "config.hpp"
#include "vfs.hpp"
#include "game.hpp"

GameManager::GameManager()
{
    m_width = 800;
    m_height = 600;
    m_fullscreen = false;
    m_title = "Game";
    m_running = false;

    m_window = std::make_unique<MgCore::Window>(m_width, m_height, m_fullscreen, m_title);
    auto tempContext = std::make_unique<MgCore::Context>();

    m_window->make_current(tempContext.get());

    bool systemSupported = true;

    if(glewInit() != GLEW_OK)
    {
        std::cout << "Error: glew failed to load." << std::endl;
    }

    // get context info
    MgCore::GraphicsInfo conInfo = tempContext->get_info();

    std::cout << "Version: " << conInfo.version << std::endl;
    std::cout << "Version: " << conInfo.versionMajor << "." << conInfo.versionMinor << std::endl;
    std::cout << "GLSL: " << conInfo.glsl << std::endl;
    std::cout << "Renderer: " << conInfo.renderer << std::endl;
    std::cout << "Vender: " << conInfo.vendor << std::endl;
    std::cout << conInfo.extensions[0] << std::endl;

    std::vector<std::string> conExt = conInfo.extensions;

    if (conInfo.versionMajor == 1 && conInfo.versionMinor < 4 ) {
        systemSupported = false;
    } else if (conInfo.versionMajor < 2) {

        auto comp = [] (const std::string &value) {

            if (value != "ARB_vertex_array_object") {
                return false;
            } else if (value != "GL_ARB_fragment_shader") {
                return false;
            } else if (value != "GL_ARB_vertex_shader") {
                return false;
            } else if (value != "GL_ARB_shader_objects") {
                return false;
            } else if (value != "GL_ARB_vertex_buffer_object") {
                return false;
            }

            return true;
        };

        if (std::find_if(conExt.begin(), conExt.end(), comp) != conExt.end()) {
            systemSupported = false;
        }

    }
    if (systemSupported != true) {
        MgCore::show_messagebox(MgCore::MessageBoxStyle::Error, "Error", "The graphics card in this system doesnt support the needed functionality used by this game.");
        return;
    }

    m_context = std::make_unique<MgCore::Context>(conInfo.versionMajor, conInfo.versionMinor);
    m_events = std::make_unique<MgCore::Events>();
    m_clock = std::make_unique<MgCore::FpsTimer>();
    m_running = true;

    m_window->make_current(m_context.get());
    //auto conInfo = m_context->get_info();
    m_window->show();

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

    m_lis.handler = std::bind(&GameManager::event_handler, this, std::placeholders::_1);
    m_lis.mask = MgCore::EventType::EventAll;


    m_events->add_listener(m_lis);

    m_fpsTime = 0.0;

    m_ss = std::cout.precision();

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    MgCore::ShaderInfo vertInfo {GL_VERTEX_SHADER_ARB, "shaders/main110.vs"};
    MgCore::ShaderInfo fragInfo {GL_FRAGMENT_SHADER_ARB, "shaders/main110.fs"};

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
            //m_mesh->translate(static_cast<float>(m_mouseX), static_cast<float>(m_mouseY));
            break;
        case MgCore::EventType::WindowSized:
            resize(event.event.windowSized.width, event.event.windowSized.height);
            //std::cout << event.event.windowSized.width  << " "
            //          << event.event.windowSized.height << " "
            //          << event.event.windowSized.id     << std::endl;
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
