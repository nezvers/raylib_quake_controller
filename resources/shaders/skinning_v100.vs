#version 100

#define MAX_BONE_NUM 32

// Input vertex attributes
attribute vec3 vertexPosition;
attribute vec2 vertexTexCoord;
attribute vec3 vertexNormal;
attribute vec4 vertexColor;
attribute vec4 vertexBoneIds;
attribute vec4 vertexBoneWeights;

// Input uniform values
uniform mat4 mvp;
uniform mat4 matNormal;
uniform mat4 boneMatrices[MAX_BONE_NUM];

// Output vertex attributes (to fragment shader)
varying vec2 fragTexCoord;
varying vec4 fragColor;
varying vec3 fragNormal;

void main()
{
    int boneIndex0 = int(vertexBoneIds.x);
    int boneIndex1 = int(vertexBoneIds.y);
    int boneIndex2 = int(vertexBoneIds.z);
    int boneIndex3 = int(vertexBoneIds.w);
    
    vec4 skinnedPosition =
        vertexBoneWeights.x*(boneMatrices[boneIndex0]*vec4(vertexPosition, 1.0)) +
        vertexBoneWeights.y*(boneMatrices[boneIndex1]*vec4(vertexPosition, 1.0)) + 
        vertexBoneWeights.z*(boneMatrices[boneIndex2]*vec4(vertexPosition, 1.0)) + 
        vertexBoneWeights.w*(boneMatrices[boneIndex3]*vec4(vertexPosition, 1.0));

    vec4 skinnedNormal =
        vertexBoneWeights.x*(boneMatrices[boneIndex0]*vec4(vertexNormal, 0.0)) +
        vertexBoneWeights.y*(boneMatrices[boneIndex1]*vec4(vertexNormal, 0.0)) + 
        vertexBoneWeights.z*(boneMatrices[boneIndex2]*vec4(vertexNormal, 0.0)) + 
        vertexBoneWeights.w*(boneMatrices[boneIndex3]*vec4(vertexNormal, 0.0));
    skinnedNormal.w = 0.0;

    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;

    fragNormal = normalize(vec3(matNormal*skinnedNormal));

    gl_Position = mvp*skinnedPosition;
}