

// Fragment shader:
// ================
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;   // extra in variable, since we need the light position in view space we calculate this in the vertex shader

uniform vec3 lightColor;
uniform vec3 objectColor;


struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;




void main()
{
    // ambient
     vec3 ambient = lightColor * material.ambient;

     // diffuse
     vec3 norm = normalize(Normal);
     vec3 lightDir = normalize(LightPos - FragPos);
     float diff = max(dot(norm, lightDir), 0.0);
     vec3 diffuse = lightColor * (diff * material.diffuse);

     // specular
//     vec3 viewDir = normalize(viewPos - FragPos);
     vec3 viewDir = normalize(-FragPos);
     vec3 reflectDir = reflect(-lightDir, norm);
     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
     vec3 specular = lightColor * (spec * material.specular);

     vec3 result = ambient + diffuse + specular;
     FragColor = vec4(result, 1.0);



//    // ambient
//    float ambientStrength = 0.1;
//    vec3 ambient = ambientStrength * lightColor;

//     // diffuse
//    vec3 norm = normalize(Normal);
//    vec3 lightDir = normalize(LightPos - FragPos);
//    float diff = max(dot(norm, lightDir), 0.0);
//    vec3 diffuse = diff * lightColor;

//    // specular
//    float specularStrength = 0.5;
//    vec3 viewDir = normalize(-FragPos); // the viewer is always at (0,0,0) in view-space, so viewDir is (0,0,0) - Position => -Position
//    vec3 reflectDir = reflect(-lightDir, norm);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
//    vec3 specular = specularStrength * spec * lightColor;

//    vec3 result = (ambient + diffuse + specular) * objectColor;
//    FragColor = vec4(result, 1.0);
}

