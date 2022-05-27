#version 430 core

layout(location = 0) out vec4 fragColor;

//layout (binding = 0) uniform sampler2D colorTexture;
layout (binding = 0) uniform sampler2D colorTexture;
in vec2 UV;
void main()
{
    //float depth = texture(colorTexture, vec3(UV, 0)).r;
    fragColor = /*vec4(vec3(depth), 1.0);*/vec4(texture(colorTexture, UV).rgb, 1.0);
}