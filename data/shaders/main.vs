#version 150

in vec2 position;

void main(void)
{
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}