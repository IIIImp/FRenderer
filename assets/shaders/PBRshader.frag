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

const float PI = 3.14159265359;

// 法线分布函数 (GGX/Trowbridge-Reitz)
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float denom = (NdotH * NdotH * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}
// 几何函数 (Schlick-GGX)
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

// 几何遮挡 (Smith方法)
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

// 菲涅尔方程 (Schlick近似)
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

mat3 TBN()
{
    vec3 pos_dx = dFdx(fragPosition);
    vec3 pos_dy = dFdy(fragPosition);
    vec3 st1    = dFdx(vec3(fragTexCoord, 0.0));
    vec3 st2    = dFdy(vec3(fragTexCoord, 0.0));
    vec3 T      = (st2.t * pos_dx - st1.t * pos_dy) / (st1.s * st2.t - st2.s * st1.t);
    vec3 N      = normalize(fragNormal);
    T           = normalize(T - N * dot(N, T));
    vec3 B      = normalize(cross(N, T));
    mat3 TBN    = mat3(T, B, N);

    return TBN;
}

void main() {
    vec4 baseColor = texture(texSampler, fragTexCoord);
    vec4 metallicRoughness = texture(metallicRoughnesstexSampler, fragTexCoord);
    vec3 normalMap = texture(normaltexSampler, fragTexCoord).rgb * 2.0 - 1.0; // 解压法线 [0,1] -> [-1,1]
    vec3 emissive = texture(emissivetexSampler, fragTexCoord).rgb;
    mat3 TBN = TBN();

    // 材质参数
    vec3 albedo = baseColor.rgb;
    float metallic = metallicRoughness.b;    // 假设Metallic在B通道
    float roughness = metallicRoughness.g;   // 假设Roughness在G通道

    // 转换法线到世界空间
    vec3 N = normalize(TBN * normalMap);
    vec3 V = normalize(light.viewPos - fragPosition);

    // 计算反射率F0（基础反射率）
    vec3 F0 = mix(vec3(0.04), albedo, metallic); // 非金属F0=0.04，金属F0=albedo

    // 点光源计算
    vec3 L = normalize(light.lightPosition - fragPosition);
    vec3 H = normalize(V + L);
    float distance = length(light.lightPosition - fragPosition);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = light.lightColor * light.intensity * attenuation;

    // Cook-Torrance BRDF计算
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    // 镜面反射部分
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    // 漫反射部分（仅非金属贡献）
    vec3 kS = F; // 反射光比例
    vec3 kD = (1.0 - kS) * (1.0 - metallic); // 折射光比例

    // 最终光贡献
    vec3 Lo = (kD * albedo / PI + specular) * radiance * max(dot(N, L), 0.0);

    // 自发光直接叠加
    vec3 finalColor = Lo + emissive;

    // 输出（Gamma校正可选）
    outColor = vec4(finalColor, 1.0);
}