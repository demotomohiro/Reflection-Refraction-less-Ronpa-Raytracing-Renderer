import argparse
import subprocess
import os
import os.path
import inspect
from GLSLJinja import GLSLJinjaLoader

parser = argparse.ArgumentParser(description="Generate multiple images using batchRIshader with particle_DF_with_motion.vert")
parser.add_argument("exe", help="path to the executable file 'batchRIshader'")
args = parser.parse_args()

outputDir   = os.path.abspath(os.getcwd());
outputName  = os.path.splitext(os.path.basename(__file__))[0]
srcDir      = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
print("srcDir: " + srcDir, flush=True)
batchRIshader = os.path.abspath(args.exe)
shaderDir = os.path.join(srcDir, "shaders")
os.chdir(shaderDir)

loader = GLSLJinjaLoader(os.path.join(srcDir, "anim_script"))
particleTmpl = loader.get_template("particle_DF_with_motion.vert.tmpl")

def render_frame(source, i):
    p = subprocess.Popen(
        [
            batchRIshader,
            "--num_particles",          str(65536 << (13-5)),
            "--num_div_particles",      str(64),
            "--super_sampling_level",   str(0),
            "--num_tile_x",             str(1),
            "--num_tile_y",             str(1),
            "--output",                 os.path.join(outputDir, outputName + "_frm%05d.png" % i),
            "--output_w",               str(1920),
            "--output_h",               str(1080),
            "--hide_gl_info",
            "main_shader.frag",
            "-",
            "particle_DF.frag"
        ],
        universal_newlines = True,
        stdin = subprocess.PIPE
        )
    p.communicate(input = source)
    if p.returncode > 0:
    #    print(source[1000:])
        raise Exception()

def get_line_directive():
    return '#line %d "%s"\n' % (inspect.stack()[1][2], inspect.stack()[1][1].replace("\\", "\\\\"))

def render_template(include, timeinfo, cross_roundness = 16.0):
    tmpl = loader.get_includable_template_from_string(include)

    d = dict(
        t01                 = timeinfo.get_t01(),
        gbl_time            = timeinfo.get_global_time(),
        tmpl                = tmpl,
        cross_roundness     = cross_roundness,
        duration_scaling    = timeinfo.duration_scaling)
    return particleTmpl.render(d)

class TimeInfo:
    def __init__(self, FPS, local_frame, global_frame, duration, duration_scaling):
        self.FPS            = FPS
        self.local_frame    = local_frame
        self.global_frame   = global_frame
        self.duration       = duration
        self.duration_scaling   = duration_scaling

    def get_t01(self):
        return self.local_frame/(self.FPS*self.duration)

    def get_global_time(self):
        return self.global_frame/self.FPS

def get_scene_still(cross_roundness = None):
    def scene_still(timeinfo):
        code = (
            get_line_directive() +
            """\
            """
        )
        return render_template(code, timeinfo, cross_roundness) if cross_roundness != None else render_template(code, timeinfo)
    return scene_still

def scene_x_slide(timeinfo):
    code = (
        get_line_directive() +
        """\
        float nrmz = (star_pos.z - DFmin.z) / DFsize.z;
        float t = max(0.08 + nrmz*1.85 - `t01`*2.0, 0.0);
        float x0 = (star_pos.x+0.25);
        star_pos.x += pow(t*8., x0*8.0+0.25)*3.0 + t*x0*8.0;
        """
    )
    return render_template(code, timeinfo, 2.0)

def scene_suck(timeinfo):
    code = (
        get_line_directive() +
        """\
        vec3 center = vec3(0.0);
        float nrmdist = length(star_pos - center) / length(DFmin - center);
        float t = clamp(1.0 + nrmdist - `t01`*2.0, 0.0, 1.0);
        star_pos = (star_pos - center)*pow(t, 4.0) + center;
        color_scaling = max(pow((t - 0.2)/(1.0-0.2), 8.0), 0.0);
        """
    )
    return render_template(code, timeinfo, 2.0)

