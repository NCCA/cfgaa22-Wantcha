#version 430 core

layout(location = 0) in vec3 position;

layout (location = 1) in vec3 inNormal;

layout (location = 2) in vec2 inUV;

uniform mat4 MVP;
//uniform mat3 NormalMatrix;

out vec3 Normal;

void main()
{
    gl_Position = MVP * vec4(position, 1.0);

    //Normal = NormalMatrix * inNormal;
    Normal = inNormal;
}
