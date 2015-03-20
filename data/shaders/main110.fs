#version 110

//in attribute vec2 UV;

//out vec4 outputColor;

uniform sampler2D textureSampler;

void main()
{
	gl_FragColor = texture2D(textureSampler, gl_TexCoord[0].st );
}