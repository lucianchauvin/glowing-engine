/*
#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec3 Tangentout;
in vec3 Bitangentout;

// Light & camera uniforms
uniform vec3 light_position;
uniform vec3 viewPos;
uniform vec3 light_color;
uniform float light_intensity;

uniform bool has_diffuse;
uniform bool has_normal;
uniform sampler2D diffuse;
uniform sampler2D normal;

void main() {
    vec3 norm = normalize(Normal);

    if (has_normal) {
        vec3 normalMap = texture(normal, TexCoord).rgb;
        normalMap = normalMap * 2.0 - 1.0; // [0,1] -> [-1,1]
        
        // TBN matrix
        vec3 T = normalize(Tangentout);
        vec3 B = normalize(Bitangentout);
        vec3 N = normalize(Normal);
        mat3 TBN = mat3(T, B, N);
        norm = normalize(TBN * normalMap);

     //   vec3 normalSample = texture(normal, TexCoord).rgb;
     //   FragColor = vec4(normalSample, 1.0); // Should show normal map visually
      //  return ;

  }

    // Basic Phong/Blinn-Phong lighting inputs
    vec3 lightDir   = normalize(light_position - FragPos);
    vec3 viewDir    = normalize(viewPos - FragPos);
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse_light = diff * light_color * light_intensity;
    vec3 ambient = 0.15 * light_color;
    
    // Combine them
    vec3 diffuseTex = texture(diffuse, TexCoord).rgb;
    vec3 result = (ambient + diffuse_light) * diffuseTex;

    FragColor = vec4(result, 1.0);
}
*/


#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec3 Tangentout;
in vec3 Bitangentout;

// Light & camera uniforms
uniform vec3 light_position;
uniform vec3 viewPos;
uniform vec3 light_color;
uniform float light_intensity;

// Texture uniforms
uniform bool has_diffuse;
uniform bool has_normal;
uniform bool has_metallic_roughness;
uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D metallic_roughness;

// PBR constants
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

void main() {
    // Get normal
    vec3 N = normalize(Normal);
    if (has_normal) {
        vec3 normalMap = texture(normal, TexCoord).rgb;
        normalMap = normalMap * 2.0 - 1.0; // [0,1] -> [-1,1]
        
        // TBN matrix
        vec3 T = normalize(Tangentout);
        vec3 B = normalize(Bitangentout);
        vec3 Norm = normalize(Normal);
        mat3 TBN = mat3(T, B, Norm);
        N = normalize(TBN * normalMap);
    }
    
    // Get material properties
    vec3 albedo = texture(diffuse, TexCoord).rgb;
    float metallic = 0.0;
    float roughness = 0.5; // Default values
    
    if (has_metallic_roughness) {
        vec3 mrSample = texture(metallic_roughness, TexCoord).rgb;
        // Assuming standard glTF format: R=unused, G=roughness, B=metallic
        // Or adjust based on your texture format
        metallic = mrSample.b;   // Blue channel = metallic
        roughness = mrSample.g;  // Green channel = roughness
    }
    
    // Calculate lighting vectors
    vec3 V = normalize(viewPos - FragPos);
    vec3 L = normalize(light_position - FragPos);
    vec3 H = normalize(V + L);
    
    // Calculate distance and attenuation
    float distance = length(light_position - FragPos);
    float attenuation = light_intensity / (distance * distance);
    vec3 radiance = light_color * attenuation;
    
    // Calculate F0 (surface reflection at zero incidence)
    vec3 F0 = vec3(0.04); // Default dielectric F0
    F0 = mix(F0, albedo, metallic); // Metals use albedo as F0
    
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
    vec3 kS = F; // Specular contribution
    vec3 kD = vec3(1.0) - kS; // Diffuse contribution
    kD *= 1.0 - metallic; // Metals have no diffuse reflection
    
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;
    
    // Add to outgoing radiance
    float NdotL = max(dot(N, L), 0.0);
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;
    
    // Ambient lighting (simple approximation)
    vec3 ambient = vec3(0.0001) * albedo;
    
    vec3 color = ambient + Lo;
    
    // HDR tonemapping and gamma correction
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    
    FragColor = vec4(color, 1.0);
}