#version 430 core

layout(location = 0) out vec4 fragColor;

layout (binding = 0) uniform sampler2D colorTexture;
in vec2 UV;
void main()
{
    fragColor = texture(colorTexture, UV);//vec4(texture(colorTexture, UV).rgb, 1.0);
}