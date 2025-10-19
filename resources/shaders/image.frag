#version 330 core

in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D tex;

void main() {
    vec2 texCoord = vec2(TexCoord.x, 1.0 - TexCoord.y);
    FragColor = texture(tex, texCoord);
}
