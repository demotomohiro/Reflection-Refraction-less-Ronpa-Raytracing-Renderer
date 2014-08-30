#version 430

in vec4 vary_color;
out vec4 out_color;

void main()
{
//	out_color = vary_color;
	out_color = vec4(gl_PointCoord, 0.0, 0.0);
}

