
#version 330 core

// Inputs from the vertex shader
in vec3 fragNormal;
in vec3 fragViewDir;
in vec2 fragTexCoord;

// Uniforms (mapping to your Unity properties)
uniform sampler2D uMainTex;     // _MainTex
uniform vec4      uColor;       // _Color

uniform float     uSmoothness;  // _Smoothness (not used here, but included for completeness)
uniform float     uMetallic;    // _Metallic   (not used here, but included for completeness)

uniform vec3      uEmission;    // _Emission
uniform vec3      uFresnelColor;
uniform float     uFresnelExponent; // _FresnelExponent

// Final output color
out vec4 outColor;

void main() {
    // Sample the base texture
    vec3 baseTexColor = texture(uMainTex, fragTexCoord).rgb;

    // Combine with tint color
    vec3 albedo = baseTexColor * uColor.rgb;

    // Normalize normal and view vector
    vec3 N = normalize(fragNormal);
    vec3 V = normalize(fragViewDir);

    // Compute Fresnel: f = (1 - dot(N, V))^_FresnelExponent
    float f = 1.0 - max(dot(N, V), 0.0);
    f = pow(f, uFresnelExponent);

    // Fresnel color
    vec3 fresnelTerm = f * uFresnelColor;

    // Emission (add Fresnel on top of base emission)
    vec3 totalEmission = uEmission + fresnelTerm;

    // Final color. No extra lighting in this minimal example,
    // so we just add the emission to the albedo.
    vec3 finalColor = albedo + totalEmission;

    // Write out final RGBA
    outColor = vec4(finalColor, 1.0);
}
