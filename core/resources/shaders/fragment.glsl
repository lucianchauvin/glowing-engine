#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;    // Light position
uniform vec3 viewPos;     // Camera position
uniform vec3 lightColor;  // Light color
uniform vec3 objectColor; // Base object color

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    // Normalize the normal vector
    vec3 norm = normalize(Normal);

    // Compute light direction
    vec3 lightDir = normalize(lightPos - FragPos);

    // Diffuse shading (Lambertian reflectance)
    float diff = max(dot(norm, lightDir), 0.0);

    // View direction (for specular lighting)
    vec3 viewDir = normalize(viewPos - FragPos);

    // Reflect light direction
    vec3 reflectDir = reflect(-lightDir, norm);
    
    // Specular shading (Phong reflection model)
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    // Combine lighting components
    vec3 ambient = 0.1 * lightColor;
    vec3 diffuse = diff * lightColor;
    vec3 specular = spec * lightColor;

    // Get texture color
    vec4 texColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
    
    // Apply lighting to texture
    vec3 result = (ambient + diffuse + specular) * texColor.rgb;
    FragColor = vec4(result, 1.0);
}
