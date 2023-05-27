#version 430

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 aInstanceMatrix[500];

void main()
{
    TexCoords = aTexCoords;
	gl_Position = projection * view * model* aInstanceMatrix[gl_InstanceID] * vec4(aPos, 1.0);
}
