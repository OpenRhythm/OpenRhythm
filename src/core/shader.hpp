#pragma once
#include <string>
#include <array>
#include <vector>
#include "glm/glm.hpp"

namespace MgCore
{
    enum class ShaderType
    {
        Vertex,
        Fragment
    };

    struct ShaderInfo
    {
        ShaderType type;
        std::string path;
        std::string data;
    };

    class Shader
    {
    private:
        unsigned int m_shader;
        ShaderInfo m_info;
    public:
        Shader(ShaderInfo);
        ~Shader();
        unsigned int get_shader();
    };

    std::vector<ShaderInfo> shader_parser(std::string path);

    class ShaderProgram
    {
    private:
        std::vector<Shader> m_shaders;
        unsigned int m_program;

    public:
        ShaderProgram(std::string shaderPath);
        ~ShaderProgram();

        void use();
        void disuse();

        int vertex_attribute(std::string name);
        int uniform_attribute(std::string name);

        void set_uniform(int uniform, int value);
        void set_uniform(int uniform, float value);

        void set_uniform(int uniform, const glm::vec2& value);
        void set_uniform(int uniform, const glm::vec3& value);
        void set_uniform(int uniform, const glm::vec4& value);

        void set_uniform(int uniform, const glm::mat2& value);
        void set_uniform(int uniform, const glm::mat3& value);
        void set_uniform(int uniform, const glm::mat4& value);

        void set_uniform(int uniform, const std::array<float, 2>& value);
        void set_uniform(int uniform, const std::array<float, 3>& value);
        void set_uniform(int uniform, const std::array<float, 4>& value);

        void set_uniform(int uniform, const std::array<int, 2>& value);
        void set_uniform(int uniform, const std::array<int, 3>& value);
        void set_uniform(int uniform, const std::array<int, 4>& value);

    };
}