#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;

out vec4 vertexColor;

uniform float windowHeight;
uniform float windowWidth;
uniform float gridHeight;
uniform float gridWidth;
uniform float maxZ;
uniform float minZ;

uniform mat4 uMVP;

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
    vertexColor = aColor;
}
