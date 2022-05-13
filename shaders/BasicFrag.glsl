#version 430 core

// Modifying this from code before loading shader
const int dirLightCount = 0   ;
const int pLightCount = 0   ;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out int idColor;

uniform vec4 baseColor;
layout(binding=2) uniform sampler2D albedoMap;

uniform vec3 dirLightDirs[dirLightCount];
uniform vec3 dirLightColors[dirLightCount];
uniform float dirLightIntensities[dirLightCount];

uniform vec3 pLightPos[dirLightCount];
uniform vec3 pLightColors[dirLightCount];
uniform float pLightIntensities[dirLightCount];

uniform int objectID;

in vec3 Normal;
in vec2 UV;
//flat in int v_objectID;

void main()
{
    vec3 N = normalize(Normal);
    fragColor = vec4(0.0, 0.0, 0.0, 1.0);
    for(int i = 0; i < dirLightCount; ++i)
    {
        //vec3 lightDir = vec3(0, 1, 0);
        float intensity = max(dot(N, dirLightDirs[i]), 0.0);

        fragColor += baseColor * intensity * vec4(dirLightColors[i], 1.0)
                    * dirLightIntensities[i] * texture(albedoMap, UV).rgba;
    }
    idColor = objectID;
}