#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 uv;
layout (location = 3) in vec4 color;

uniform mat4 projection = mat4(1.0);
uniform mat4 view = mat4(1.0);
uniform mat4 model = mat4(1.0);

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
}