#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec4 color;

uniform mat4 projection = mat4(1.0);
uniform mat4 view = mat4(1.0);
uniform mat4 model = mat4(1.0);
uniform mat4 lightViewProjection = mat4(1.0);

out DATA
{
	vec3 position;
	vec3 normal;
	vec4 color;
	vec4 lightSpacePosition;
} vs_out;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);
	vs_out.position = vec3(model * vec4(position, 1.0));
	vs_out.normal = mat3(transpose(inverse(model))) * normal;
	vs_out.color = color;
	vs_out.lightSpacePosition = lightViewProjection * model * vec4(position, 1.0);
}