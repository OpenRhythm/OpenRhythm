#include "renderer.hpp"
#include <iostream>

namespace ORCore
{

    std::vector<Vertex> create_rect_mesh(glm::vec4 color)
    {
        return {
            // Vertex2     UV            Color
            {{0.0f, 0.0f, 0.5f}, {0.0f, 0.0f}, color},
            {{0.0f, 1.0f, 0.5f}, {0.0f, 1.0f}, color},
            {{1.0f, 0.0f, 0.5f}, {1.0f, 0.0f}, color},
            {{0.0f, 1.0f, 0.5f}, {0.0f, 1.0f}, color},
            {{1.0f, 1.0f, 0.5f}, {1.0f, 1.0f}, color},
            {{1.0f, 0.0f, 0.5f}, {1.0f, 0.0f}, color}
        };
    }


    RenderObject::RenderObject()
    {
    }

    void RenderObject::set_scale(glm::vec3&& scale)
    {
        mesh.scale = scale;
    }

    void RenderObject::set_translation(glm::vec3&& translation)
    {
        mesh.translate = translation;
    }

    void RenderObject::set_primitive_type(Primitive primitive)
    {
        mesh.primitive = primitive;
        if (primitive == Primitive::triangle)
        {
            mesh.vertexSize = 3;
        } else if (primitive == Primitive::line)
        {
            mesh.vertexSize = 2;

        } else if (primitive == Primitive::point)
        {
            mesh.vertexSize = 1;
        }
    }

    void RenderObject::set_geometry(std::vector<Vertex>&& geometry)
    {
        mesh.vertices = geometry;
    }

    void RenderObject::set_texture(int _texture)
    {
        texture = _texture;
    }

    void RenderObject::set_program(int _program)
    {
        program = _program;
    }


    Renderer::Renderer()
    : m_logger(spdlog::get("default"))
    {
    }

    int Renderer::find_batch(int texture, int program)
    {
        // Find existing batch that isnt full or already submitted.
        for (int i = 0; i < m_batchesInfo.size(); i++)
        {
            if (m_batchesInfo[i].committed == false && m_batchesInfo[i].texture == texture && m_batchesInfo[i].program == program)
            {
                return i;
            }
        }

        std::cout << "No batches found creating new batch: " << std::endl;

        // If the above couldnt find a batch create one.
        BatchInfo batchInfo = {false, program, texture};
        int id = m_batchesInfo.size();
        m_batchesInfo.push_back(batchInfo);
        m_batches.push_back(std::make_unique<Batch>(m_programs[program].get(), m_textures[texture].get(), 512));
        return id;
    }

    int Renderer::add_object(const RenderObject& objIn)
    {
        int batchId = find_batch(objIn.texture, objIn.program);

        m_objects.push_back(objIn);
        auto &obj = m_objects.back();

        obj.id=m_objects.size();
        obj.modelMatrix = glm::scale(glm::translate(glm::mat4(1.0f), obj.mesh.translate), obj.mesh.scale);

        // try until it gets added to a batch.
        while (m_batches[batchId]->add_mesh(obj.mesh, obj.modelMatrix) != true)
        {
            m_batches[batchId]->commit(); // commit that batch as it is full.
            m_batchesInfo[batchId].committed = true;
            batchId = find_batch(objIn.texture, objIn.program); // find or create the next batch
        }

        return obj.id;
    }

    int Renderer::add_texture(Image&& img)
    {
        int id = m_textures.size();
        m_textures.push_back(std::make_unique<Texture>(GL_TEXTURE_2D));
        auto &texture = m_textures.back();
        texture->update_image_data(img);
        return id;
    }

    int Renderer::add_program(Shader&& vertex, Shader&& fragment)
    {
        int id = m_programs.size();
        m_programs.push_back(std::make_unique<ShaderProgram>(vertex, fragment));
        auto &program = m_programs.back();
        program->check_error();
        return id;
    }

    void Renderer::set_camera_transform(std::string name, glm::mat4&& transform)
    {
        try {
            m_cameraUniforms.at(name) = transform;
        } catch (std::out_of_range &err) {
            m_cameraUniforms.insert({name, transform});
        }
    }

    // commit all remaining batches.
    void Renderer::commit()
    {
        for (int i = 0; i < m_batchesInfo.size(); i++)
        {
            if (m_batchesInfo[i].committed == false)
            {
                m_batches[i]->commit();
            }
        }
    }

    void Renderer::render()
    {
        // TODO - Do sorting of batches to minimize state changes.
        int currentProgram = -1;
        for (int i = 0; i < m_batchesInfo.size(); i++)
        {
            currentProgram = m_batchesInfo[i].program;
            auto &program = m_programs[currentProgram];

            program->use();
            for (auto &cam : m_cameraUniforms)
            {
                program->set_uniform(program->uniform_attribute(cam.first), cam.second);
            }
            m_batches[i]->render();
        }
    }

    Renderer::~Renderer()
    {

    }


}