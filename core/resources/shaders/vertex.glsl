#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNor; // New: Normal attribute

out vec2 TexCoord;
out vec3 FragPos;  // New: Position in world space
out vec3 Normal;   // New: Normal in world space

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));  // Transform to world space
    Normal = mat3(transpose(inverse(model))) * aNor;  // Correct normal transform
    // Normal = aNor;  // Correct normal transform

    gl_Position = projection * view * vec4(FragPos, 1.0);
    TexCoord = aTexCoord;
}
