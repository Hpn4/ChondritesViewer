#version 330 core

layout(location = 0) in vec2 aPos;

uniform vec2 uResolution;

out vec2 vPos;

void main() {
    vPos = aPos;

    vec2 ndc = (aPos / uResolution) * 2.0 - 1.0;

    gl_Position = vec4(ndc.xy, 0.0, 1.0);
}
