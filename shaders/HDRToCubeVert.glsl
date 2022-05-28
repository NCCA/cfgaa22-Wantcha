#version 430 core
layout (location = 0) in vec3 aPos;

out vec3 localPos;

//uniform mat4 projection;
uniform mat4 VP;

void main()
{
    localPos = aPos;  
    gl_Position =  VP * vec4(localPos, 1.0);
}