#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D diffuse;

void main()
{
    FragColor = texture(diffuse, TexCoords);
}