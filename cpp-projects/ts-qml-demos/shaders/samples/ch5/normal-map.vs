#version 440

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 3) in vec4 VertexTangent;
layout (location = 4) in ivec4 BoneIDs;
layout (location = 5) in vec4 Weights;


uniform struct LightInfo {
    vec4 Position;  // Light position in cam. coords.
    vec3 L;         // D,S intensity
    vec3 La;        // Amb intensity
} Light ;



out vec3 LightDir;
out vec2 TexCoord;
out vec3 ViewDir;

//out vec4 Weights2;
//out ivec4 Id2;
//out vec3 PosL2;

out vec3 TCol;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

//uniform mat4 BonesM;

//uniform mat4 BonesM1;
//uniform mat4 BonesM2;
//uniform mat4 BonesM3;
//uniform mat4 BonesM4;

const int MAX_BONES = 100;
uniform mat4 BonesM[MAX_BONES];



void main(){

    mat4 BoneTransform =BonesM[BoneIDs[0]] * Weights[0];
    BoneTransform += BonesM[BoneIDs[1]] * Weights[1];
    BoneTransform += BonesM[BoneIDs[2]] * Weights[2];
    BoneTransform += BonesM[BoneIDs[3]] * Weights[3];


//    // DEBUG 0
//    // Fox: 22 bones
//    mat4 m0 = BonesM[21];// * Weights[0];
//    mat4 m1 = BonesM[BoneIDs[0]];// * Weights[0];
//    mat4 m2 = BonesM[BoneIDs[1]];// * Weights[1];
//    mat4 m3 = BonesM[BoneIDs[2]] * Weights[2];
//    mat4 m4 = BonesM[BoneIDs[3]] * Weights[3];

//    int c1 = 0;
//    for(int ii = 0; ii < 4; ++ii){
//        for(int jj = 0; jj < 4; ++jj){
//            if(m1[ii][jj] == 0){
//                ++c1;
//            }
//        }
//    }

//    if(c1 == 16){
//        TCol = vec3(0,0,0);
//    }else{
//        TCol = vec3(1,0,0);
//    }

//    BoneTransform = m2;


//    BoneTransform = mat4;

    // DEBUG 1
    if(Weights[0] == 0){
        TCol = vec3(1,0,0);
    }else if(Weights[1] == 0){
        TCol = vec3(0,1,0);
    }else if(Weights[2] == 0){
        TCol = vec3(0,0,1);
    }else if(Weights[3] == 0){
        TCol = vec3(1,0,1);
    }else{
        TCol = vec3(1,1,1);
    }

    // DEBUG 2
//    if(BoneIDs[0] == 0 && Weights[0] == 0){
//        TCol = vec3(1,0,0);
//    }else if(BoneIDs[1] == 0 && Weights[1] == 0){
//        TCol = vec3(0,1,0);
//    }else if(BoneIDs[2] == 0 && Weights[2] == 0){
//        TCol = vec3(0,0,1);
//    }else if(BoneIDs[3] == 0 && Weights[3] == 0){
//        TCol = vec3(1,0,1);
//    }else{
//        TCol = vec3(1,1,1);
//    }

    // DEBUG 3
//    TCol = vec3(Weights[0], Weights[1], Weights[2]);



//    BoneTransform = BonesM[BoneIDs[3]];//mat4(1.0);

//    BoneTransform = BonesM[1];

//    if(BoneIDs[0] == 0){

//    }else if(BoneIDs[0] == 1){

//    }

//    float total = Weights[0]+Weights[1]+Weights[2]+Weights[3];
//    float total2 = BoneIDs[0]+BoneIDs[1]+BoneIDs[2]+BoneIDs[3];
//    PosL2 = vec3(1*BoneIDs[0]/total2, 1*BoneIDs[1]/total2, 1*BoneIDs[2]/total2);
//    if(total > 0.01){
//        PosL2  = vec3(1,0,0);
//    }else{
//        PosL2  = vec3(0,1,0);
//    }

//
//    mat4 BoneTransform = BonesM[BoneIDs[0]] * Weights[0];
//    BoneTransform += BonesM[BoneIDs[1]] * Weights[1];
//    BoneTransform += BonesM[BoneIDs[2]] * Weights[2];
//    BoneTransform += BonesM[BoneIDs[3]] * Weights[3];

//    mat4 BoneTransform =
//                     identity * Weights[0];
//    BoneTransform += identity * Weights[1];
//    BoneTransform += identity * Weights[2];
//    BoneTransform += identity * Weights[3];


//    mat4 BoneTransform = BonesM[BoneIDs[0]] * Weights[0];
//    BoneTransform += BonesM[BoneIDs[1]] * Weights[1];
//    BoneTransform += BonesM[BoneIDs[2]] * Weights[2];
//    BoneTransform += BonesM[BoneIDs[3]] * Weights[3];

//    vec4 PosL = BoneTransform * vec4(Position, 1.0);

//    vec3 PosL     = VertexPosition;//(BoneTransform * vec4(VertexPosition, 1.0)).xyz;
//    vec3 NormalL  = VertexNormal;//(BoneTransform * vec4(VertexNormal, 0.0)).xyz;
//    vec4 TangentL = VertexTangent;//BoneTransform * VertexTangent;

    vec3 PosL     = (BoneTransform * vec4(VertexPosition, 1.0)).xyz;
    vec3 NormalL  = (BoneTransform * vec4(VertexNormal, 0.0)).xyz;
    vec4 TangentL = BoneTransform * VertexTangent;

    // transform normal and tangent to eye space
    vec3 norm = normalize( NormalMatrix * NormalL );
    vec3 tang = normalize( NormalMatrix * vec3(TangentL) );

    // compute the binormal
    vec3 binormal = normalize( cross( norm, tang ) ) * TangentL.w;

    // matrix for transformation to tangent space
    mat3 toObjectLocal = mat3(
        tang.x, binormal.x, norm.x,
        tang.y, binormal.y, norm.y,
        tang.z, binormal.z, norm.z ) ;

    // transform light direction and view direction to tangent space
    vec3 pos = vec3( ModelViewMatrix * vec4(PosL,1.0) );
    LightDir = toObjectLocal * (Light.Position.xyz - pos);

    ViewDir = toObjectLocal * normalize(-pos);

    TexCoord = VertexTexCoord;

    gl_Position = MVP * vec4(PosL,1.0);
}


