#version 430

in vec4 vary_color;
out vec4 color;

void main()
{
    color = vary_color;
}
