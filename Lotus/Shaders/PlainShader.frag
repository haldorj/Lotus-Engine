#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPositionWorld;
layout (location = 2) in vec3 fragNormalWorld;

layout (location = 0) out vec4 outColor;

struct PointLight {
	vec3 position;
	vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
	mat4 projectionMatrix;
	mat4 viewMatrix;
	mat4 inverseViewMatrix;
	vec4 ambientLightColor; // w is intensity
	PointLight pointLights[10];
	int numPointLights;
} ubo;


layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main() {
	vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
	vec3 specularLight = vec3(0.0);
	vec3 surfaceNormal = normalize(fragNormalWorld);

	vec3 cameraPositionWorld = vec3(ubo.inverseViewMatrix[3].xyz);
	vec3 viewDirection = normalize(cameraPositionWorld - fragPositionWorld);

	for (int i = 0; i < ubo.numPointLights; i++)
	{
		PointLight light = ubo.pointLights[i];
		vec3 directionToLight = light.position.xyz - fragPositionWorld;
		float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance squared
		directionToLight = normalize(directionToLight);

		float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0);
		vec3 intensity = light.color.xyz * light.color.w * attenuation;

		diffuseLight += intensity * cosAngIncidence;

		vec3 halfAngle = normalize(directionToLight + viewDirection);
		float blinnTerm = dot(surfaceNormal, halfAngle);
		blinnTerm = max(blinnTerm, 0);
		blinnTerm = pow(blinnTerm, 32.0);
		specularLight += light.color.xyz * attenuation * blinnTerm;
	}

	outColor = vec4(diffuseLight * fragColor + specularLight * fragColor, 1.0);
}