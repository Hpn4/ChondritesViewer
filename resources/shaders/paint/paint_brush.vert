#version 330 core

layout(location = 0) in vec2 aPos;

uniform vec2 uResolution;
uniform mat4 u_transform;

out vec2 vPos;

void main() {
    vPos = aPos;

    vec2 ndc = (aPos / uResolution) * 2.0 - 1.0;

    gl_Position = u_transform * vec4(aPos, 0.0, 1.0);
}
