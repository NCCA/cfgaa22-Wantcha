#version 430 core

layout(location = 0) out vec4 fragColor;
uniform sampler2D colorTexture;
in vec2 UV;
void main()
{
    fragColor = /*texture(colorTexture, UV)*/vec4(1,0,1,1);
}