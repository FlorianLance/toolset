#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D texture1;
//uniform sampler2D texture2;
//uniform sampler2D texture3;
//uniform sampler2D texture4;

void main()
{
	//FragColor = vec4(TexCoord,0,1);
    //FragColor = vec4(Normal,1);//*texture(ourTexture, TexCoord);
    FragColor = 10*texture(texture1, TexCoord);
}
