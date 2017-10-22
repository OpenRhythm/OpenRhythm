// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#pragma once
#include <memory>
#include <unordered_map>
#include <string>

#include <spdlog/spdlog.h>

#include "texture.hpp"
#include "batch.hpp"
#include "mesh.hpp"

namespace ORCore
{

    std::vector<Vertex> create_rect_mesh(const glm::vec4& color);
    std::vector<Vertex> create_rect_z_mesh(const glm::vec4& color);
    std::vector<Vertex> create_rect_z_center_mesh(const glm::vec4& color);
    std::vector<Vertex> create_cube_mesh(const glm::vec4& color);

	struct RenderObject
	{
		Mesh mesh;
		glm::mat4 modelMatrix;
        RenderState state;
        ObjectID id; // id of this object in the renderer.
        BatchID batchID;

        RenderObject();
        void set_scale(glm::vec3&& scale);
        void set_translation(glm::vec3&& translation);
        void set_primitive_type(Primitive primitive);
        void set_geometry(std::vector<Vertex>&& geometry);
        void set_texture(TextureID texture);
        void set_program(ProgramID program);
        void set_camera(CameraID camera);
        void update();
	};

    struct CameraObject
    {
        glm::mat4 projection;
        glm::vec3 translation;

        float rotation; // degrees
        glm::vec3 axes;

        glm::mat4 cameraMatrix;
        CameraID id;

        std::string uniformName;

        CameraObject();
        void set_projection(glm::mat4&& proj);
        void set_rotation(float rot, glm::vec3&& axis);
        void set_translation(glm::vec3&& translate);
        void set_uniform_name(std::string&& name);
        void update();
    };

	// Builds and renders batches from objects.
    class Renderer
    {
    public:
        Renderer();
        
        // Disable copy since wth would you want to copy the renderer...
        Renderer(const Renderer&) = delete;
        void operator=(const Renderer&) = delete;

        void init_gl();
        BatchID create_batch(RenderState state, int batchSize);
        bool check_error();

        // Renderable object API
        ObjectID add_object(const RenderObject& objIn);
        RenderObject* get_object(ObjectID objID);
        void update_object(ObjectID objID);

        // Camera API
        CameraID add_camera(CameraObject& camera);
        CameraObject* get_camera(CameraID camID);
        void update_camera(CameraID camID);

        TextureID add_texture(Image&& img);
        ProgramID add_program(Shader&& vertex, Shader&& fragment);
        ShaderProgram* get_program(ProgramID id);
        
        // add global attribute/uniforms for shaders ?
        void commit();
        void render();
        void clear();

    private:
        BatchID find_batch(RenderState state);
        std::vector<RenderObject> m_objects;
        std::vector<std::unique_ptr<Batch>> m_batches;
        std::vector<std::unique_ptr<Texture>> m_textures;
        std::vector<std::unique_ptr<ShaderProgram>> m_programs;
        std::vector<CameraObject> m_cameras;
        std::shared_ptr<spdlog::logger> m_logger;
        int m_defaultTextureID;
    };
}
