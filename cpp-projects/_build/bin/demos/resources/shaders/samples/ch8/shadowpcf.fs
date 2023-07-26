#version 400

uniform struct LightInfo {
    vec4 Position;
    vec3 Intensity;
} Light;

uniform struct MaterialInfo {
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float Shininess;
} Material;

uniform sampler2DShadow ShadowMap;

in vec3 Position;
in vec3 Normal;
in vec4 ShadowCoord;

layout (location = 0) out vec4 FragColor;

vec3 phongModelDiffAndSpec()
{
    vec3 n = Normal;
    vec3 s = normalize(vec3(Light.Position) - Position);
    vec3 v = normalize(-Position.xyz);
    vec3 r = reflect( -s, n );
    float sDotN = max( dot(s,n), 0.0 );
    vec3 diffuse = Light.Intensity * Material.Kd * sDotN;
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
        spec = Light.Intensity * Material.Ks *
            pow( max( dot(r,v), 0.0 ), Material.Shininess );

    return diffuse + spec;
}

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

subroutine (RenderPassType)
void shadeWithShadow()
{
    vec3 ambient = Light.Intensity * Material.Ka;
    vec3 diffAndSpec = phongModelDiffAndSpec();

    // Lookup the texels nearby
    float sum = 0;
    float shadow = 1.0;

    // Dont' text points behind the light source.
    if(ShadowCoord.z >= 0 ) {
        // Sum contributions from 4 texels around ShadowCoord
        sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(-1,-1));
        sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(-1,1));
        sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(1,1));
        sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(1,-1));
        shadow = sum * 0.25;
    }

    FragColor = vec4( ambient + diffAndSpec * shadow, 1.0 );

    // Gamma correct
    FragColor = pow( FragColor, vec4(1.0 / 2.2) );
    //FragColor = vec4(shadow,shadow,shadow,1.0);
}

subroutine (RenderPassType)
void recordDepth()
{
    // Do nothing, depth will be written automatically
}

void main() {
    // This will call either shadeWithShadow or recordDepth
    RenderPass();
}
