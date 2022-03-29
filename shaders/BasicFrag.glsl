#version 430 core

layout(location = 0) out vec4 fragColor;

in vec3 Normal;

void main()
{
    vec3 N = normalize(Normal);

    vec3 lightDir = vec3(0, 1, 0);
    float intensity = max(dot(N, lightDir), 0.0) + 0.2;

    fragColor = vec4(0.2, 0.2, 0.9, 1.0) * intensity;
}