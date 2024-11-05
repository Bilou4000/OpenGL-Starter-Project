#version 330 core

in vec3 pos;
out vec3 ourPos;

uniform float offsetX;
uniform float offsetY;

void main()
{
    ourPos = vec3(pos.x + offsetX, pos.y + offsetY, pos.z);
    gl_Position = vec4(ourPos, 1.0);
}
