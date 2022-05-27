#version 430 core

in vec4 FragPos;

flat in int pLightPositionIndex;
uniform float far_plane;
uniform vec3 pLightPositions[4];

void main()
{
    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - pLightPositions[pLightPositionIndex]);
    
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / far_plane;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
}  