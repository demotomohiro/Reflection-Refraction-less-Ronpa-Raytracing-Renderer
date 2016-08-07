#version 430

out vec3 out_color;

uniform vec2 coord_offset;
uniform vec2 resolution;

void main()
{
	vec2 coord = (gl_FragCoord.xy+coord_offset-resolution*0.5)*2.0/resolution.y;
	vec2 d = fract(abs(coord)*2.0);
	vec2 c = pow(vec2(1.0)-abs(d), vec2(16.0));
	out_color = vec3(max(c.x, c.y));
}

