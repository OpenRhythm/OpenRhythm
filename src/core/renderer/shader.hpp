#pragma once
#include <string>
#include <array>
#include <glm/glm.hpp>

namespace ORCore
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
        void init_gl();
        ~Shader();
        void check_error();
    };

    class ShaderProgram
    {
    public:
        ShaderProgram(Shader& vertex, Shader& fragment);
        ~ShaderProgram();

        int get_id();
        
        void check_error();

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

    private:
        Shader m_vertex;
        Shader m_fragment;
        unsigned int m_program;
        int m_programID;

    };
} // namespace ORCore
