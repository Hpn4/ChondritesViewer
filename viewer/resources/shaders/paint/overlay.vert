#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 tex;

out vec2 TexCoord;
uniform mat4 u_transform;

void main() {
    TexCoord = tex;
    gl_Position = u_transform * vec4(pos, 0.0, 1.0);
}
