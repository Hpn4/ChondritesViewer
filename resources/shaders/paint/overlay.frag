#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D u_labelTex;
uniform float u_alpha;

// Exemple pour 12 couleurs
vec3 labelColors[9] = vec3[9](
    vec3(0.341, 0.447, 0.467), // #577277
    vec3(0.275, 0.510, 0.196), // #468232
    vec3(0.776, 0.318, 0.592), // #c65197
    vec3(0.451, 0.745, 0.827), // #73bed3
    vec3(0.662, 0.792, 0.345), // #a8ca58
    vec3(0.906, 0.835, 0.702), // #e7d5b3
    vec3(0.478, 0.212, 0.482), // #7a367b
    vec3(0.647, 0.188, 0.188), // #a53030
    vec3(0.855, 0.525, 0.243)  // #da863e
);

void main() {
    float label = texture(u_labelTex, TexCoord).r * 255;
    if (label < 0.5) discard; // 0 = pas de label

    FragColor = vec4(labelColors[int(label) - 1], 1.0);
}
