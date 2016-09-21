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

def rotate_y_mat(theta):
    return """
        mat3(
	cos({0}), 0.0,    -sin({0}),
	0.0,	    1.0,     0.0,
	sin({0}), 0.0,     cos({0}))
    """.format(theta)

def render_frame(source, i):
    p = subprocess.Popen(
        [
            batchRIshader,
            "--num_particles",          str(65536 << (13-5)),
            "--num_div_particles",      str(64),
            "--super_sampling_level",   str(0),
            "--num_tile_x",             str(1),
            "--num_tile_y",             str(1),
            "--output",                 os.path.join(outputDir, outputName + "_frm%d.png" % i),
            "--output_w",               str(1920),
            "--output_h",               str(1080),
            "-DROT_THETA=(%d/120.0*3.141)" % i,
            "-DCNT=%d" % i,
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

def render_template(include, time, i):
    tmpl = loader.get_includable_template_from_string(include)

    d = dict(cnt = i, time = time, rotate_y_mat = rotate_y_mat, tmpl = tmpl)
    return particleTmpl.render(d)

def scene0(time, i):
    code = (
        get_line_directive() +
        """\
        float t = max(1.0 - `time` - (-star_pos.z+0.09)*4.0, 0.0);
        float x0 = (star_pos.x+0.25);
        star_pos.x += pow(t*8., x0*8.0+0.25)*3.0 + t*x0*8.0;
    //    star_pos.x = min(star_pos.x, 0.499);
        """
    )
    render_frame(render_template(code, time, i), i)

def scene1(time, i):
    code = (
        get_line_directive() +
        """\
        vec3 rnorm = normalize(round(normal));
        float dot = dot(rnorm, star_pos);
        const float dmin = 0.03, dmax = 0.15;
        float d = (abs(dot) - dmin)/(dmax - dmin);
        float t = max(`time` - (1.0 - d), 0.0);
        star_pos += /*normalize(star_pos)*/sign(dot)*rnorm*t*t*2.;
        """
    )
    render_frame(render_template(code, time, i), i)

def render_anim():
    FPS = 30.0
    scenes = [(scene0, 2.0), (scene1, 2.0)]
    nfrm = 0
    for scn in scenes:
        for i in range(int(FPS*scn[1])):
            scn[0](i/FPS, nfrm)
            nfrm += 1

render_anim()
