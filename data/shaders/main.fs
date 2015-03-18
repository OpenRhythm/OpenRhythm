#version 150

in vec2 UV;

out vec4 outputColor;

uniform sampler2D textureSampler;

void main()
{
	outputColor = texture(textureSampler, UV);
}