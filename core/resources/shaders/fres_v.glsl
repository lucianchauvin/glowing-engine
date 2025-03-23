#version 330 core

// Input vertex attributes (match with your vertex array bindings)
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// Matrices for standard MVP transform
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Camera position in world space
uniform vec3 viewPos;

// Outputs to the fragment shader
out vec3 fragNormal;
out vec3 fragViewDir;
out vec2 fragTexCoord;

void main()
{
    // Compute world-space position
    vec4 worldPos = model * vec4(inPosition, 1.0);

    // Pass UV straight through
    fragTexCoord = inTexCoord;

    // Transform the normal from vNormal space to world space.
    // In practice you often use the inverse transpose of the vNormal matrix,
    // but if your vNormal matrix is uniform-scale or you’re already computing 
    // a normal matrix, you can use that approach.
    mat3 normalMatrix = mat3(transpose(inverse(model)));
    fragNormal = normalize(normalMatrix * inNormal);

    // Compute the view direction in world space (cameraPos - fragmentPos)
    fragViewDir = viewPos - worldPos.xyz;

    // Output the clip-space position
    gl_Position = projection * view * worldPos;
}
