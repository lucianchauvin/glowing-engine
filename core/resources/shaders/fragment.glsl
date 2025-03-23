// #version 330 core
// out vec4 FragColor;

// in vec2 TexCoord;
// in vec3 FragPos;
// in vec3 Normal;

// uniform vec3 lightPos;
// uniform vec3 viewPos;
// uniform vec3 lightColor;
// uniform vec3 objectColor;

// uniform sampler2D texture1;
// uniform sampler2D texture2;

// // struct Material {
// //     sampler2D texture_diffuse1;
// //     sampler2D texture_diffuse2;
// //     sampler2D texture_diffuse3;

// //     sampler2D texture_specular1;
// //     sampler2D texture_specular2;
// //     sampler2D texture_specular3;
// // };CHECK_GL_ERROR();
// // uniform Material material;

// void main() {
//     vec3 norm = normalize(Normal);
//     vec3 lightDir = normalize(lightPos - FragPos);
//     float diff = max(dot(norm, lightDir), 0.0);
//     vec3 viewDir = normalize(viewPos - FragPos);
//     vec3 reflectDir = reflect(-lightDir, norm);
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

//     vec3 ambient = objectColor;
//     vec3 diffuse = diff * lightColor;
//     vec3 specular = spec * lightColor;
//     // vec4 texColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
//     vec3 result = (0.66 * (ambient + diffuse + specular)); //* texColor.rgb;
//     FragColor = vec4(result, 1.0);
// }

#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

// Light & camera uniforms
uniform vec3  lightPos;
uniform vec3  viewPos;
uniform vec3  lightColor;
uniform vec3  objectColor;

// A struct for our material’s texture samplers:
struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_diffuse4;
    sampler2D texture_diffuse5;
    sampler2D texture_diffuse6;
    sampler2D texture_diffuse7;
    sampler2D texture_diffuse8;

    sampler2D texture_specular1;
    sampler2D texture_specular2;
    sampler2D texture_specular3;
};

// Now we can say `uniform Material material;`
uniform Material material;

void main()
{
    // Basic Phong/Blinn-Phong lighting inputs
    vec3 norm      = normalize(Normal);
    vec3 lightDir  = normalize(lightPos - FragPos);
    vec3 viewDir   = normalize(viewPos - FragPos);
    vec3 reflectDir= reflect(-lightDir, norm);
    
    float diff     = max(dot(norm, lightDir), 0.0);
    float spec     = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    // -- SAMPLE the diffuse texture(s) here.
    //    For multiple diffuse textures, you might blend them or pick just the first:
    vec3 diffuseTex = texture(material.texture_diffuse1, TexCoord).rgb;
    // If you want to combine them, for example:
    // vec3 diffuseTex2 = texture(material.texture_diffuse2, TexCoord).rgb;
    // diffuseTex = mix(diffuseTex, diffuseTex2, 0.5);

    // -- SAMPLE the specular texture(s):
    vec3 specularTex = texture(material.texture_specular1, TexCoord).rgb;
    
    // Combine the texture color with your usual lighting terms:
    vec3 ambient  = objectColor * diffuseTex * 0.2;        // example ambient
    vec3 diffuse  = lightColor  * diffuseTex * diff;
    vec3 specular = lightColor  * specularTex * spec;

    // Combine them
    vec3 result = ambient + diffuse + specular;
    FragColor   = vec4(result, 1.0);
}
