#version 430 core

layout(location = 0) in vec3 position;

layout (location = 1) in vec3 inNormal;

layout (location = 2) in vec2 inUV;

//layout(location = 3) in int objectID;

uniform mat4 MVP;
uniform mat4 M;
uniform mat3 NormalMatrix;
//uniform mat3 NormalMatrix;

out vec3 Normal;
out vec2 UV;
out vec3 WorldPos;
//flat out int v_objectID;

void main()
{
    gl_Position = MVP * vec4(position, 1.0);
    WorldPos = vec3 ( M * vec4(position, 1.0) );
    //Normal = NormalMatrix * inNormal;
    Normal = NormalMatrix * inNormal;
    UV = inUV;
    //v_objectID = objectID;
}
