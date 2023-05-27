#version 450
#extension GL_EXT_shader_explicit_arithmetic_types_int64  : enable
//change to use uint32, 80x faster

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;
layout (location = 3) in vec2 fragTexCoord;
layout (location = 4) in vec3 rayDirIn;
layout (location = 8) in vec4 fragFinalPos;

layout (location = 0) out vec4 outColor;

layout (binding = 1)    uniform     sampler2D 	albedoMap;
layout (binding = 2)    uniform     sampler2D   normalMap;	
layout (binding = 3)    uniform     sampler2D   aoMap;	
layout (binding = 4)    uniform     sampler2D   roughnessMap;			
layout (binding = 5)    uniform     sampler2D   emissiveMap;		
layout (binding = 6)    uniform     sampler2D   metallicMap;	
layout (binding = 7)    uniform     sampler2D   heightMap;	

layout (binding = 10) buffer IdOutput {
	uint64_t selectedID;
	float distance;
} idOut;

struct PointLight {
  vec4 position; // ignore w
  vec4 color; // w is intensity
  float radius;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  mat4 invView;
  mat4 finalBonesMatrices[30];
  vec4 ambientLightColor; // w is intensity
  PointLight pointLights[100];
  int numLights;
  vec4 cameraWorldPos;
  vec4 mousePos;
  vec4 vpData;
  vec4 frameSize;
  int mouseState;
} ubo;

layout(push_constant) uniform Push {
  mat4 modelMatrix;
  mat4 normalMatrix;
  vec4 flickerColor;
  vec4 wireframeColor;
  float albedoStrength;
  float aoStrength;
  float emissiveStrength;
  float metallicStrength;
  float normalStrength;
  float roughnessStrength;
  float opacity;
  float brightnessFactor;
  int hasAnimShouldFlickerWireFrameModeIgnoreLighting;
  int reflectSpecularCustomAlbedoCustomNormalCustomAO;
  int customRoughnessCustomEmissiveCustomMetallicCustomHeight;
  uint64_t id;
} push;

