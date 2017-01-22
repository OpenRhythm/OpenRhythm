#version 150

in vec2 position;
in vec2 vertexUV;
in uint matrixIndex;

out vec2 UV;

uniform mat4 ortho;
uniform mat4 models[64]; // batch size of 64

void main(void)
{
	gl_Position = ortho * models[matrixIndex] * vec4(position, 0.0, 1.0);
	UV = vertexUV;
}