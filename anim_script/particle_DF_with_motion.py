import argparse
import subprocess
import os
import os.path
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

for i in range(0, 300):
    d = dict(cnt = i, rotate_y_mat = rotate_y_mat)
    particleCode = particleTmpl.render(d)
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
    p.communicate(input = particleCode)
    if p.returncode > 0:
    #    print(particleCode[:500])
        break
