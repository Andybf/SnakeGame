#version 330

precision highp float;

in vec2 outTexCoords;
in vec2 outSpriteCoords;

uniform sampler2D texture0;

out vec4 frag_Color;

void main() {
    vec2 newCoord = vec2(outTexCoords.x + outSpriteCoords.x,
                         outTexCoords.y + outSpriteCoords.y);
    
    frag_Color = texture(texture0, newCoord);
}
