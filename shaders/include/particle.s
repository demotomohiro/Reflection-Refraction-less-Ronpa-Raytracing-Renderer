#pragma once

uniform vec2 viewport_scale;
uniform vec2 viewport_size;
uniform vec2 viewport_offset;
uniform int	 vertexID_offset;

//pvm = projection_matrix * view_matrix * model_matrix.
void brOutputPosition(vec3 position, mat4 pvm)
{
    gl_Position = pvm * vec4(position, 1.);
    gl_Position.xy = gl_Position.xy * viewport_scale + viewport_offset*gl_Position.w;
}

void brOutputPointSize(float dim, mat4 pvm)
{
	float ps = pvm[0][0] * dim * viewport_scale.x;
    //gl_PointSize is side length of rasterized point in pixel.
	gl_PointSize = viewport_size.x*0.5*ps / gl_Position.w;
}

int brGetVertexID()
{
    return gl_VertexID + vertexID_offset;
}
