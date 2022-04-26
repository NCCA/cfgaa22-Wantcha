#version 430 core

uniform vec4 Color;
uniform int objectID;
layout(location = 0) out vec4 fragColor;
layout(location = 1) out int idColor;
void main()
{
    fragColor = Color;
    idColor = objectID;
}