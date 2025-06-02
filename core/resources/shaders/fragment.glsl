#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

// Light & camera uniforms
uniform vec3  lightPos;
uniform vec3  viewPos;
uniform vec3  lightColor;
uniform vec3  objectColor;

uniform sampler2D diffuse;
uniform sampler2D normal;

void main() {
    // Basic Phong/Blinn-Phong lighting inputs
    vec3 norm       = normalize(Normal);
    vec3 lightDir   = normalize(lightPos - FragPos);
    vec3 viewDir    = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    
    vec3 diffuseTex = texture(diffuse, TexCoord).rgb;

    // Combine them
    vec3 result = diffuseTex;
    FragColor   = vec4(result, 1.0);
    // FragColor   = vec4(norm, 1.0);
}
