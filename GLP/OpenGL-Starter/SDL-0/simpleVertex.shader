#version 330 core

in vec3 pos;
in vec3 color;

out vec3 ourColor;
out vec3 ourPos;

uniform float offset;

void main()
{
    ourColor = color;
    ourPos = vec3(pos.x * (offset * 2.0f) + offset, -pos.y * (offset * 2.0f), pos.z);
    gl_Position = vec4(ourPos, 1.0);

}
