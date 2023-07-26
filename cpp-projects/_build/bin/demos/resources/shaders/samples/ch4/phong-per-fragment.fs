#version 440

in vec3 Position;
in vec3 Normal;

uniform struct LightInfo{
    vec4 Position; // Light position in eye coords.
    vec3 La;       // Ambient light intensity
    vec3 L;        // Diffuse and Specular light intensity
} Light;


layout (binding = 0) uniform MaterialInfo {
  vec3 Ka;            // Ambient reflectivity
  vec3 Kd;            // Diffuse reflectivity
  vec3 Ks;            // Specular reflectivity
  float Shininess;    // Specular shininess factor
} Material;


layout( location = 0 ) out vec4 FragColor;

vec3 phong_model(vec3 position, vec3 n ) {

    vec3 ambient = Light.La * Material.Ka;

    vec3 s;
    if(Light.Position.w == 0.0){
        s = normalize(Light.Position.xyz);
    }else{
        s = normalize( Light.Position.xyz - position );
    }


    float sDotN = max( dot(s,n), 0.0 );
    vec3 diffuse = Material.Kd * sDotN;
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 ) {
        vec3 v = normalize(-position.xyz);
        vec3 r = reflect( -s, n );
        spec = Material.Ks *pow( max( dot(r,v), 0.0 ), Material.Shininess );
    }

    return ambient + Light.L * (diffuse + spec);
}


void main() {

    FragColor = vec4(phong_model(Position, Normal ), 1.0);
}
