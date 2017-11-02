// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#pragma once
#include <glm/glm.hpp>

namespace ORCore
{

    using TextureID = int;
    using ProgramID = int;
    using ObjectID = int;
    using BatchID = int;
    using CameraID = int;

    struct Vertex
    {
        glm::vec3 vertex;
        glm::vec2 uv;
        glm::vec4 color;
        unsigned int matIndex;
    };
    
    enum class States
    {
        program,
        texture,
        point_size,
        blend_mode,
        primitive
    };

    enum Primitive
    {
        nil = -1,
        point = 1,
        line,
        triangle
    };

    // TODO - Turn this into an easily comparable data structure.
    // It is a huge annoyance having to manually add in each new render state into the
    // comparison if statement.
    // Some sort of sparse bit based map could work. Each render state would have a
    // bitmask value accociated with it. This bitmask would be used to quickly compare if two
    // objects have the same elements in the state. If not they arent the same. If they do
    // Then the array of elements will be compared very easily.

    // Each object in the renderstate struct should be based on int's as eventually this will
    // be implemented as an array of ints.

    struct RenderState
    {
        ProgramID program;
        TextureID texture;
        CameraID camera;
        Primitive primitive;
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