#version 330 core

in vec2 out_uv;

uniform vec3 diffuse_color;
uniform sampler2D main_texture;

out vec4 frag_color;

void main () {
	float gamma = 2.2;
	vec4 tex_color = texture (main_texture, out_uv);
	frag_color = vec4 (diffuse_color, 1.0) * tex_color;
	frag_color.rgb = pow (frag_color.rgb, vec3 (1.0 / gamma));
}