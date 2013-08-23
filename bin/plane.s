#include "noise.s"
#line 3 "planes.s"

const float focus = 1.0;
//slider[0.1,1,4]
const float height = 3.0;
//slider[0.0,3.0,4]
const float radius = 10.0;
//slider[3.0,10.0,16.0]
//uniform
const float occul_pwr = 0.2;
//slider[0.1,0.2,0.3]
//uniform
const float occul_xscl = 0.95;
//slider[0.6,0.95,1]
//uniform float param0; slider[-1,0,1]
const float param0 = 0.0;
//uniform float param1; slider[-1,0,1]
const float param1 = 0.0;
//uniform float param2; slider[-1,0,1]
//uniform float param3; slider[-1,0,1]
//uniform float param4; slider[-1,0,1]
//uniform float param5; slider[-1,0,1]
const float box_w = 3.0;
//slider[0.01,3,6]
const float box_h = 3.0;
//slider[0.01,3,6]
const float box_d = 6.0;
//slider[0.02,6,12]
const vec3 box_color = vec3(0.6,0.4,0.3);
//color[0.6,0.4,0.3]
const vec2 box_light_sz = vec2(0.3,0.3);
//slider[(0.1,0.1),(0.3,0.3),(0.5,0.5)]
const float box_light_scale = 10.0;
const float box_indrct_light_scale = 1.0;
//slider[0.4,1.0,2.0]
const float box_indrct_light_power = 2.0;
//slider[1.0,2.0,4.0]

const float specular_scale = 5.0;

const float PI = 3.14159265;

float tex(vec2 p)
{
	ivec2 ip = ivec2(floor(p));

	return float((ip.x^ip.y)&1)/2.0;
}

vec3 texColor(vec2 p)
{
	return vec3(fract(p), fract(length(p/10)));
}

float distance(vec2 p, float r)
{
	vec2 ap = abs(p);
	ap = pow(ap, vec2(r));
	return pow(ap.x+ap.y, 1.0/r);
}

float occul_hole(vec2 uv)
{
	float x = 1.0 - abs((fract(uv.x)-0.5)*2.0)*occul_xscl;
//	return vec3((1.0 - x*x)/(uv.y+1.0));
	return pow(x, occul_pwr)/(uv.y*0.5+1.0);
}

float occul_wall(vec2 uv)
{
	float r = 1.0 - length(uv)/sqrt(2.0)*occul_xscl;

	return pow(r, occul_pwr);
}

float spec_edge(vec2 uv)
{
	vec2 spec_uv = vec2(uv.x*(0.98), 1.0 - uv.y);
	float d = distance(spec_uv, 24.0);

	float a = 1.0 - abs(d - 0.95);
	a = max(a, 0.0);
	float specular_edge = pow(a, 82.8)-pow(abs(uv.x), 16.0)-0.1;

	return max(specular_edge, 0.0);
}

vec3 tube_shade(vec3 v)
{
	vec2 uv = v.xy;

	if(v.z > 0.0)
	{
		return vec3(occul_hole(uv));
	}else
	{
		vec2 fuv = fract(uv);
		fuv.x = abs(fuv.x-0.5)*2.0;
		float specular =
		spec_edge(fuv)
		+
		spec_edge(vec2(fuv.x*2.4+1.1, fuv.y-0.1))
		+
		spec_edge(vec2(fuv.x*2.4-1.1, fuv.y-0.1))
		;
		for(int i=0; i<2; ++i)
		for(int j=0; j<2; ++j)
		{
			float y0 = float(i*2-1);
			float y1 = float(j*2-1);
			specular += spec_edge(vec2((fuv.x-0.46*y0-0.2*y1)*2.6*2.6, fuv.y-0.2));
		}

		vec2 shade_uv = vec2(fuv.x*0.5, (fuv.y-0.5)*2.0);
		float shade_d = distance(shade_uv, 2.8);
		shade_d = max(shade_d*shade_uv.y, 0.0);

		float shade_grad = pow((1.0-shade_d) * 1.23, 0.6);
		float shade = clamp(shade_grad, 0.0, 1.0);

	//	return vec3(shade_d);
		return vec3(specular*specular_scale + vec3(0.2)) * shade;
	}
}

