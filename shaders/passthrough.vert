#version 150

in vec4 position;
smooth out vec2 texCoord;

void main() {
	gl_Position = position;
	texCoord = (position.xy + 1.0) * 0.5;
}