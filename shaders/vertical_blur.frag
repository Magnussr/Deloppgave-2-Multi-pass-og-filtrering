#version 150

uniform sampler2D my_texture;
uniform float dy;
out vec4 out_color;
smooth in vec2 texCoord;

void main() {
	const float weights[6] = float[](	0.382925f,	0.24173f,	0.060598f,	0.005977f,	0.000229f,	0.000003f);
	
	vec3 color = weights[0] * texture2D(my_texture, texCoord).rgb;
	for (int i = 1; i <= 5; ++i) {
		color += weights[i] * texture2D(my_texture, vec2(texCoord.x, texCoord.y-i*dy)).rgb;
		color += weights[i] * texture2D(my_texture, vec2(texCoord.x, texCoord.y+i*dy)).rgb;
	}

	out_color = vec4(color, 1.0);
}