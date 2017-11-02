// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_GTX_transform
#include "config.hpp"
#include "renderer.hpp"
#include <iostream>

#include <glm/gtx/transform.hpp>

namespace ORCore
{
    const double pi = 3.14159265359;

    // TODO - Remove these out of the renderer... PLZZZZ gah i hate these here lol
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

    // TODO - A lot can still be done here in general.

    RenderObject::RenderObject()
    :state{-1, -1, -1}, id(-1), batchID(-1)
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
        }
        else if (primitive == Primitive::line)
        {
            mesh.vertexSize = 2;

        }
        else if (primitive == Primitive::point)
        {
            mesh.vertexSize = 1;
        }
        state.primitive = primitive;
    }

    void RenderObject::set_geometry(std::vector<Vertex>&& geometry)
    {
        if (batchID == -1 || geometry.size() == mesh.vertices.size())
        {
            mesh.vertices = geometry;
        }
        else
        {
            throw std::length_error("Geometry size does not match original.");
        }
    }

    void RenderObject::set_texture(TextureID texture)
    {
        state.texture = texture;
    }

    void RenderObject::set_program(ProgramID program)
    {
        state.program = program;
    }

    void RenderObject::set_camera(CameraID camera)
    {
        state.camera = camera;
    }

    void RenderObject::update()
    {
        modelMatrix = glm::scale(glm::translate(mesh.translate), mesh.scale);
    }

    CameraObject::CameraObject()
    :id(-1)
    {
    }

    void CameraObject::set_projection(glm::mat4&& proj)
    {
        projection = proj;
    }

    void CameraObject::set_rotation(float rot, glm::vec3&& axis)
    {
        axes = axis;
        rotation = glm::radians(rot);
    }

    void CameraObject::set_translation(glm::vec3&& translate)
    {
        translation = translate;
    }

    void CameraObject::set_uniform_name(std::string&& name)
    {
        uniformName = name;
    }

    void CameraObject::update()
    {
        cameraMatrix = projection * glm::rotate(rotation, axes) * glm::scale(glm::vec3(1.0f, 1.0f, -1.0f)) * glm::translate(-translation);
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

    Batch* Renderer::create_batch_owned(RenderState state, int batchSize)
    {
        BatchID id = m_batches.size();
        m_batches.push_back(
            std::make_unique<Batch>(
                m_programs[state.program].get(),
                m_textures[state.texture].get(),
                batchSize, id));
        auto &batch = m_batches.back();
        batch->set_state(state);
        batch->set_owned(true);

        return batch.get();
    }

    BatchID Renderer::create_batch(RenderState state, int batchSize)
    {
        BatchID id = m_batches.size();
        m_batches.push_back(
            std::make_unique<Batch>(
                m_programs[state.program].get(),
                m_textures[state.texture].get(),
                batchSize, id));
        auto &batch = m_batches.back();
        batch->set_state(state);
        return id;
    }

    BatchID Renderer::find_batch(RenderState state)
    {
        // Find existing batch that isnt full or already submitted.
        for (auto &batch : m_batches)
        {

            auto& bState = batch->get_state();
            // TODO - Clean this up with a custom data structure that allows faster iteration over render states vs something like a map.
            // The current struct implementation is just a bit to manual to add new render states for my tastes.
            if (!batch->is_committed() && !batch->is_owned() && bState.texture == state.texture && bState.program == state.program && state.camera == bState.camera && state.primitive == bState.primitive)
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
        }
        while(error != GL_NO_ERROR);

        return errorFound;
    }

    int Renderer::add_object(const RenderObject& objIn)
    {

        ObjectID objID = m_objects.size();
        m_objects.push_back(objIn);
        auto &obj = m_objects.back();

        obj.id=objID;
        obj.update();

        if (obj.state.texture == -1)
        {
            obj.state.texture = m_defaultTextureID;
        }

        BatchID batchId = find_batch(obj.state);

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
    RenderObject* Renderer::get_object(ObjectID objID)
    {
        return &m_objects[objID];
    }

    void Renderer::update_object(ObjectID objID)
    {
        RenderObject& obj = m_objects[objID];
        obj.update();
        m_batches[obj.batchID]->update_mesh(obj.mesh, obj.modelMatrix);
    }

    CameraID Renderer::add_camera(CameraObject& camera)
    {
        CameraID camID = m_cameras.size();
        m_cameras.push_back(camera);
        m_cameras.back().id = camID;
        return camID;
    }

    CameraObject* Renderer::get_camera(CameraID camID)
    {
        return &m_cameras[camID];
    }

    void Renderer::update_camera(CameraID camID)
    {
        m_cameras[camID].update();
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
        ProgramID id = m_programs.size();
        m_programs.push_back(std::make_unique<ShaderProgram>(vertex, fragment));
        auto &program = m_programs.back();
        program->check_error();
        return id;
    }

    ShaderProgram* Renderer::get_program(int id)
    {
        return m_programs[id].get();
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
        // TODO - We need to be able to better define render order
        // TODO - We just dont really handle transparency at all atm other than using the order batches are created.
        for (auto &batch : m_batches)
        {
            ShaderProgram* program = batch->get_program();
            program->use();

            int camID = batch->get_state().camera;
            auto& camera = m_cameras[camID];
            glUniformMatrix4fv(glGetUniformLocation(*program, camera.uniformName.c_str()), 1, GL_FALSE, &camera.cameraMatrix[0][0]);

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

}
