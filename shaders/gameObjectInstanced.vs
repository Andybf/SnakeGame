#version 330

in vec3 vertices;
in vec2 texCoords;

//Per instance rendering
in vec2 bodySegmentPosition;
in vec2 bodySegmentTexCoords;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec2 outTexCoords;
out vec2 outSpriteCoords;

void main(void) {
    vec3 newPosition = vec3(vertices.x + bodySegmentPosition.x,
                            vertices.y + bodySegmentPosition.y,
                            vertices.z);
    
    
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(newPosition, 1.0f);
    outTexCoords = texCoords;
    outSpriteCoords = bodySegmentTexCoords;
}
