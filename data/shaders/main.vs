#version 330

in vec3 position;
in vec2 vertexUV;
in vec4 color;

in int matrixIndex;

out vec2 UV;
out vec4 fragColor;

uniform mat4 proj;

uniform samplerBuffer matrixBuffer;

mat4 read_matrix(int offset)
{
    return mat4(texelFetch(matrixBuffer, offset),
        texelFetch(matrixBuffer, offset + 1),
        texelFetch(matrixBuffer, offset + 2),
        texelFetch(matrixBuffer, offset + 3));
}

void main(void)
{
    gl_Position = proj * read_matrix(matrixIndex * 4) * vec4(position, 1.0);
    UV = vertexUV;
    fragColor = color;
}