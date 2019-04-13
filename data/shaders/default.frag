#version 330 core

in vec2 out_uv;
in vec3 out_normal;

uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec3 specular_color;

uniform vec3 light_direction;
uniform vec3 view_direction;

out vec4 frag_color;

void main () {
	// Calculate ambient color
	vec3 ambient = vec3 (0.1) * ambient_color;

	// Calculate diffuse color
	vec3 norm = normalize (out_normal);
	vec3 light_dir = normalize (-light_direction);
	float diff = max (dot (norm, light_dir), 0.0);
	vec3 diffuse = diff * diffuse_color;

	// Calculate specular color
	vec3 view_dir = normalize (view_direction);
	vec3 halfway_dir = normalize (light_direction + view_dir);
	float spec = pow (max (dot (norm, halfway_dir), 0.0), 16.0);
	vec3 specular = 0.5 * spec * specular_color; 

	vec3 color = ambient + diffuse + specular;
	frag_color = vec4 (color, 1.0);
}