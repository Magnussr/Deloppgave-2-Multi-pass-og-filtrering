#version 150

uniform sampler2D my_texture;
out vec4 out_color;
smooth in vec2 texCoord;

void main() {
	out_color = texture2D(my_texture, texCoord);

	float avg = (out_color.x +  out_color.y  +  out_color.z) /3;

    //out_color = vec4(vec3(max(out_color.r, max(out_color.g, out_color.b))), 1.0);
    out_color = vec4(vec3(avg), 1.0);


    
}