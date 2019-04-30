#version 330 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 uv;

uniform mat4 projection;
uniform mat4 model;

out vec2 out_uv;

void main () {
	gl_Position	= projection * model * vec4 (pos, 0.0, 1.0);
	out_uv = uv;
}