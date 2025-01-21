
#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float t1;
uniform float t2;

void main()
{
    vec4 color1 = texture(texture1, TexCoord) * t1;
    vec4 color2 = texture(texture2, TexCoord) * t2;

    FragColor = color1 + color2;
}