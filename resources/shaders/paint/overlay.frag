#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D u_labelTex;
uniform float u_alpha;

void main() {
    float label = texture(u_labelTex, TexCoord).r * 255;
    if (label < 0.5) discard; // 0 = pas de label

    FragColor = vec4(label / 255, 0.0, 0.0, 1.0);
}
