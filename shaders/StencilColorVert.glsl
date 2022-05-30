#version 430 core

layout(location = 0) in vec3 position;
layout (location = 1) in vec3 inNormal;

uniform mat4 MVP;


void main()
{
    gl_Position = MVP * vec4(position + inNormal * 0.0125, 1.0);
}