#pragma once
#include <string>
#include <array>
#include "glm/glm.hpp"

namespace MgCore
{
    struct ShaderInfo
    {
        unsigned int type;
        std::string path;
    };


    struct Shader
    {
        unsigned int shader;
        ShaderInfo info;
        Shader(ShaderInfo);
        ~Shader();
    };

    class ShaderProgram
    {
    private:
        Shader m_vertex;
        Shader m_fragment;
        unsigned int m_program;

    public:
        ShaderProgram(Shader* vertex, Shader* fragment);
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