#pragma once
#include <glm/glm.hpp>

namespace ORCore
{
    struct Vertex
    {
        // Use vec here in-case we want to easily transform verts cpu-side.
        glm::vec3 vertex;
        glm::vec2 uv;
        glm::vec4 color;
    };
    
    enum class States
    {
        program,
        texture,
        point_size,
        blend_mode,
        primitive
    };

    enum class Primitive
    {
        point,
        line,
        triangle
    };

    struct RenderState
    {
        int program;
        int texture;
    };

    struct Mesh
    {
        glm::vec3 scale;
        glm::vec3 translate;
        Primitive primitive;
        int transformOffset;
        int verticesOffset;
        int vertexSize; // Number of vertices used for the primitive type of this mesh. points = 1, lines = 2, triangles = 3
        std::vector<Vertex> vertices;
    };

}