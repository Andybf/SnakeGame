#version 330

in vec3 vertices;
in vec3 texCoords;
in vec3 charInfo;

// For certain WebGL implementations, the max number of uniform elements is 256;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 fragColor;
out vec3 outTexCoords;
out float character;

void calcPerspectiveText(float widthFactor);
 
void main(void) {
    float widthFactor = 0.75f;
    calcPerspectiveText(widthFactor);
    outTexCoords = texCoords;
    character = charInfo.z;
}

void calcPerspectiveText(float widthFactor) {
    vec2 newPosition = vec2(vertices.x + charInfo.x * widthFactor,
                            vertices.y + -charInfo.y);
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(newPosition.x, newPosition.y, 0.0, 1.0);
}

void calcOrthograficText(float widthFactor) {
    vec2 newPosition = vec2( vertices.x + charInfo.x * widthFactor,
                            -vertices.y + charInfo.y);
    gl_Position = projectionMatrix * modelMatrix * vec4(newPosition, 0.0, 1.0);
}
