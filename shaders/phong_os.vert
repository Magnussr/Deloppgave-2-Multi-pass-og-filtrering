#version 150
uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
uniform mat4 modelview_inverse_matrix;

in  vec3 position;
in  vec3 normal;

flat out vec3 color;
smooth out vec3 v;
smooth out vec3 l;
smooth out vec3 normal_smooth;

void main() {
	vec4 pos = modelview_matrix * vec4(position, 1.0);

	float homogeneous_divide = (1.0f/modelview_inverse_matrix[3].w);
	vec3 light_pos_world = vec3(modelview_inverse_matrix * vec4(200.0f, 200.0f, 200.0f, 1.0))*homogeneous_divide;
	vec3 cam_pos_world = modelview_inverse_matrix[3].xyz*homogeneous_divide;

	v = normalize(cam_pos_world - position);
	l = normalize(light_pos_world - position);

	gl_Position = projection_matrix * pos;
	color = vec3(0.5f, 0.7f, 0.5f);
	normal_smooth = normal;
}