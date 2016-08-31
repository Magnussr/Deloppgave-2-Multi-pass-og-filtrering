#version 150

uniform sampler2D my_texture;
out vec4 out_color;
smooth in vec2 texCoord;

void main() {
    out_color = texture2D(my_texture, texCoord);
}