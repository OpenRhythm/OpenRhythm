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

    std::vector<Vertex> create_rect_mesh(glm::vec4 color);
    std::vector<Vertex> create_rect_z_mesh(glm::vec4 color);
    std::vector<Vertex> create_rect_z_center_mesh(glm::vec4 color);
    std::vector<Vertex> create_cube_mesh(glm::vec4 color);

	struct RenderObject
	{
		Mesh mesh;
		glm::mat4 modelMatrix;
        RenderState state;
        int id; // id of this object in the renderer.
        int batchID;

        RenderObject();
        void set_scale(glm::vec3&& scale);
        void set_translation(glm::vec3&& translation);
        void set_primitive_type(Primitive primitive);
        void set_geometry(std::vector<Vertex>&& geometry);
        void set_texture(int _texture);
        void set_program(int _program);
        void update();
	};

	// Builds and renders batches from objects.
    class Renderer
    {
    public:
        Renderer();
        void init_gl();
        int create_batch(RenderState state, int batchSize);
        bool check_error();
        int add_object(const RenderObject& objIn);
        RenderObject* get_object(int objID);
        void update_object(int objID);
        int add_texture(Image&& img);
        int add_program(Shader&& vertex, Shader&& fragment);
        void set_camera_transform(std::string name, glm::mat4&& transform);
        // add global attribute/uniforms for shaders ?
        void commit();
        void render();
        void clear();
        ~Renderer();

    private:
        int find_batch(RenderState state);
        std::vector<RenderObject> m_objects;
        std::vector<std::unique_ptr<Batch>> m_batches;
        std::unordered_map<std::string, glm::mat4> m_cameraUniforms;
        std::vector<std::unique_ptr<Texture>> m_textures;
        std::vector<std::unique_ptr<ShaderProgram>> m_programs;
        std::shared_ptr<spdlog::logger> m_logger;
        int m_defaultTextureID;
    };
}