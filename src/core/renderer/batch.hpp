// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "texture.hpp"
#include "mesh.hpp"

namespace ORCore
{
    // `BatchInterface` defines the standard interface for all batch types.
    class BatchInterface
    {
    public:
        virtual void init_gl() = 0;
        virtual void clear() = 0;
        virtual void commit() = 0;
        virtual void render() = 0;

        // `RenderState` is the various state values this batch requires to be set.
        virtual void set_state(const RenderState& state) = 0;
        virtual const RenderState& get_state() = 0;

        // For batch types that require a transform a default one will be used.
        virtual bool add_mesh(Mesh& mesh) = 0; 

        // for batch types which do not support transforms, the mesh will be transformed on the cpu.
        virtual bool add_mesh(Mesh& mesh, const glm::mat4& transform) = 0;

        // similar as the above add_mesh functions
        virtual void update_mesh(Mesh& mesh) = 0;
        virtual void update_mesh(Mesh& mesh, const glm::mat4& transform) = 0;

        // If a batch is committed, this means that all of its data is already on
        // the gpu, and is ready to render.
        virtual bool is_committed() = 0;

        // If a batch isn't owned it will be fully managed by the renderer class.
        virtual void set_owned(bool owned) = 0;
        virtual bool is_owned() = 0;

        virtual BatchID get_id() = 0;

        virtual ShaderProgram* get_program() = 0;

        virtual ~BatchInterface() = default;
    };

    // The purpose of BatchBase seperate from BatchInterface is to have common implementations of 
    class BatchBase : public BatchInterface
    {
    public:

        BatchBase(ShaderProgram* program, int batchSize, BatchID id)
        :m_program(program), m_batchSize(batchSize), m_id(id), m_committed(false), m_owned(false)
        {
        }

        virtual void set_state(const RenderState& state);

        virtual const RenderState& get_state()
        {
            return m_state;
        }

        virtual bool is_committed()
        {
            return m_committed;
        }

        virtual bool is_owned()
        {
            return m_owned;
        }

        virtual void set_owned(bool owned)
        {
            m_owned = owned;
        }

        virtual BatchID get_id()
        {
            return m_id;
        }

        virtual ShaderProgram* get_program()
        {
            return m_program;
        }
        virtual ~BatchBase() = default;

    protected:

        ShaderProgram *m_program;
        int m_batchSize;
        BatchID m_id;

        bool m_committed;
        bool m_owned;

        GLenum m_primitive;
        RenderState m_state;
    };

    class Batch : public BatchBase
    {
    public:
        Batch(ShaderProgram *program, Texture *texture, int batchSize, BatchID id);
        virtual void init_gl();
        virtual void clear();
        virtual bool add_mesh(Mesh& mesh); 
        virtual bool add_mesh(Mesh& mesh, const glm::mat4& transform);
        virtual void update_mesh(Mesh& mesh);
        virtual void update_mesh(Mesh& mesh, const glm::mat4& transform);
        virtual void commit();
        virtual void render();
        ~Batch();

    protected:
        BufferTexture m_matTexBuffer;
        GLuint m_vertLoc;
        GLuint m_uvLoc;
        GLuint m_colorLoc;
        GLuint m_matIndexLoc;
        GLuint m_texSampID;
        GLuint m_matBufTexID;
        GLuint m_vao;
        GLuint m_vbo;
        GLuint m_matBufferObject;

        Texture *m_texture;

        std::vector<int> m_meshMatrixIndex;

        std::vector<Vertex> m_vertices;
        std::vector<glm::mat4> m_matrices;
    };

    class SimpleBatch : public BatchBase
    {
    public:
        SimpleBatch(ShaderProgram *program, Texture *texture, int batchSize, BatchID id);
        virtual void init_gl();
        virtual void clear();
        virtual bool add_mesh(Mesh& mesh); 
        virtual bool add_mesh(Mesh& mesh, const glm::mat4& transform);
        virtual void update_mesh(Mesh& mesh);
        virtual void update_mesh(Mesh& mesh, const glm::mat4& transform);
        virtual void commit();
        virtual void render();
        ~SimpleBatch();

    protected:
        int m_batchSize;
        GLuint m_vertLoc;
        GLuint m_colorLoc;
        GLuint m_texSampID;
        GLuint m_vao;
        GLuint m_vbo;

        Texture *m_texture;

        std::vector<Vertex> m_vertices;
    };

} // namespace ORCore
