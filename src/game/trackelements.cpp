// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "config.hpp"
#include "resolver.hpp"

#include "trackelements.hpp"

#include <iostream>

namespace ORGame
{
    void TrackElements::set_renderInfo(CoreRenderIDs renderIDs)
    {
        m_renderIDs = renderIDs;
    }

    void TrackElements::update(double songTime)
    {
        auto& renderer = ORCore::Resolver::get<ORCore::Renderer>();

        // TODO - Allow renderer to be able to specify uniforms and set them per batch/shader
        auto neckProgram = renderer.get_program(m_neckProgram);
        neckProgram->use();
        

        float boardPos = (songTime/neck_speed_divisor);

        // TODO - FIX ME No gl calls outside of renderer.
        glUniform1f(m_boardPosID, boardPos/neck_board_length);
    }

    void TrackElements::init_bars(std::vector<BarEvent> &bars)
    {
        auto& renderer = ORCore::Resolver::get<ORCore::Renderer>();

        m_texture = renderer.add_texture(ORCore::loadSTB("data/icon.png"));

        // reuse the same container when creating bars as add_obj wont modify the original.
        ORCore::RenderObject obj;
        obj.set_camera(m_renderIDs.cameraDynamic);
        obj.set_texture(m_texture);
        obj.set_program(m_renderIDs.program);

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

            obj.set_translation(glm::vec3{0.0, 0.0f, z}); // center the line on the screen
            obj.set_primitive_type(ORCore::Primitive::triangle);
            obj.set_geometry(ORCore::create_rect_z_center_mesh(glm::vec4{1.0f,1.0f,1.0f,1.0f}));

            renderer.add_object(obj);
        }
    }

    void TrackElements::init_neck()
    {
        auto& renderer = ORCore::Resolver::get<ORCore::Renderer>();

        m_neckTexture = renderer.add_texture(ORCore::loadSTB("data/neck.png"));

        // Neck shaders
        ORCore::ShaderInfo neckVertInfo {GL_VERTEX_SHADER, "./data/shaders/neck.vs"};
        ORCore::ShaderInfo fragInfo {GL_FRAGMENT_SHADER, "./data/shaders/main.fs"};

        ORCore::ShaderProgram neckProgram;
        neckProgram.add_shader(ORCore::Shader(neckVertInfo));
        neckProgram.add_shader(ORCore::Shader(fragInfo));
        neckProgram.link();
        m_neckProgram = renderer.add_program(std::move(neckProgram));


        auto neckProgramTemp = renderer.get_program(m_neckProgram);

        m_boardPosID = glGetUniformLocation(*neckProgramTemp, "neckPos");


        // Setup Neck geometry/objects
        ORCore::RenderObject obj;

        obj.set_camera(m_renderIDs.cameraStatic);
        obj.set_program(m_neckProgram);

        obj.set_primitive_type(ORCore::Primitive::triangle);
        obj.set_texture(m_neckTexture);

        obj.set_scale(glm::vec3{1.0f, 1.0f, neck_board_length});
        obj.set_translation(glm::vec3{0.0f, 0.0f, -0.35f});
        obj.set_geometry(ORCore::create_rect_z_mesh(glm::vec4{1.0f,1.0f,1.0f,1.0f}));
        m_neckObj = renderer.add_object(obj);

    }

    void TrackElements::init_solo_sections(std::vector<Event> &events)
    {
        auto& renderer = ORCore::Resolver::get<ORCore::Renderer>();

        m_soloNeckTexture = renderer.add_texture(ORCore::loadSTB("data/soloneck.png"));

        // Solos
        ORCore::RenderObject obj;
        obj.set_primitive_type(ORCore::Primitive::triangle);
        obj.set_camera(m_renderIDs.cameraDynamic);
        obj.set_program(m_renderIDs.program);
        obj.set_texture(m_soloNeckTexture);

        glm::vec4 solo_color = glm::vec4{0.0f,1.0f,1.0f,0.75f};
        for (auto &event : events)
        {

            if (event.type == EventType::solo) {
                float z = event.time / neck_speed_divisor;
                float length = event.length / neck_speed_divisor;

                obj.set_scale(glm::vec3{1.125f, 1.0f, length});
                obj.set_translation(glm::vec3{-0.0625f, 0.0f, z});
                obj.set_geometry(ORCore::create_rect_z_mesh(solo_color));
                renderer.add_object(obj);
            }
        }

    }

    void TrackElements::init_energy_sections(std::vector<Event> &events)
    {
        auto& renderer = ORCore::Resolver::get<ORCore::Renderer>();

        // drive
        ORCore::RenderObject obj;
        obj.set_primitive_type(ORCore::Primitive::triangle);
        obj.set_camera(m_renderIDs.cameraDynamic);
        obj.set_program(m_renderIDs.program);
        obj.set_texture(m_soloNeckTexture);

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
                renderer.add_object(obj);
            }
        }

    }

}