def scene_box_stack(timeinfo):
    code = (
        get_line_directive() +
        """\
            uvec3 blk = uvec3(abs((star_pos - vec3(0.0, ground, 0.0))* 32.0));
            vec3 timing = uintToFloat(Philox4x32(uvec4(blk.xz, 19, 11), uvec2(32743, 410275))).xyz;
        //    float t = max(`t01`*2.0 - timing.y*0.5 - float(blk.y)/16.0, 0.0);
            float t = max(0.3 + timing.y + float(blk.y)/12.0 - `t01`*2.0, 0.0);
            star_pos.y += t*t*8.0;
        """
    )
    return render_template(code, timeinfo)

def scene_plates(timeinfo):
    code = (
        get_line_directive() +
        """\
        float t0 = `t01`*4.0;
        float t1 = clamp(`t01`*4.0 - 1.0, 0.0, 1.0);
        float t2 = clamp(`t01`*4.0 - 2.0, 0.0, 1.0);
        float t3 = clamp(`t01`*4.0 - 3.0, 0.0, 1.0);
        vec3 in_star_pos = star_pos;
        bool axis = abs(star_pos.x) > abs(star_pos.z);
        vec3 rnorm = axis ? vec3(sign(star_pos.x), 0.0, 0.0) : vec3(0.0, 0.0, sign(star_pos.z));
        float dist = abs(axis ? star_pos.x : star_pos.z);
        const float dmin = 0.04, dmax = 0.125;
        float d = (abs(dist) - dmin)/(dmax - dmin);
        const float fall_duration = 0.25;
        float t = clamp(t0*(1.0+fall_duration) - (1.0 - d), 0.0, fall_duration);
        float theta = pow(t/fall_duration, 2.0)*3.1416*0.5;
        float h = star_pos.y - DFmin.y;
        vec3 v = rnorm*h*sin(theta) + vec3(0.0, 1.0, 0.0)*h*cos(theta);
        star_pos = v + vec3(star_pos.x, DFmin.y, star_pos.z);

        float depth = dmax - dist;
        //star_pos.y += depth*t1;
        star_pos += vec3(rnorm.x, 1.0, rnorm.z)*depth*t1;

        vec3 center = in_star_pos + rnorm*depth;
        center.y = DFmin.y;
        float theta3 = t3*3.1416*0.5;
        vec3 xaxis = rnorm*cos(theta3) + vec3(0.0, 1.0, 0.0)*sin(theta3);
        vec3 yaxis = -rnorm*sin(theta3) + vec3(0.0, 1.0, 0.0)*cos(theta3);
        vec3 dir = star_pos - center;
        star_pos = dot(dir, rnorm) * xaxis + dir.y*yaxis + center;

        """
    )
    return render_template(code, timeinfo)

def scene_sphere(timeinfo):
    code = (
        get_line_directive() +
        """\
        float t0 = min(`t01`*5.0, 1.0);
        float t1 = clamp(`t01`*5.0 - 1.0, 0.0, 1.0);
        float t2 = clamp(`t01`*5.0 - 2.0, 0.0, 1.0);
        float t3 = clamp(`t01`*5.0 - 3.0, 0.0, 1.0);
        float t4 = clamp(`t01`*5.0 - 4.0, 0.0, 1.0);
        float dg = abs(div_grad_box(star_pos))*0.0625*0.5;
        dg = dg*dg;
        vec3 sphere = normalize(star_pos)*DFsize.x*0.5*sqrt(2.0)*(1.0 - t2*0.25);
        star_pos = mix(star_pos, sphere, (t0 + t2)*(1.0 - t4));
        color_scaling = mix(1.0, min(dg*dg*0.0625*0.125*0.125, 8.0), t1);
        """
    )
    return render_template(code, timeinfo)

def render_anim():
    FPS = 30.0
    duration_scaling = 1.0
    scenes = [(scene_x_slide, 2.0), (get_scene_still(2.0), 1.0), (scene_suck, 2.0), (scene_box_stack, 2.0), (get_scene_still(), 1.0), (scene_plates, 4.0), (scene_sphere, 5.0)]
    #scenes = [(scene_sphere, 5.0)]
    total_nframes = sum(int(i[1]*duration_scaling*FPS) for i in scenes)
    nfrm = 0
    for scn in scenes:
        duration = scn[1] * duration_scaling
        for i in range(int(FPS*duration)):
            timeinfo = TimeInfo(FPS, i, nfrm, duration, duration_scaling)
            render_frame(scn[0](timeinfo), nfrm)
            nfrm += 1
            print("Animation Progress: %d/%d" % (nfrm, total_nframes))

render_anim()
