#version 110

uniform sampler2D textureSampler;

void main()
{
	gl_FragColor = texture2D(textureSampler, gl_TexCoord[0].st );
}