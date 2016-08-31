#version 150

uniform sampler2D my_texture;
out vec4 out_color;
smooth in vec2 texCoord;

void main() {
	//Invert the color, just to illustrate that we simply render a texture
	vec3 color = vec3(1.0) - texture2D(my_texture, texCoord).rgb;
	out_color = texture2D(my_texture, texCoord);
    //out_color = vec4(color, 1.0);
}