float plane_shadow(vec2 uv)
{
	vec2 uv0 = vec2(uv.x+box_w, uv.y+(uv.x+box_w)-6);
	if(uv0.x < 0.0 && uv0.x > -box_h && uv0.y > 0.0 && uv0.y < box_d)
	{
		return 1.0;
	}else
	{
		return 0.0;
	}
}

vec3 plane_shade(vec3 v)
{
	float occul = occul_hole(v.xy);
	if(v.z < 1.0)
	{
		//x
		return vec3(occul);
	}else if(v.z < 2.0)
	{
		//y
		return vec3(0.2, 0.8, 0.2);
	}else if(v.z < 3.0)
	{
		//z
		return vec3(occul);
	}else
	{
		return vec3(1.0, 0.0, 0.0);
	}
}

vec3 plane_cubes(vec2 scr_p, vec2 uv, float height)
{
	if(tex(uv) > 0.2)
	{
		return vec3(uv, 1.0);
	}else
	{
		vec2 rect_o = trunc(abs(uv));
		float rate2 = (rect_o.y+1.0)/focus;
		vec2 uv_z = scr_p*rate2;
		if(abs(uv_z.x) > rect_o.x+1.0)
		{
			float rate3 = (rect_o.x+1.0)/abs(scr_p.x);
			vec2 uv_x = vec2(focus, scr_p.y)*rate3;
			uv_x.y -= height;
			return vec3(uv_x, 0.0);
		}else
		{
			uv_z.y -= height;
			return vec3(uv_z, 2.0);
		}
	}
}

vec2 plane(vec2 p, float height)
{
	float rate_y = height/p.y;
	vec2 uv_y = vec2(p.x, focus)*rate_y;

	return uv_y;
}

vec2 polar(vec2 p)
{
	return vec2(atan(p.y, p.x)/PI*0.5, length(p));
}

vec2 tube(vec2 p, float radius)
{
	return vec2(p.x, radius/p.y*focus);
}

vec3 tube_cubes(vec2 scr_p, vec2 uv, float height)
{
	if(tex(uv) > 0.2)
	{
		return vec3(uv, 0.0);
	}else
	{
		vec2 rect_o = trunc(abs(uv));
		float rate2 = (rect_o.y+1.0)/focus;
		vec2 uv_z = vec2(scr_p.x, scr_p.y*rate2);
	/*	if(abs(uv_z.x) > rect_o.x+1.0)
		{
			float rate3 = (rect_o.x+1.0)/abs(scr_p.x);
			vec2 uv_x = vec2(focus, scr_p.y)*rate3;
			uv_x.y -= height;
			return uv_x*2.0;
		}else
	*/
		{
			uv_z.y -= height;
			return vec3(uv_z, 1.0);
		}
	}
}

vec3 background(vec2 c)
{
	float y = 1.0-c.y*0.65;
	return vec3(vec2(y*y), 1.0);
}

vec3 tube_plane_backgroud(vec2 c)
{
	vec2 pplr = polar(c);
	vec2 pp = vec2(c.x, abs(c.y));
	vec2 plane_uv = plane(pp, height);
	if
	(
		(fract(abs(pplr.x)*8.0)  > 0.25)
		&&
		!(c.y < 0.0 && abs(plane_uv.x) < radius)
	)
	{
		//tube
		pplr.x *= 32.0;
	//	return vec3(texColor(tube_cubes(pplr, tube(pplr, radius), radius).xy));
		return vec3(tube_shade(tube_cubes(pplr, tube(pplr, radius), radius)));
	}else if(c.y < 0.0)
	{
		//plane
		float shadow = plane_shadow(plane_uv) * 0.5;
		return vec3(plane_shade(plane_cubes(pp, plane_uv, height))) - vec3(shadow);
	} else
	{
		return background(c);
	}
}

