#version 330 core

out vec4 FragColor;

uniform vec3 shift;


void main()
{
    FragColor = vec4(shift,1.0f);
}