#version 330 core

in vec2 out_uv;
in vec3 out_normal;

uniform vec3 diffuse_color;

out vec4 frag_color;

void main () {
	frag_color = vec4 (out_normal, 1.0);
	// frag_color = vec4 (diffuse_color, 1.0) * vec4 (out_uv, 0.5, 1.0);
}