#include "config.hpp"
#include "renderer.hpp"
#include <iostream>

namespace ORCore
{

    std::vector<Vertex> create_rect_mesh(const glm::vec4& color)
    {
        return {
            // Vertex2           UV            Color
            {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, color},
            {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, color},
            {{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, color},

            {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, color},
            {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, color},
            {{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, color}
        };
    }

    std::vector<Vertex> create_rect_z_mesh(const glm::vec4& color)
    {
        return {
            // Vertex2           UV            Color
            {{0.0f, 0.0, 0.0f}, {0.0f, 0.0f}, color},
            {{0.0f, 0.0, 1.0f}, {0.0f, 1.0f}, color},
            {{1.0f, 0.0, 0.0f}, {1.0f, 0.0f}, color},

            {{0.0f, 0.0, 1.0f}, {0.0f, 1.0f}, color},
            {{1.0f, 0.0, 1.0f}, {1.0f, 1.0f}, color},
            {{1.0f, 0.0, 0.0f}, {1.0f, 0.0f}, color}
        };
    }

    std::vector<Vertex> create_rect_z_center_mesh(const glm::vec4& color)
    {
        return {
            // Vertex2           UV            Color
            {{0.0f, 0.0, -1.0f}, {0.0f, 0.0f}, color},
            {{0.0f, 0.0, 1.0f}, {0.0f, 1.0f}, color},
            {{1.0f, 0.0, -1.0f}, {1.0f, 0.0f}, color},

            {{0.0f, 0.0, 1.0f}, {0.0f, 1.0f}, color},
            {{1.0f, 0.0, 1.0f}, {1.0f, 1.0f}, color},
            {{1.0f, 0.0, -1.0f}, {1.0f, 0.0f}, color}
        };
    }

    std::vector<Vertex> create_cube_mesh(const glm::vec4& color)
    {
        return {
            {{0.0f,0.0f,-0.5f}, {0.0f, 0.0f}, color},
            {{0.0f,0.0f,0.5f}, {0.0f, 1.0f}, color},
            {{0.0f, 1.0f,0.5f}, {1.0f, 0.0f}, color},

            {{1.0f, 1.0f,-0.5f}, {0.0f, 1.0f}, color},
            {{0.0f,0.0f,-0.5f}, {1.0f, 1.0f}, color},
            {{0.0f, 1.0f,-0.5f}, {1.0f, 0.0f}, color},

            {{1.0f,0.0f,0.5f}, {0.0f, 0.0f}, color},
            {{0.0f,0.0f,-0.5f}, {0.0f, 1.0f}, color},
            {{1.0f,0.0f,-0.5f}, {1.0f, 0.0f}, color},

            {{1.0f, 1.0f,-0.5f}, {0.0f, 1.0f}, color},
            {{1.0f,0.0f,-0.5f}, {1.0f, 1.0f}, color},
            {{0.0f,0.0f,-0.5f}, {1.0f, 0.0f}, color},

            {{0.0f,0.0f,-0.5f}, {0.0f, 0.0f}, color},
            {{0.0f, 1.0f,0.5f}, {0.0f, 1.0f}, color},
            {{0.0f, 1.0f,-0.5f}, {1.0f, 0.0f}, color},

            {{1.0f,0.0f,0.5f}, {0.0f, 1.0f}, color},
            {{0.0f,0.0f,0.5f}, {1.0f, 1.0f}, color},
            {{0.0f,0.0f,-0.5f}, {1.0f, 0.0f}, color},

            {{0.0f, 1.0f,0.5f}, {0.0f, 0.0f}, color},
            {{0.0f,0.0f,0.5f}, {0.0f, 1.0f}, color},
            {{1.0f,0.0f,0.5f}, {1.0f, 0.0f}, color},

            {{1.0f, 1.0f,0.5f}, {0.0f, 1.0f}, color},
            {{1.0f,0.0f,-0.5f}, {1.0f, 1.0f}, color},
            {{1.0f, 1.0f,-0.5f}, {1.0f, 0.0f}, color},

            {{1.0f,0.0f,-0.5f}, {0.0f, 0.0f}, color},
            {{1.0f, 1.0f,0.5f}, {0.0f, 1.0f}, color},
            {{1.0f,0.0f,0.5f}, {1.0f, 0.0f}, color},

            {{1.0f, 1.0f,0.5f}, {0.0f, 1.0f}, color},
            {{1.0f, 1.0f,-0.5f}, {1.0f, 1.0f}, color},
            {{0.0f, 1.0f,-0.5f}, {1.0f, 0.0f}, color},

            {{1.0f, 1.0f,0.5f}, {0.0f, 0.0f}, color},
            {{0.0f, 1.0f,-0.5f}, {0.0f, 1.0f}, color},
            {{0.0f, 1.0f,0.5f}, {1.0f, 0.0f}, color},

            {{1.0f, 1.0f,0.5f}, {0.0f, 1.0f}, color},
            {{0.0f, 1.0f,0.5f}, {1.0f, 1.0f}, color},
            {{1.0f,0.0f,0.5f}, {1.0f, 0.0f}, color}
        };
    }


    RenderObject::RenderObject()
    :state{-1, -1}, id(-1), batchID(-1)
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
        if (batchID == -1 || geometry.size() == mesh.vertices.size())
        {
            mesh.vertices = geometry;
        } else {
            throw std::length_error("Geometry size does not match original.");
        }
    }

    void RenderObject::set_texture(int texture)
    {
        state.texture = texture;
    }

    void RenderObject::set_program(int program)
    {
        state.program = program;
    }

    void RenderObject::update()
    {
        modelMatrix = glm::scale(glm::translate(glm::mat4(1.0f), mesh.translate), mesh.scale);
    }


    Renderer::Renderer()
    : m_logger(spdlog::get("default"))
    {
    }

    void Renderer::init_gl()
    {
        // Eventually one thing init_gl should be able to be used to handle context resets 
        // Add the blank texture by default as it will be the default texture.
        m_defaultTextureID = add_texture(ORCore::loadSTB("data/blank.png"));
    }

    int Renderer::create_batch(RenderState state, int batchSize)
    {
        int id = m_batches.size();
        m_batches.push_back(
            std::make_unique<Batch>(
                m_programs[state.program].get(),
                m_textures[state.texture].get(),
                batchSize, id));
        auto &batch = m_batches.back();
        batch->set_state(state);
        return id;
    }

    int Renderer::find_batch(RenderState state)
    {
        // Find existing batch that isnt full or already submitted.
        for (auto &batch : m_batches)
        {

            auto& bState = batch->get_state();
            if (!batch->is_committed() && bState.texture == state.texture && bState.program == state.program)
            {
                return batch->get_id();
            }
        }

        // If the above couldnt find a batch create one.c
        return create_batch(state, 65536);
    }

    bool Renderer::check_error()
    {
        bool errorFound = false;
        GLenum error;
        do
        {
            error = glGetError();
            switch(error)
            {
                case GL_INVALID_ENUM:
                    m_logger->error("OpenGL Error: GL_INVALID_ENUM");
                    errorFound = true;
                    break;
                case GL_INVALID_VALUE:
                    m_logger->error("OpenGL Error: GL_INVALID_VALUE");
                    errorFound = true;
                    break;
                case GL_INVALID_OPERATION:
                    m_logger->error("OpenGL Error: GL_INVALID_OPERATION");
                    errorFound = true;
                    break;
                case GL_INVALID_FRAMEBUFFER_OPERATION:
                    m_logger->error("OpenGL Error: GL_INVALID_FRAMEBUFFER_OPERATION");
                    errorFound = true;
                    break;
                case GL_OUT_OF_MEMORY:
                    m_logger->error("OpenGL Error: GL_OUT_OF_MEMORY");
                    errorFound = true;
                    break;
                case GL_NO_ERROR:
                    break;
                default:
                    m_logger->error("Unknown OpenGL Error: {}", error);
                    errorFound = true;
                    break;
            }
        } while(error != GL_NO_ERROR);

        return errorFound;
    }

    int Renderer::add_object(const RenderObject& objIn)
    {


        int objID = m_objects.size();
        m_objects.push_back(objIn);
        auto &obj = m_objects.back();

        obj.id=objID;
        obj.update();

        if (obj.state.texture == -1)
        {
            obj.state.texture = m_defaultTextureID;
        }

        int batchId = find_batch(obj.state);

        // try until it gets added to a batch.
        while (m_batches[batchId]->add_mesh(obj.mesh, obj.modelMatrix) != true)
        {
            m_batches[batchId]->commit(); // commit that batch as it is full.
            batchId = find_batch(obj.state); // find or create the next batch
        }

        obj.batchID = batchId;

        check_error();

        return obj.id;
    }

    // In order to update an object you must get the object from the renderer using this command
    // Modify the object through the returned pointer, then call update_object() below.
    // If you modify geometry it MUST have the same size as the original geometry used.
    // Otherwise you risk overwriting other geometry or having orphaned geometry.
    RenderObject* Renderer::get_object(int objID)
    {
        return &m_objects[objID];
    }

    void Renderer::update_object(int objID)
    {
        RenderObject& obj = m_objects[objID];
        obj.update();
        m_batches[obj.batchID]->update_mesh(obj.mesh, obj.modelMatrix);
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
        for (auto &batch : m_batches)
        {
            if (!batch->is_committed())
            {
                batch->commit();
            }
        }
        // m_logger->info("Batches: {}", m_batches.size());

        // GLint size;
        // glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &size);
        // m_logger->info("Max buffer texture size: {}", size);
        // glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &size);
        // m_logger->info("Max texture units: {}", size);
        // glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &size);
        // m_logger->info("Max combined texture units: {}", size);
    }

    void Renderer::render()
    {
        // TODO - Do sorting of batches to minimize state changes.
        for (auto &batch : m_batches)
        {
            ShaderProgram* program = batch->get_program();
            program->use();
            for (auto &cam : m_cameraUniforms)
            {
                program->set_uniform(program->uniform_attribute(cam.first), cam.second);
            }
            batch->render();
        }
    }

    void Renderer::clear()
    {
        for (auto &batch : m_batches)
        {
            batch->clear();
        }
    }

    Renderer::~Renderer()
    {

    }


}