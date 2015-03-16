#version 150

in vec2 position;

uniform mat4 ortho;

void main(void)
{
	gl_Position = vec4(position, 0.0, 1.0);
}