vec3 box_xzwall_lit(vec2 c)
{
	const float a = 0.6;
	vec2 uv0 = vec2(c.x, (-c.y+a)/(1.0+a));
	float y = max(uv0.y*0.5, -uv0.y*(3.0));
	vec2 light_uv = vec2(uv0.x, y);
	float b = 0.8;
	float r = length(light_uv/vec2(max(uv0.y, 0.0)*b+1.0, 1.0));
	float lit = (0.2)/(1.0 + (2.5)*r*r);

	//	float lit = pow(r*box_indrct_light_scale, box_indrct_light_power);
	//	return vec3(lit);
	//	return vec3(fract(light_uv), 0.0);
	//	return vec3(uv_x, 0.0);

	return vec3(lit);
}

vec3 box(vec2 c, vec2 box_scr)
{
	vec2 box_uv_offset = vec2(box_w, box_h)/vec2(box_scr)*focus;
	vec2 ac = vec2(abs(c.x), c.y);
	vec2 uv_x = plane(ac.yx, box_w);
	uv_x.y -= box_uv_offset.x;
	uv_x /= vec2(box_h, box_d);
	vec2 ca = vec2(c.x, abs(c.y));
	vec2 uv_y = plane(ca, box_h);
	uv_y.y -= box_uv_offset.y;
	uv_y /= vec2(box_w, box_d);
	if(uv_x.y > 1.0 && uv_y.y > 1.0)
	{
		//z wall
		vec2 uv_z = vec2(c/box_scr*(box_d+box_uv_offset.y)/box_uv_offset.y);
		vec3 lit = box_xzwall_lit(uv_z);
		float o = 0.2;
		vec2 occul_uv = vec2(uv_z.x, uv_z.y*(1.0-o)+o);
		return vec3(box_color*occul_wall(occul_uv) + lit);
	}else if(abs(uv_y.x) < 1.0)
	{
		//y wall
		vec2 light_uv = vec2(uv_y.x, uv_y.y*2.0-1.0);
		float lit = 0.0;
	//	return vec3(abs(light_uv), 0.0);
		if(c.y > 0.0)
		{
			
			if(all(lessThan(abs(light_uv), box_light_sz)))
			{
				return vec3(box_light_scale);
			}else
			{
				float r = 1.0 - length(light_uv/box_light_sz.yx)/length(vec2(1.0)/box_light_sz);
				lit = pow(r*box_indrct_light_scale, box_indrct_light_power);
			}
			return vec3(box_color*occul_wall(uv_y) + lit);//texColor(uv_y);
		}else
		{
			float r = length(light_uv)/sqrt(2.0);
			float shadow = sin(min(r*1.5, 1.0)*PI);
			lit = pow((1.0 - r), 3.0)*0.5 - pow(shadow, 2.0)*0.5;
			return vec3(box_color + lit);//texColor(uv_y);
		}
	}else
	{
		//x wall
		vec2 uv0 = vec2(uv_x.y*2.0-1.0, uv_x.x);
		vec3 lit = box_xzwall_lit(uv0);
		float o = 0.2;
		vec2 occul_uv = vec2(uv_x.x*(1.0-o)+o, uv_x.y);
		return vec3(box_color*occul_wall(occul_uv) + lit);//texColor(uv_x);
	}

	return vec3(1.0, 0.0, 0.0);
}

vec3 ball(vec2 c, float rad_scr)
{
	float r = length(c)/rad_scr;
	float blk = pow(r, param0*4.0+8.0) * (param1*0.5+0.15) * min(pow(abs(c.x/rad_scr), 2.0)*2.0, 1.0);
	vec2 lit_uv = polar(c);
	lit_uv.y /= rad_scr;
	float lit = abs(lit_uv.x - 0.25) < 0.035 && abs(lit_uv.y - 0.9) < 0.04 ? 1.0 : 0.0;
	float r2 = length((lit_uv-vec2(0.25, 0.9))/vec2(0.15, 0.25));
//	return vec3(fract(max(lit_uv-vec2(0.25, 0.9), vec2(-0.25, -0.1))/vec2(0.25, 0.1)), 0.0);
	float lit2 = pow(max(1.0 - r2, 0.0), 2.0)*0.20;
	return vec3(box_color)*1.2 + lit + lit2 - blk;
}

