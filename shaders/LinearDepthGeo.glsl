#version 430 core

#define MAX_CUBEMAP_COUNT 4
#define MAX_VERTICES 72//4*18

uniform int cubemapCount;

layout(triangles) in;

layout (triangle_strip, max_vertices=MAX_VERTICES) out;

uniform mat4 PointSpaceMats[24];

out vec4 FragPos;
flat out int pLightPositionIndex;

void main()
{
    for(int c = 0; c < cubemapCount; ++c)
    {
        for(int face = 0; face < 6; ++face)
        {
            int faceLayer = c*6 + face;
            
            for(int i = 0; i < 3; ++i) // for each triangle vertex
            {
                gl_Layer = faceLayer; // built-in variable that specifies to which face we render.
                FragPos = gl_in[i].gl_Position;
                pLightPositionIndex = c;
                gl_Position = PointSpaceMats[faceLayer] * gl_in[i].gl_Position;
                EmitVertex();
            }    
            EndPrimitive();
        }
    }
}  