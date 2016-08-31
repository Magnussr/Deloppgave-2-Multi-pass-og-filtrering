#version 150

uniform sampler2D my_texture;
out vec4 out_color;
smooth in vec2 texCoord;

void main() {
	out_color = texture2D(my_texture, texCoord);

	//Find the average out_color from all the colors and sets the avg value it to all the color channels to get a grey color value
	float avg = (out_color.x +  out_color.y  +  out_color.z) /3;

    out_color = vec4(vec3(avg), 1.0);    
}