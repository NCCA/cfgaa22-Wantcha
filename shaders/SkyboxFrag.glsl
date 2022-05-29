#version 430 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int idColor;

in vec3 localPos;
  
uniform samplerCube environmentMap;
  
void main()
{
    vec3 envColor = texture(environmentMap, localPos).rgb/*textureLod(environmentMap, localPos, 1).rgb*/;
    
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 
  
    FragColor = vec4(envColor, 1.0);
    idColor = -100;
}