#version 330


in vec2 texCoords;
in vec3 vertices;

//Per instance rendering
in mat4 bodyInstance;
in vec2 spriteLocations;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec2 outTexCoords;
out vec2 outSpriteCoords;

void main(void) {
    gl_Position = projectionMatrix * viewMatrix * bodyInstance * vec4(vertices, 1.0f);
    outTexCoords = texCoords;
    outSpriteCoords = spriteLocations;
}
