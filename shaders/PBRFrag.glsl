#version 430 core

// Modifying this from code before loading shader
#define dirLightCount 0
#define pLightCount 0

layout(location = 0) out vec4 fragColor;
layout(location = 1) out int idColor;

layout(binding=1) uniform sampler2D albedoMap;
layout(binding=2) uniform sampler2D roughnessMap;
layout(binding=3) uniform sampler2D normalMap;
layout(binding=4) uniform sampler2D aoMap;
layout(binding=5) uniform sampler2D metallicMap;
layout (binding = 6) uniform sampler2DArray shadowMap;
layout (binding = 7) uniform samplerCubeArray pointShadowMap;
layout (binding = 8) uniform samplerCube irradianceMap;
layout (binding = 9) uniform samplerCube prefilterMap;
layout (binding = 10) uniform sampler2D brdfLUT;

#if dirLightCount != 0
uniform vec3 dirLightDirs[dirLightCount];
uniform vec3 dirLightColors[dirLightCount];
uniform float dirLightIntensities[dirLightCount];
#endif

#if pLightCount != 0
uniform vec3 pLightPos[pLightCount];
uniform vec3 pLightColors[pLightCount];
uniform float pLightIntensities[pLightCount];
#endif

uniform int objectID;

uniform vec3 camPos;

uniform float ambientIntensity;

in vec3 Normal;
in vec2 UV;
in vec3 WorldPos;
#if dirLightCount != 0
in vec4 LightSpacePos[dirLightCount];
#endif
//flat in int v_objectID;

const float PI = 3.14159265359;

vec3 getNormalFromNormalMap()
{
    vec3 tangentNormal = texture(normalMap, UV).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(UV);
    vec2 st2 = dFdy(UV);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   

vec3 PowerVec3(vec3 color, float power)
{
    vec3 v;
    v.x = pow(color.x, power);
    v.y = pow(color.y, power);
    v.z = pow(color.z, power);
    return v;
}

float ShadowCalculation(vec4 fragPosLightSpace, int index)
{
    float bias = 0.025;
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float shadow = 0;
    if(projCoords.z > 1.0)
        return shadow;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    //float closestDepth = texture(shadowMap, vec3(projCoords.xy, index)).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    vec2 texelSize = vec3(1.0 / textureSize(shadowMap, 0)).xy;
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, vec3(projCoords.xy+vec2(x, y)*texelSize*0.5, index)).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    return shadow;
}

#if pLightCount != 0
vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
); 
float PointShadowCalculation(vec3 fragPos, int index)
{
    float far_plane = 25.0;
    float shadow = 0.0;
    vec3 fragToLight = fragPos - pLightPos[index];
    float currentDepth = length(fragToLight);
    
    float closestDepth = texture(pointShadowMap, vec4(fragToLight, index)).r;
    int samples = 20;
    float diskRadius = (1.0 + (length( camPos - fragPos ) / far_plane)) / 25.0 * 0.1;
    float bias = 0.0125;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(pointShadowMap, vec4(fragToLight + sampleOffsetDirections[i] *
                    diskRadius, index)).r;
        closestDepth *= 25.0;   //far_plane
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    return shadow /= float(samples);
}
#endif

void main()
{
    vec3 albedo     = PowerVec3( texture(albedoMap, UV).rgb, 2.2);
    vec3 normal     = getNormalFromNormalMap();
    float metallic  = texture(metallicMap, UV).r;
    float roughness = texture(roughnessMap, UV).r;
    float ao        = texture(aoMap, UV).r;

    vec3 N = /*normalize(Normal)*/ normal;
    vec3 V = normalize(camPos - WorldPos);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 Lo;
    #if pLightCount != 0
    for(int i = 0; i < pLightCount; ++i) //point lights
    {
        vec3 L = normalize(pLightPos[i] - WorldPos);
        vec3 H = normalize(V + L);
    
        float dist    = length(pLightPos[i] - WorldPos);
        float attenuation = 1.0 / (dist * dist);
        vec3 radiance     = pLightColors[i] * attenuation; 

        float NDF = DistributionGGX(N, H, roughness);        
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular     = numerator / denominator;  
            
        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);                
        Lo += (kD * albedo / PI + specular) * radiance * NdotL *
                pLightIntensities[i] * ( 1 - PointShadowCalculation(WorldPos, i) ); 
    }
    #endif

    #if dirLightCount != 0
    for(int i = 0; i < dirLightCount; ++i) //dir lights
    {
        vec3 L = normalize(dirLightDirs[i]);
        vec3 H = normalize(V + L);
    
        //float dist    = length(pLightPos[i] - WorldPos);
        //float attenuation = 1.0 / (dist * dist);
        vec3 radiance     = dirLightColors[i]; 

        float NDF = DistributionGGX(N, H, roughness);        
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular     = numerator / denominator;  
            
        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);                
        Lo += (kD * albedo / PI + specular) * radiance * NdotL
            * dirLightIntensities[i] * (1-ShadowCalculation(LightSpacePos[i], i)); 
    }
    #endif

    vec3 R = reflect(-V, N);   
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

    vec3 kS = F; 
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse    = irradiance * albedo;

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 envBRDF  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

    vec3 ambient    = (kD * diffuse + specular) * ao * ambientIntensity; 

    vec3 color = ambient + Lo;
	
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  
   
    //fragColor = vec4(normal, 1.0);
    fragColor = vec4(color, 1.0);
    //fragColor = vec4(albedo, 1.0);
    idColor = objectID;
}