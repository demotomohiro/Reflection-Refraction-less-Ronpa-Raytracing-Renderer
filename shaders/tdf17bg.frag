#version 430

out vec3 out_color;

uniform vec2 coord_offset;
uniform vec2 resolution;

uniform float aspect_rate;

#define ZNEAR	0.001
#define ZNEAR_W 0.001
#define ZNEAR_H	(ZNEAR_W*resolution.y/resolution.x)

void main()
{
	vec2 pos0 = ((gl_FragCoord.xy+coord_offset)*2.0 - resolution)/resolution.y;
	vec2 pos1 = (gl_FragCoord.xy+coord_offset)/resolution.xy;

    float theta = TIME*3.141*0.25;
	mat2 m = mat2(cos(theta), sin(theta), -sin(theta), cos(theta));
    vec3 viewdir = normalize(vec3((pos1*2.0 - vec2(1.0)) * vec2(ZNEAR_W, ZNEAR_H), -ZNEAR));
    viewdir.xz = m*viewdir.xz;
    float phi = atan(viewdir.x, -viewdir.z);

    const float width = 4.0;
    const float height = 0.25;
    vec2 uv;
    vec2 nuv;
    out_color = vec3(1.0);
    if(abs(viewdir.y)/height > abs(viewdir.x) && abs(viewdir.y)/height > abs(viewdir.z)) {
        float a = height/viewdir.y;
        nuv = uv = viewdir.xz * a;
        vec2 uv2 = fract(uv*4.0)*2.0 - vec2(1.0);
        out_color = vec3(abs(sin((abs(uv2.x) + abs(uv2.y + sin(uv2.x*4.0)*0.2))*16.0)));
    }else{
        if(abs(viewdir.x) > abs(viewdir.z)) {
            float a = width/viewdir.x;
            uv = viewdir.zy * a;
        }else{
            float a = width/viewdir.z;
            uv = viewdir.xy * a;
        }
        nuv = vec2(uv.x/width, uv.y);
#if 0
        float theta2 = atan(viewdir.y, length(viewdir.xz));
        float r = distance(vec2(phi, theta2), vec2(0.3, 0.4));
		out_color = vec3(0.3, 0.3, 0.7) + vec3(0.01/r/r);
#endif
    }

    //out_color = vec3(uv, 0.0);

    float d = max(length(nuv) - 0.9, 0.0)/(sqrt(2.0)-0.9);
    out_color = (out_color*0.25 + 0.25)*vec3(1.0 - d*d*0.5);
}
