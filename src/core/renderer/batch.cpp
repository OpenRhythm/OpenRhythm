// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "batch.hpp"
#include <iostream>
#include <algorithm>

namespace ORCore
{

    Batch::Batch(ShaderProgram *program, Texture *texture, int batchSize, int id)
    : m_program(program), m_texture(texture), m_batchSize(batchSize), m_id(id),
    m_committed(false), m_owned(false), m_matTexBuffer(GL_RGBA32F)
    {
        m_vertices.reserve(batchSize);
        m_matrices.reserve(batchSize);
        m_meshMatrixIndex.reserve(batchSize);
        init_gl();
    }

    void Batch::init_gl()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        m_vertLoc = glGetAttribLocation(*m_program, "position");
        m_uvLoc = glGetAttribLocation(*m_program, "vertexUV");
        m_colorLoc = glGetAttribLocation(*m_program, "color");
        m_matIndexLoc = glGetAttribLocation(*m_program, "matrixIndex");
        m_texSampID = glGetUniformLocation(*m_program, "textureSampler");
        m_matBufTexID = glGetUniformLocation(*m_program, "matrixBuffer");


        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        
        // Vertex Buffer
        glGenBuffers(1, &m_vbo);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glEnableVertexAttribArray(m_vertLoc);
        glEnableVertexAttribArray(m_uvLoc);
        glEnableVertexAttribArray(m_colorLoc);
        glEnableVertexAttribArray(m_matIndexLoc);

        // Setup VAO attributes for this batch. Once these are set the vbo can be replaced or allocated and these will still be valid.
        glVertexAttribPointer(m_vertLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, vertex)));
        glVertexAttribPointer(m_uvLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, uv)));
        glVertexAttribPointer(m_colorLoc, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, color)));
        glVertexAttribIPointer(m_matIndexLoc, 1, GL_INT, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, matIndex)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Matrix Texture Buffer Objects
        glGenBuffers(1, &m_matBufferObject);
        glBindBuffer(GL_TEXTURE_BUFFER, m_matBufferObject);
        m_matTexBuffer.assign_buffer(m_matBufferObject);

        glBindBuffer(GL_TEXTURE_BUFFER, 0);

        glBindVertexArray(0);

    }

    void Batch::clear()
    {
        m_committed = false;
        m_matrices.clear();
        m_vertices.clear();
    }

    bool Batch::add_mesh(Mesh& mesh, const glm::mat4& transform)
    {
        // Optimize this using glMapBuffer? constrain batch with m_batchSize return false if mesh doesnt fit.
        int meshVertexCount = mesh.vertices.size();
        if ((m_vertices.size() + meshVertexCount) <= m_batchSize)
        {
            // Add one index per vertex
            for (auto &vertex : mesh.vertices)
            {
                vertex.matIndex = m_matrices.size();
            }

            mesh.transformOffset = m_matrices.size();
            mesh.verticesOffset = m_vertices.size();

            // Condensing the translation will be moved to the object side of things to make rebuilding geometry less cpu intensive.
            m_matrices.push_back(transform);
            m_vertices.insert(std::end(m_vertices), std::begin(mesh.vertices), std::end(mesh.vertices));

            return true;
        }
        else
        {
            return false;
        }
    }

    void Batch::update_mesh(Mesh& mesh, const glm::mat4& transform)
    {
        m_matrices[mesh.transformOffset] = transform;
        for (auto &vertex : mesh.vertices)
        {
            vertex.matIndex = mesh.transformOffset;
        }
        std::copy(std::begin(mesh.vertices), std::end(mesh.vertices), std::begin(m_vertices)+mesh.verticesOffset);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferSubData(GL_ARRAY_BUFFER,  mesh.verticesOffset*sizeof(Vertex), mesh.vertices.size()*sizeof(Vertex), &mesh.vertices[0]);

        glBindBuffer(GL_TEXTURE_BUFFER, m_matBufferObject);
        glBufferSubData(GL_TEXTURE_BUFFER, mesh.transformOffset*sizeof(glm::mat4), sizeof(glm::mat4), &transform[0]);

        glBindBuffer(GL_TEXTURE_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Batch::set_state(RenderState state)
    {
        m_state = state;
    }

    // update buffer objects
    void Batch::commit()
    {
        m_committed = true;

        //std::cout << m_vertices.size() << std::endl;

        // When switching to glMapBuffer
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size()*sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_TEXTURE_BUFFER, m_matBufferObject);
        glBufferData(GL_TEXTURE_BUFFER, m_matrices.size()*sizeof(glm::mat4), &m_matrices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_TEXTURE_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Batch::render()
    {
        if (m_matrices.size() > 0)
        {

            glBindVertexArray(m_vao);

            // Bind textures
            m_texture->bind(m_texSampID);
            m_matTexBuffer.bind(m_matBufTexID);

            glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
        }

    }

    Batch::~Batch()
    {

        glBindVertexArray(m_vao);
        glDisableVertexAttribArray(m_colorLoc);
        glDisableVertexAttribArray(m_uvLoc);
        glDisableVertexAttribArray(m_vertLoc);
        glDisableVertexAttribArray(m_matIndexLoc);
        glDeleteVertexArrays(1, &m_vao);

        // Make sure the buffers arent bound
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_TEXTURE_BUFFER, 0);
        
        glDeleteBuffers(1, &m_vbo); 
        glDeleteBuffers(1, &m_matBufferObject);

    }

} // namespace ORCore
