#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform sampler2D u_labelTex;
uniform float u_alpha;

// Génère une couleur distincte par label (simple hash)
vec3 labelToColor(uint label) {
    float hue = fract(sin(float(label) * 12.9898) * 43758.5453);
    float s = 0.8;
    float v = 0.9;
    float c = v * s;
    float x = c * (1.0 - abs(mod(hue * 6.0, 2.0) - 1.0));
    vec3 rgb;
    if (hue < 1.0/6.0) rgb = vec3(c, x, 0);
    else if (hue < 2.0/6.0) rgb = vec3(x, c, 0);
    else if (hue < 3.0/6.0) rgb = vec3(0, c, x);
    else if (hue < 4.0/6.0) rgb = vec3(0, x, c);
    else if (hue < 5.0/6.0) rgb = vec3(x, 0, c);
    else rgb = vec3(c, 0, x);
    float m = v - c;
    return rgb + vec3(m);
}

void main() {
    float label = texture(u_labelTex, vUV).r;
    if (label < 0.5) discard; // 0 = pas de label
    vec3 color = labelToColor(uint(label));
    FragColor = vec4(color, u_alpha);
}
