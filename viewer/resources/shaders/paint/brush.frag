#version 330 core

uniform vec2 center;
uniform float radius;
uniform float label;

out float FragColor;

void main() {
    vec2 uv = gl_FragCoord.xy;
    float d = distance(uv, center);

    if(d < radius)
        FragColor = label / 255.0;
    else
        discard;
}