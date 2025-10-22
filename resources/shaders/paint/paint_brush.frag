#version 330 core

in vec2 vPos;

uniform vec2 uCenter;
uniform float uRadius;
uniform int uLabel;

layout(location = 0) out uvec4 outLabel;

void main() {
    float dist = distance(vPos, uCenter);

    //if (dist <= uRadius) {
    //    outLabel = uvec4(uint(uLabel), 0u, 0u, 0u);
    //} else {
    //    discard;
    //}
    outLabel = uvec4(1.0);
}
