#version 330 core

out vec4 FragColor;
uniform float time;
uniform vec3 colorShift;

float Fractal(vec2 p)
{
    vec2 c = vec2(-0.12,0.2) + p/(pow(200,time));
    vec2 z = c;
    float maxIteration = 50;

    for(int i = 0; i < maxIteration; i++)
    {
         z = vec2(z.x * z.x - z.y * z.y, 2 * z.x * z.y) + vec2(0, 0.8);

        if(z.x * z.x + z.y * z.y > 4)
        {
            return i/maxIteration;
        }
    }
    return 1;
}

void main()
{
    vec2 pos = 4 * (gl_FragCoord.xy - 0.5 * vec2(1280,720))/1280;
    float color = Fractal(pos);
    FragColor = vec4(color * colorShift.x, color * colorShift.y, color * colorShift.z, 1.0f);
}
