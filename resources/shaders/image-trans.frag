#version 330 core

in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D tex;

void main() {
    vec2 texCoord = vec2(TexCoord.x, 1.0 - TexCoord.y);
    vec3 color = texture(tex, texCoord).rgb;

    if (color == vec3(0.0))
        discard;

    FragColor = vec4(color, 1.0);
}