bool getBool(int storage, int index)
{
	return bool(storage & (1<<index));
}

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal 
// mapping the usual way for performance anways; I do plan make a note of this 
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap()
{
    if(getBool(push.reflectSpecularCustomAlbedoCustomNormalCustomAO,2)== false)
        return fragNormalWorld;

    // Get the tangent-space normal from the normal map texture
    const vec3 tangentNormal = texture(normalMap, fragTexCoord).rgb * push.normalStrength * 2.0 - 1.0;

    // Compute the tangent and bitangent vectors
    const vec3 Q1  = dFdx(fragPosWorld);
    const vec3 Q2  = dFdy(fragPosWorld);
    const vec2 st1 = dFdx(fragTexCoord);
    const vec2 st2 = dFdy(fragTexCoord);
    const vec3 N   = fragNormalWorld;
    const vec3 T   = Q1*st2.t - Q2*st1.t;
    const vec3 B   = -normalize(cross(N, T));

    // Compute the TBN matrix
    const mat3 TBN = mat3(T, B, N);

    // Transform the tangent-space normal to world space
    return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
float DistributionGGX(const vec3 N, const vec3 H, const float roughness)
{
    const float a = roughness*roughness;
    const float a2 = a*a;
    const float NdotH = max(dot(N, H), 0.0);
    const float NdotH2 = NdotH*NdotH;

    const float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(const float NdotV, const float roughness)
{
    const float r = (roughness + 1.0);
    const float k = (r*r) * 0.125f;

    const float nom   = NdotV;
    const float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(const vec3 N, const vec3 V, const vec3 L, const float roughness)
{
    const float NdotV = max(dot(N, V), 0.0);
    const float NdotL = max(dot(N, L), 0.0);
    const float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    const float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(const float cosTheta, const vec3 F0)
{
    const float value = clamp(1.0 - cosTheta, 0.0, 1.0);

    return F0 + (1.0 - F0) *(value*value*value*value*value);
}
// ----------------------------------------------------------------------------


void main() 
{
	const float visibility = step(0.0, fragFinalPos.w + abs(fragFinalPos.x)) *
                 step(0.0, fragFinalPos.w + abs(fragFinalPos.y)) *
                 step(0.0, fragFinalPos.w + abs(fragFinalPos.z));
				 
	if(visibility == 0.0)
	{
		discard;
	}
				 
    if(getBool(push.hasAnimShouldFlickerWireFrameModeIgnoreLighting,2))
    {
        outColor = push.wireframeColor;
    }
    else
    {
        if(getBool(push.hasAnimShouldFlickerWireFrameModeIgnoreLighting,3) == false)
        {
            //start of pbr
            const vec3 albedo = getBool(push.reflectSpecularCustomAlbedoCustomNormalCustomAO,1)? pow(texture(albedoMap,fragTexCoord).rgb,vec3(2.2)) * push.albedoStrength : vec3(0.0,0.0,0.0);
            const float metallic = getBool(push.customRoughnessCustomEmissiveCustomMetallicCustomHeight,2)? texture(metallicMap,fragTexCoord).r * push.metallicStrength : 0.0;
            const float roughness = getBool(push.customRoughnessCustomEmissiveCustomMetallicCustomHeight,0)? texture(roughnessMap, fragTexCoord).r * push.roughnessStrength : 0.0;
            const float ao        = getBool(push.reflectSpecularCustomAlbedoCustomNormalCustomAO,3)? texture(aoMap, fragTexCoord).r * push.aoStrength : 1.0;
            const float height   = getBool(push.customRoughnessCustomEmissiveCustomMetallicCustomHeight,3)? texture(heightMap,fragTexCoord).r * push.normalStrength : 0.0;

            vec3 F0 = vec3(0.04);
            F0 = mix(F0,albedo,metallic);
            vec3 Lo = vec3(0.0);
            const vec3 N = getNormalFromMap();
            const vec3 V = normalize(ubo.cameraWorldPos.xyz - fragPosWorld); //previously using ubo.invview[3].xyz

            for(int i=0;i<ubo.numLights;++i)
            {

                const vec3 L = normalize(ubo.pointLights[i].position.xyz - fragPosWorld);
                const vec3 H = normalize(V + L);

                const float distance = length(ubo.pointLights[i].position.xyz - fragPosWorld);
                const float attenuation = 1.0 / (distance * distance);

                const vec3 radiance = (ubo.pointLights[i].color.xyz * ubo.pointLights[i].color.w) * attenuation * ubo.pointLights[i].radius;

                // Cook-Torrance BRDF
                const float NDF = DistributionGGX(N, H, roughness);   
                const float G   = GeometrySmith(N, V, L, roughness);      
                const vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
                
                const vec3 numerator    = NDF * G * F; 
                const float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
                const vec3 specular = numerator / denominator;
                
                // kS is equal to Fresnel
                const vec3 kS = F;
                
                // for energy conservation, the diffuse and specular light can't
                // be above 1.0 (unless the surface emits light); to preserve this
                // relationship the diffuse component (kD) should equal 1.0 - kS.
                vec3 kD = vec3(1.0) - kS;
                
                // multiply kD by the inverse metalness such that only non-metals 
                // have diffuse lighting, or a linear blend if partly metal (pure metals
                // have no diffuse light).
                kD *= 1.0 - metallic;	  

                // scale light by NdotL
                const float NdotL = max(dot(N, L), 0.0);        

                // add to outgoing radiance Lo
                //currently commented out since no proper map or bugged
                if(getBool(push.reflectSpecularCustomAlbedoCustomNormalCustomAO,0))
                    Lo += (kD * albedo / PI + specular * (getBool(push.customRoughnessCustomEmissiveCustomMetallicCustomHeight,3)? height : 1.0)) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
                else
                    Lo += (kD * albedo / PI) * radiance * NdotL; 
            }

            // ambient lighting (note that the next IBL tutorial will replace 
            // this ambient lighting with environment lighting).
            const vec3 ambient = (ubo.ambientLightColor.xyz * ubo.ambientLightColor.w) * albedo * ao * (getBool(push.customRoughnessCustomEmissiveCustomMetallicCustomHeight,3)? height : 1.0);
            vec3 color = ambient + Lo;

            // HDR tonemapping
            color = color / (color + vec3(1.0));
            // gamma correct
            color = pow(color, vec3(1.0/2.2)); 
            
            if(getBool(push.hasAnimShouldFlickerWireFrameModeIgnoreLighting,1))
            {
                outColor = vec4(push.flickerColor.xyz, push.opacity);
            }
            else
            {
                outColor = vec4(
                    (color + (getBool(push.customRoughnessCustomEmissiveCustomMetallicCustomHeight,1)?
                    texture(emissiveMap,fragTexCoord).rgb * push.emissiveStrength : vec3(0.0)))*push.brightnessFactor, 
                push.opacity*(getBool(push.reflectSpecularCustomAlbedoCustomNormalCustomAO,1)? texture(albedoMap,fragTexCoord).a:1.0));
            }
            //end of pbr
        }
        else
        {
            const vec3 albedo = getBool(push.reflectSpecularCustomAlbedoCustomNormalCustomAO,1)? pow(texture(albedoMap,fragTexCoord).rgb,vec3(2.2)) * push.albedoStrength : vec3(0.0,0.0,0.0);
            const float metallic = getBool(push.customRoughnessCustomEmissiveCustomMetallicCustomHeight,2)? texture(metallicMap,fragTexCoord).r * push.metallicStrength : 0.0;
            const float roughness = getBool(push.customRoughnessCustomEmissiveCustomMetallicCustomHeight,0)? texture(roughnessMap, fragTexCoord).r * push.roughnessStrength : 0.0;
            const float ao        = getBool(push.reflectSpecularCustomAlbedoCustomNormalCustomAO,3)? texture(aoMap, fragTexCoord).r * push.aoStrength : 1.0;
            const float height   = getBool(push.customRoughnessCustomEmissiveCustomMetallicCustomHeight,3)? texture(heightMap,fragTexCoord).r * push.normalStrength : 0.0;

            vec3 F0 = vec3(0.04);
            F0 = mix(F0,albedo,metallic);
            vec3 Lo = vec3(0.0);

            vec3 ambient = (ubo.ambientLightColor.xyz * ubo.ambientLightColor.w) * albedo * ao *(getBool(push.customRoughnessCustomEmissiveCustomMetallicCustomHeight,3)? height : 1.0);
            // HDR tonemapping
            ambient = ambient / (ambient + vec3(1.0));
            // gamma correct
            ambient = pow(ambient, vec3(1.0/2.2)); 
            outColor = vec4((ambient + (getBool(push.customRoughnessCustomEmissiveCustomMetallicCustomHeight,1)?
            texture(emissiveMap,fragTexCoord).rgb * push.emissiveStrength : vec3(0.0)))*push.brightnessFactor, push.opacity
            *texture(albedoMap,fragTexCoord).a);
        }

        const float distCamToFragment = abs(distance(ubo.cameraWorldPos.xyz,fragPosWorld));
        const vec3 potentialIntersectionPoint = ubo.cameraWorldPos.xyz + distCamToFragment*rayDirIn;
        const float absDistance = abs(distance(potentialIntersectionPoint,fragPosWorld));

        //upon clicking, find closest object
        if(absDistance<=0.01f && ubo.mouseState == 1 && absDistance<idOut.distance)
        {
            idOut.selectedID=push.id;
            idOut.distance = absDistance;
        } //If MB1 is held down, don't change
        else if(ubo.mouseState == 3) //upon release of MB1, reset data
        {
            idOut.selectedID = 0;
            idOut.distance = 3.402823466e+38; //FLT_MAX
        }
    }
}