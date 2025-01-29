#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPosition;

layout(binding = 1) uniform sampler2D texSampler;

layout(std140, binding = 2) uniform lightInform {
	vec3 lightPosition;
	vec3 lightColor;
	vec3 viewPos;
	float intensity;
}light;

layout(binding = 3) uniform sampler2D emissivetexSampler;
layout(binding = 4) uniform sampler2D metallicRoughnesstexSampler;
layout(binding = 5) uniform sampler2D normaltexSampler;

void main() {
	vec3 viewPos = light.viewPos;
	vec3 norm = normalize(fragNormal);
	vec3 lightDir = normalize(light.lightPosition - fragPosition);
	vec3 viewDir = normalize(viewPos - fragPosition);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	float diff = max(dot(norm, lightDir), 0.0);

	float spec = pow(max(dot(norm, halfwayDir), 0.0), 39.999996);
	vec3 specular = 1.0 * spec * light.lightColor;

	vec3 diffuse = diff * light.lightColor;
	vec3 ambient = 0.1 * light.lightColor;

	vec3 finalColor = (ambient + diffuse + specular) * light.intensity * texture(texSampler, fragTexCoord).rgb;
	outColor = vec4(finalColor, 1.0);
	// outColor = vec4(texture(emissivetexSampler, fragTexCoord).rgb + vec3(0.1, 0.1, 0.1), 1.0);
}