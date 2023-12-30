#version 450

const vec2 OFFSETS[6] = vec2[](
  vec2(-1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, -1.0),
  vec2(1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, 1.0)
);

layout(location = 0) out vec2 fragOffset;

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

void main() {
	fragOffset = OFFSETS[gl_VertexIndex];
	vec3 cameraRightWorld = { ubo.viewMatrix[0][0], ubo.viewMatrix[1][0], ubo.viewMatrix[2][0] };
	vec3 cameraUpWorld = { ubo.viewMatrix[0][1], ubo.viewMatrix[1][1], ubo.viewMatrix[2][1] };

	vec3 lightPositionWorld = pushConstants.lightPosition.xyz
	+ pushConstants.lightRadius * fragOffset.x * cameraRightWorld
	+ pushConstants.lightRadius * fragOffset.y * cameraUpWorld;

	gl_Position = ubo.projectionMatrix * ubo.viewMatrix * vec4(lightPositionWorld, 1.0);
}