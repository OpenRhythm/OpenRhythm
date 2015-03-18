#version 150

in vec2 position;
in vec2 vertexUV;

out vec2 UV;

uniform mat4 ortho;
uniform mat4 model;

void main(void)
{
	gl_Position = ortho * model * vec4(position, 0.0, 1.0);
	UV = vertexUV;
}