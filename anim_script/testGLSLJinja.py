import argparse
import subprocess
import os
import os.path
from GLSLJinja import GLSLJinjaLoader

parser = argparse.ArgumentParser(description="Generate multiple images using batchRIshader")
parser.add_argument("exe", help="path to the executable file 'batchRIshader'")
args = parser.parse_args()

outputDir   = os.path.abspath(os.getcwd())
outputName  = os.path.basename(__file__)
srcDir      = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
print("srcDir: " + srcDir, flush=True)
batchRIshader = os.path.abspath(args.exe)
shaderDir = os.path.join(srcDir, "shaders")
os.chdir(shaderDir)

loader = GLSLJinjaLoader(os.path.join(srcDir, "anim_script"))
fragTmpl = loader.get_template("testGLSLJinja.frag.tmpl")
particleTmpl = loader.get_template("testGLSLJinja.vert.tmpl")

for i in range(0, 30):
    d = dict(time = str(i)+"/29.")
    fragCode = fragTmpl.render(d)
    particleCode = particleTmpl.render(d)
    p = subprocess.Popen(
        [
            batchRIshader,
            "--super_sampling_level",   str(0),
            "--num_tile_x",             str(1),
            "--num_tile_y",             str(1),
            "--output",                 os.path.join(outputDir, outputName + "%d.png" % i),
            "--output_w",               str(1920),
            "--output_h",               str(1080),
            "-",
            "-",
            "particle_DF.frag"
        ],
        universal_newlines = True,
        stdin = subprocess.PIPE
        )
    p.communicate(input = fragCode + "\0" + particleCode)
    if p.returncode > 0:
        print(particleCode)
        break
