#ifndef NOISE_S
#define NOISE_S

float rand(vec3 pos)
{
  vec3 p = pos + vec3(2.);
  vec3 fp = fract(p*p.yzx*222.)+vec3(2.);
  p.y *= p.z * fp.x;
  p.x *= p.y * fp.y;
  return
    fract
    (
		p.x*p.x
    );
}

float softnoise(vec3 pos, float scale)
{
  vec3 smplpos = pos*scale;
  vec3 nsmplpos = floor(smplpos);
  float c000 = rand((nsmplpos+vec3(.0,.0,.0))/scale);
  float c100 = rand((nsmplpos+vec3(1.,.0,.0))/scale);
  float c010 = rand((nsmplpos+vec3(.0,1.,.0))/scale);
  float c110 = rand((nsmplpos+vec3(1.,1.,.0))/scale);
  float c001 = rand((nsmplpos+vec3(.0,.0,1.))/scale);
  float c101 = rand((nsmplpos+vec3(1.,.0,1.))/scale);
  float c011 = rand((nsmplpos+vec3(.0,1.,1.))/scale);
  float c111 = rand((nsmplpos+vec3(1.,1.,1.))/scale);

  vec3 a = smoothstep(0.0, 1.0, fract(smplpos));
  return
    mix(
      mix(
        mix(c000, c100, a.x),
        mix(c010, c110, a.x),
        a.y
      ),
      mix(
        mix(c001, c101, a.x),
        mix(c011, c111, a.x),
        a.y
      ),
      a.z
    );
}

#endif
