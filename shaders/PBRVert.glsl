#version 430 core

#define dirLightCount 0

layout(location = 0) in vec3 position;

layout (location = 1) in vec3 inNormal;

layout (location = 2) in vec2 inUV;

uniform mat4 MVP;
uniform mat4 M;

#if dirLightCount != 0
uniform mat4 LightSpaceMats[dirLightCount];
#endif

out vec3 Normal;
out vec2 UV;
out vec3 WorldPos;

#if dirLightCount != 0
out vec4 LightSpacePos[dirLightCount];
#endif

void main()
{
    gl_Position = MVP * vec4(position, 1.0);
    vec4 WorldPosVec4 = M * vec4(position, 1.0);
    WorldPos = vec3 ( WorldPosVec4 );
    Normal = /*NormalMatrix * */inNormal;
    UV = inUV;
    
    #if dirLightCount != 0
    for(int i = 0; i< dirLightCount; i++)
    {
        LightSpacePos[i] = LightSpaceMats[i] * WorldPosVec4;
    }
    #endif
}
