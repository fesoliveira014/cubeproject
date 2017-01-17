#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 lightViewProjection;
uniform mat4 model = mat4(1.0);

void main()
{
    gl_Position = lightViewProjection * model * vec4(position, 1.0);
}  