vec3 main_pic(vec2 c)
{
//	return vec3(c.x, c.y, 0.0);
	const float rad_scr = 0.23;
	if(length(c) < rad_scr)
	{
		return ball(c, rad_scr);
	}else
	{
		vec2 ac = abs(c);
		float box_scr = 0.5;
		if(max(ac.x, ac.y) < box_scr)
		{
			return box(c, vec2(box_scr));
		}else
		{
			return tube_plane_backgroud(c);
		}
	}
}

float perlin2D(vec3 pos)
{
	float c = 0.0;
	float s = 4.0;
	for(int i=0; i<6; ++i)
	{
		c += softnoise(vec3(pos)+vec3(1.0/float(i+2)), s)/s*2.0;
		s *= 2.0;
	}

	return c;
}

vec3 master_pic(vec2 c)
{
	vec2 pplr = polar(vec2(c)*0.5-vec2(1.1, 0.6));
	pplr.x *= 8.0;
	if(perlin2D(vec3(pplr, 0.2))>pplr.y*0.3+0.3)
	{
		return vec3(1.0, 0.0, 0.0);
	}

	vec2 leftdown = c*0.5 + vec2(1.1, 0.6);
	float dense = 16.0;
	vec2 flrld = floor(leftdown*dense)/dense;
	if(rand(vec3(flrld, 0.22))>length(flrld)*0.8+0.2)
	{
		vec2 frcld = (leftdown - flrld)*dense*2.0-1.0;
		float d = max(abs(frcld.x), abs(frcld.y));
		dense = 64.0;
		flrld = floor(leftdown*dense)/dense;
		ivec2 ild = ivec2(floor(leftdown*dense));
		if(rand(vec3(flrld, 0.21))>/*length(flrld)*/0.1 || d < 0.7)
		{
		//	if(((ild.x^ild.y)&1) == 0 || d < 0.7)
				return vec3(d);
		}
	}


	return main_pic(c);
}

const vec2 frm = vec2(16.0/10.0, 1.0);

vec3 real_master_pic(vec2 c)
{
//	return master_pic(c);
	vec2 uv = c*vec2(1.2);
	if(all(lessThan(abs(uv), frm)))
		return master_pic(uv);

	vec2 auv = abs(uv)-vec2(0.1);
	vec2 ouv = abs(auv - frm);
	return vec3(cos(min(ouv.x, ouv.y)*9.0))*vec3(0.9, 0.8, 0.2);
	//if(abs(max(auv.x, auv.y) - frm) )
}

vec3 stars(vec2 c)
{
	float lit = 0;
	vec2 leftdown = c*0.5 + vec2(1.1, 0.6);
	float dense = 8.0;
	for(int i=0; i<6; ++i)
	{
		vec2 dense_uv = leftdown*dense;
		vec2 lcl_uv = fract(dense_uv)*2.0-1.0;
		vec2 flrld = floor(dense_uv)/dense;//vec2(log(dense)*0.0001);
		if(rand(vec3(flrld, 0.32))>0.5)
		{
			vec2 offset = vec2(2.1, 1.1) + vec2((sin(dense)))*0.005;
			vec2 uv = (flrld*2.0-offset)*vec2(1.2);
			vec2 auv = abs(uv)-vec2(0.1);
			vec2 ouv = abs(auv - frm);
			if(min(ouv.x, ouv.y) < 0.15)
				lit += max((1.0/(1.0+distance(lcl_uv, 0.5))-0.5)*2.0, 0.0);
		}
		dense *= 1.2;
	}

	return lit*vec3(0.8);
}

vec3 color(vec2 c)
{
	return real_master_pic(c) + stars(c);
}
