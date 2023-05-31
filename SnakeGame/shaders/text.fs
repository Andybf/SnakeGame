#version 330

precision mediump float;

in vec3 outTexCoords;
in float character;
 
uniform sampler2D texture0;
 
out vec4 frag_Color;
 
vec2 processCharacterPosition(float character) {
    float posPixelY = float(int(character/16.0f))*16.0f;
    float xAxisTextureOffset = character - posPixelY;
    posPixelY /= 256.0f;
    float posPixelX = (xAxisTextureOffset*16.0f)/256.0f;
    return vec2(posPixelX, posPixelY);
}
 
void main(void) {
    float widthFactor = 0.75f;
    vec2 charPosition = processCharacterPosition(character);
    vec4 texColor = texture(texture0, vec2(charPosition.x + outTexCoords.x * widthFactor,
                                           charPosition.y + outTexCoords.y));
    if (texColor.r < 0.1) {
        texColor.a = 0.5f;
    }
    frag_Color = texColor;
}
