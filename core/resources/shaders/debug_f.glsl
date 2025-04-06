#version 330 core

in vec3 fragColor;
out vec4 FragColor;

// If we also have a uniform "debugColor" for spheres:
uniform vec3 debugColor;

void main()
{
    // If we're drawing lines, "fragColor" is valid. 
    // If we're drawing spheres, we might want to use "debugColor" uniform.
    FragColor = vec4(fragColor, 1.0);
    // Or detect if the color is a default or something. 
    // But simpler approach: for lines, put color in fragColor, 
    // for spheres, store color in debugColor. 
    // Then choose whichever you prefer below.
    // e.g.:
    // FragColor = vec4(debugColor, 1.0);
    // Or choose:
    // if (debugColor.x < 0.0) FragColor = vec4(fragColor, 1.0);
    // else                    FragColor = vec4(debugColor, 1.0);
}
