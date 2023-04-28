#version 330

in vec3 vertices;
in vec2 texCoords;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec2 outTexCoords;

void main(void) {
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertices, 1.0f);
    outTexCoords = texCoords;
}
