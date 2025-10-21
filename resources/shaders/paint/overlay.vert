#version 330 core

layout(location = 0) in vec2 aPos;
uniform mat4 u_transform;
out vec2 vUV;

void main() {
    vUV = (aPos + 1.0) * 0.5; // NDC → [0,1]
    gl_Position = u_transform * vec4(aPos, 0.0, 1.0);
}
