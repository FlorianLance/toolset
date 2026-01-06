#version 430 core
out vec4 FragColor;

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout (binding=0) uniform sampler2D Tex1;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main(){
	
    //float depth = LinearizeDepth(gl_FragCoord.z) / far;
    //FragColor = vec4(vec3(depth), 1.0);
    //FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
    //return;
    //if(gl_FragCoord.x < 200)
        //FragColor = vec4(textCoord.x,textCoord.y,0,1);
    //return;
    //else if(gl_FragCoord.x < 400)
    //	FragColor = vec4()
    //		FragColor = vec4(normal.x,normal.y,0,1);


    vec4 textureColour = texture(texture_diffuse1, textCoord);

    //vec4(texture(texture_diffuse1, textCoord).y,1,0,1);
    //
    //FragColor= vec4(1-textureColour.w,0,0,1);
    //return;

    if(textureColour.a < 1.0) {
        //FragColor= vec4(1,0,0,1);
        //return;
        //discard;
    }

    //FragColor= vec4(textureColour.r,0,0,1);
    FragColor= textureColour;
}
