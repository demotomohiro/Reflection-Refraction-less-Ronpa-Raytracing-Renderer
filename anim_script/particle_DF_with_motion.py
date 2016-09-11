import argparse
import subprocess
import os
import os.path

parser = argparse.ArgumentParser(description="Generate multiple images using batchRIshader with particle_DF_with_motion.vert")
parser.add_argument("exe", help="path to the executable file 'batchRIshader'")
parser.add_argument("outDir", help="output directory")
args = parser.parse_args()

outputDir   = os.path.abspath(os.getcwd());
srcDir      = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
print("srcDir: " + srcDir, flush=True)
batchRIshader = os.path.abspath(args.exe)
shaderDir = os.path.join(srcDir, "shaders")
os.chdir(shaderDir)

for i in range(0, 300):
    if subprocess.call(
        [
            batchRIshader,
            "--num_particles",          str(65536 << (13-5)),
            "--num_div_particles",      str(64),
            "--super_sampling_level",   str(0),
            "--num_tile_x",             str(1),
            "--num_tile_y",             str(1),
            "--output",                 os.path.join(outputDir, "particle_DF_frame%d.png" % i),
            "--output_w",               str(1920),
            "--output_h",               str(1080),
            "-DROT_THETA=(%d/120.0*3.141)" % i,
            "-DCNT=%d" % i,
            "main_shader.frag",
            "particle_DF_with_motion.vert",
            "particle_DF.frag"]
        ) > 0:
        break
