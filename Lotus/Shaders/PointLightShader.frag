#version 450

layout (location = 0) in vec2 fragOffset;
layout (location = 0) out vec4 outColor;

struct PointLight {
	vec3 position;
	vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
	mat4 projectionMatrix;
	mat4 viewMatrix;
	vec4 ambientLightColor; // w is intensity
	PointLight pointLights[10];
	int numPointLights;
} ubo;

layout(push_constant) uniform PushConstants {
	vec4 lightPosition;
	vec4 lightColor;
	float lightRadius;
} pushConstants;

const float PI = 3.14159265359;
void main() {
	float distance = sqrt(dot(fragOffset, fragOffset));
	if (distance > 1.0) 
	{
		discard;
	}
	float cosDis = 0.5 * (cos(distance*PI) + 1.0);
	outColor = vec4(pushConstants.lightColor.xyz + cosDis, cosDis);
}