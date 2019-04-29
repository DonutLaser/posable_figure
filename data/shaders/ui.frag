#version 330 core

uniform vec3 diffuse_color;

out vec4 frag_color;

void main () {
	float gamma = 2.2;
	frag_color = vec4 (diffuse_color, 1.0);
	frag_color.rgb = pow (frag_color.rgb, vec3 (1.0 / gamma));
}