#version 330

in vec3 position;
in vec2 vertexUV;
in vec4 color;

out vec2 UV;
out vec4 fragColor;

uniform mat4 ortho;

uniform samplerBuffer matrixBuffer;
uniform usamplerBuffer matrixIndices;

mat4 read_matrix(int offset)
{
    return mat4(texelFetch(matrixBuffer, offset), texelFetch(matrixBuffer, offset + 1), texelFetch(matrixBuffer, offset + 2), texelFetch(matrixBuffer, offset + 3));
}

void main(void)
{
	int faceID = int(gl_VertexID/3);
	int matrixOffset = int(texelFetch(matrixIndices, faceID).r) * 4;
	gl_Position = ortho * read_matrix(matrixOffset) * vec4(position, 1.0);
	UV = vertexUV;
	fragColor = color;
}