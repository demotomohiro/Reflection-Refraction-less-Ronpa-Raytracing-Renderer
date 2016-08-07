#version 430

in vec4 vary_color;
out vec4 out_color;

float glow(float l, float r0, float r1)
{
    return 1.-smoothstep(0., r1, (l-r0)/(1.-r0));
}

float star(float l)
{
    return (glow(l, 0., 0.5)*0.6 + glow(l, 0.025, 0.075));
}

float star_cross(vec2 p)
{
   	const float s = 1./8.;
	vec2 a = pow(abs(p)*0.04, vec2(s));
	float d = (1. - pow(a.x + a.y, 1./2.))*22.;
	d = max(d, 0.);
    return d;
}

void main()
{
//	out_color = vary_color;
//	out_color = vec4(gl_PointCoord, 0.0, 0.0);
	vec2 p = gl_PointCoord * 2.0 - 1.0;
	float rad2 = dot(p, p);
	float centor = star(rad2);
    float sc = star_cross(p);
    float c = (centor + sc)*vary_color.w*0.075;
	out_color = vary_color*vec4(c);
}

