#version 430 core
out vec4 FragColor;

in vec3 FragPos;
in vec4 FragPosLight;
in vec4 FragPosLightDirectional;
in vec3 Normal;
in vec2 TexCoord;
in vec3 Tangentout;
in vec3 Bitangentout;

uniform vec3 point_light_position;
uniform vec3 point_light_color;
uniform float point_light_intensity;

uniform vec3 directional_light_direction;
uniform vec3 directional_light_color;
uniform float directional_light_intensity;

uniform vec3 spot_light_position;
uniform vec3 spot_light_direction;
uniform vec3 spot_light_color;
uniform float spot_light_intensity;
uniform float spot_light_inner_cone;
uniform float spot_light_outer_cone;

uniform vec3 viewPos;

uniform bool has_diffuse;
uniform bool has_normal;
uniform bool has_metallic_roughness;
uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D metallic_roughness;
uniform sampler2D shadow_map; // spotlight
uniform sampler2D directional_shadow_map;

const float PI = 3.14159265359;

// Normal Distribution Function (GGX/Trowbridge-Reitz)
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return num / denom;
}

// Geometry function (Smith's method)
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    
    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

// Fresnel equation (Schlick's approximation)
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// todo point light shadow calc

float SpotShadowCalculation(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.z > 1.0)
        return 0.0;
    
    float closestDepth = texture(shadow_map, projCoords.xy).r;
    float currentDepth = projCoords.z;
    
    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    
    return shadow;
}

float DirectionalShadowCalculation(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.z > 1.0)
        return 0.0;
    
    float closestDepth = texture(directional_shadow_map, projCoords.xy).r;
    float currentDepth = projCoords.z;
    
    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    
    return shadow;
}

vec3 CalculateLighting(vec3 L, vec3 radiance, vec3 N, vec3 V, vec3 F0, vec3 albedo, float metallic, float roughness) {
    vec3 H = normalize(V + L);
    
    // cook-torrance brdf
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
    
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;
    
    float NdotL = max(dot(N, L), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 CalculatePointLight(vec3 N, vec3 V, vec3 F0, vec3 albedo, float metallic, float roughness) {
    vec3 L = normalize(point_light_position - FragPos);
    float distance = length(point_light_position - FragPos);
    float attenuation = point_light_intensity / (distance * distance);
    vec3 radiance = point_light_color * attenuation;
    
    return CalculateLighting(L, radiance, N, V, F0, albedo, metallic, roughness);
}

vec3 CalculateDirectionalLight(vec3 N, vec3 V, vec3 F0, vec3 albedo, float metallic, float roughness) {
    vec3 L = normalize(-directional_light_direction); // Light direction points towards the light
    vec3 radiance = directional_light_color * directional_light_intensity;
    
    vec3 lighting = CalculateLighting(L, radiance, N, V, F0, albedo, metallic, roughness);
    
    float shadow = DirectionalShadowCalculation(FragPosLightDirectional);
    return lighting * (1.0 - shadow);
}

vec3 CalculateSpotLight(vec3 N, vec3 V, vec3 F0, vec3 albedo, float metallic, float roughness) {
    vec3 L = normalize(spot_light_position - FragPos);
    float distance = length(spot_light_position - FragPos);
    
    vec3 spotDir = normalize(spot_light_direction);
    float theta = dot(L, -spotDir);
    
    float epsilon = spot_light_inner_cone - spot_light_outer_cone;
    float intensity = clamp((theta - spot_light_outer_cone) / epsilon, 0.0, 1.0);
    
    float attenuation = spot_light_intensity / (distance * distance);
    vec3 radiance = spot_light_color * attenuation * intensity;
    
    vec3 lighting = CalculateLighting(L, radiance, N, V, F0, albedo, metallic, roughness);
    
    float shadow = SpotShadowCalculation(FragPosLight);
    return lighting * (1.0 - shadow);
}

void main() { 

//    FragColor = vec4(texture(diffuse, TexCoord).rgb, 1.0);
//    return ;

    vec3 N = normalize(Normal);
    if (has_normal) {
        vec3 normalMap = texture(normal, TexCoord).rgb;
        normalMap = normalMap * 2.0 - 1.0;
        
        vec3 T = normalize(Tangentout);
        vec3 B = normalize(Bitangentout);
        vec3 Norm = normalize(Normal);
        mat3 TBN = mat3(T, B, Norm);
        N = normalize(TBN * normalMap);
    }
    
    vec3 albedo = texture(diffuse, TexCoord).rgb;
    float metallic = 0.0;
    float roughness = 0.5;
    
    if (has_metallic_roughness) {
        vec3 mrSample = texture(metallic_roughness, TexCoord).rgb;
        metallic = mrSample.b;
        roughness = mrSample.g;
    }
    
    // view direction
    vec3 V = normalize(viewPos - FragPos);
    
    // F0
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    
    vec3 Lo = vec3(0.0);
    
    // Add point light contribution
    //Lo += CalculatePointLight(N, V, F0, albedo, metallic, roughness);
    Lo += CalculateDirectionalLight(N, V, F0, albedo, metallic, roughness);
    Lo += CalculateSpotLight(N, V, F0, albedo, metallic, roughness);
    
    vec3 ambient = vec3(0.0001) * albedo;
    
    vec3 color = ambient + Lo;
    
    // HDR tonemapping and gamma correction
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    
    FragColor = vec4(color, 1.0);
}