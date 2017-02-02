#version 330

in vec2 UV;
in vec4 fragColor;

out vec4 outputColor;

uniform sampler2D textureSampler;

void main()
{
	outputColor = fragColor * texture(textureSampler, UV);
}