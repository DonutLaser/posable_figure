#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 out_uv;
out vec3 out_normal;

void main () {
	gl_Position = projection * view * model * vec4 (pos, 1.0);
	out_uv = uv;
	out_normal = mat3 (transpose(inverse(model))) * normal;
}