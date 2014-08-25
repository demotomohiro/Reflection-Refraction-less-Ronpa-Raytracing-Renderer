#version 430

void main()
{
	if(gl_VertexID==0)
		gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
	else if(gl_VertexID==1)
		gl_Position = vec4(1.0, 0.0, 0.0, 1.0);
	else
	{
		float x = gl_VertexID / 10.0;
		gl_Position = vec4(x, x, 0.0, 1.0);
	}

	gl_PointSize = 100.0;
}
