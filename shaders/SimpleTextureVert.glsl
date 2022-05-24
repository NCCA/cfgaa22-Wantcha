#version 430 core

layout(location = 0) in vec3 position;
//layout(location = 1) in vec2 uv;

out vec2 UV;

void main()
{
    gl_Position = vec4(0, 0, 0, 1.0);//vec4(position.xyz, 1.0);
    //UV = uv;
}