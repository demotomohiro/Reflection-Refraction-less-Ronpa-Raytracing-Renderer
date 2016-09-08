#version 430

in vec4 vary_color;
out vec4 out_color;

void main()
{
	vec2 p = gl_PointCoord * 2.0 - 1.0;
	float rad2 = dot(p, p);
    if(rad2 > 1.0)
        discard;

    out_color = vary_color;
}
