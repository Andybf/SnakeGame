#version 330

precision highp float;

in vec2 outTexCoords;

uniform sampler2D texture0;

out vec4 frag_Color;

void main() {
    frag_Color = texture(texture0, outTexCoords);
}
