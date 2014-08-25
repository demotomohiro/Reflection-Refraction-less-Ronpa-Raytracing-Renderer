#version 430

out vec4 out_color;

void main()
{
	out_color = vec4(gl_PointCoord, 0.0, 0.0);
}

