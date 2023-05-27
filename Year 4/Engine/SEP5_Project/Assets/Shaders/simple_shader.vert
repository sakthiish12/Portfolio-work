#version 450
#extension GL_EXT_shader_explicit_arithmetic_types_int64  : enable
//change to use uint32, 80x faster

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec3 bittangent;
layout(location = 6) in ivec4 bone_idxs;
layout(location = 7) in vec4 bone_weights;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;
layout(location = 3) out vec2 fragTexCoord;
layout(location = 4) out vec3 rayDirOut;
layout(location = 8) out vec4 fragFinalPos;

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

vec3 rayDirFromCamera()
{
	vec2 newMousePos = ubo.mousePos.xy;
	newMousePos.x -= ubo.vpData.x;
	newMousePos.y -= ubo.vpData.y;
	newMousePos.x = newMousePos.x/ubo.vpData.z * ubo.frameSize.x;
	newMousePos.y = newMousePos.y/ubo.vpData.w * ubo.frameSize.y;
	
	const float mx = 2.0 * (newMousePos.x/ubo.frameSize.x - 0.5) / ubo.projection[0][0];
	const float my = 2.0 * (newMousePos.y/ubo.frameSize.y -0.5) / ubo.projection[1][1];

	return vec3(ubo.view[0][2] + mx * ubo.view[0][0] + my * ubo.view[0][1],
	ubo.view[1][2] + mx * ubo.view[1][0] + my * ubo.view[1][1],
	ubo.view[2][2] + mx * ubo.view[2][0] + my * ubo.view[2][1]);
}

bool getBool(int storage, int index)
{
	return bool(storage & (1<<index));
}

void main() 
{
	vec4 positionToUse;
	vec3 normalToUse;

	if(getBool(push.hasAnimShouldFlickerWireFrameModeIgnoreLighting,0))
	{
		mat4 bone_transform = mat4(0.0);
		
		if(bone_weights[0] > 0)
		{
			bone_transform += ubo.finalBonesMatrices[bone_idxs[0]] * bone_weights[0];
		}
		if(bone_weights[1] > 0)
		{
			bone_transform += ubo.finalBonesMatrices[bone_idxs[1]] * bone_weights[1];
		}
		if(bone_weights[2] > 0)
		{
			bone_transform += ubo.finalBonesMatrices[bone_idxs[2]] * bone_weights[2];
		}
		if(bone_weights[3] > 0)
		{
			bone_transform += ubo.finalBonesMatrices[bone_idxs[3]] * bone_weights[3];
		}
		
		positionToUse = push.modelMatrix  *bone_transform*vec4(position,1.0);
		normalToUse = mat3(push.normalMatrix)*mat3(transpose(inverse(bone_transform)))*normal;
	}
	else
	{
		positionToUse = push.modelMatrix * vec4(position, 1.0);
		normalToUse   = mat3(push.normalMatrix)*normal;
	}
	
	fragPosWorld 		= positionToUse.xyz;
	fragNormalWorld 	= normalize(normalToUse);
	fragColor 			= color;
	fragTexCoord 		= vec2(uv.s,1.0-uv.t);
	rayDirOut           = normalize(rayDirFromCamera());
	gl_Position = ubo.projection * ubo.view* vec4(fragPosWorld,1.0f);
	fragFinalPos = gl_Position;
}