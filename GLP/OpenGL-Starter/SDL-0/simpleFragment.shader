#version 330 core

out vec4 FragColor;
//in vec3 ourColor;

float Fractal(vec2 c)
{
    vec2 z = vec2(0,0);
    float maxIteration = 20;

    for(int i = 0; i < maxIteration; i++)
    {
        z = vec2(z.x * z.x - z.y * z.y, 2 * z.x * z.y)+c;

        if(z.x * z.x + z.y * z.y > 4)
        {
            return i/maxIteration;
        }
    }
    return 1;
}

void main()
{
    vec2 pos = (gl_FragCoord.xy - 0.5 * vec2(1280,720))/1280;
    float color = Fractal(pos)
    FragColor = vec4(color, 1.0f);
}
