#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 CustomColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float t1;
uniform float t2;

void main() {
    vec4 texColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
    FragColor = texColor * vec4(CustomColor, 1.0);
}