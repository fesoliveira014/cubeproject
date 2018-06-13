#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 lightViewProjection;
uniform mat4 model = mat4(1.0);

out vec4 v_position;

void main()
{
    v_position = lightViewProjection * model * vec4(position, 1.0);
    gl_Position = v_position;
}  