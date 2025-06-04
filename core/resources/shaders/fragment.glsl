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

/*        vec3 normalSample = texture(normal, TexCoord).rgb;
        FragColor = vec4(normalSample, 1.0); // Should show normal map visually
        return ;
  */
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

