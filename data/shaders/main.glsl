#shader vertex

attribute vec2 position;
attribute vec2 vertexUV;

uniform mat4 ortho;
uniform mat4 model;

void main(void)
{
	gl_Position = ortho * model * vec4(position.x, position.y, 0.0, 1.0);//position;// 
	gl_TexCoord[0] = vec4(vertexUV, 0.0, 0.0);
}

#shader fragment

uniform sampler2D textureSampler;

void main()
{
	gl_FragColor = texture2D(textureSampler, gl_TexCoord[0].st );
}