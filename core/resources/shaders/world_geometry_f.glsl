#version 330 core
in vec3 Normal;
in vec3 Color; // Incoming color

out vec4 FragColor;

uniform vec3 lightDir = normalize(vec3(1.0, -1.0, -1.0)); // Directional light
uniform vec3 lightColor = vec3(1.0); // White light

void main()
{
    float diff = max(dot(normalize(Normal), -lightDir), 0.0);
    vec3 lighting = diff * lightColor; // Basic diffuse lighting
    vec3 finalColor = Color * lighting; // Apply lighting to color

    // FragColor = vec4(finalColor, 1.0);
    FragColor = vec4(finalColor, 1.0);
}
