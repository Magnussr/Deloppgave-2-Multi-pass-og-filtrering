#version 150
flat in vec3 color;
smooth in vec3 normal_smooth;
smooth in vec3 v;
smooth in vec3 l;
out vec4 out_color;

void main() {
    vec3 h = normalize(v+l);
    vec3 n = normalize(normal_smooth);
    float diff = max(0.0f, dot(n, l));
    float spec = pow(max(0.0f, dot(n, h)), 128.0f);
	vec3 my_color = diff*color + spec;
	float Y = pow(dot(vec3(0.30, 0.59, 0.11), color), 5);
	//Y = Y * float(Y > 0.25);
    out_color = vec4(my_color, Y);
}