@for /L %%i in (1, 1, 5) do @(
@for /L %%j in (1, 1, 5) do @(
@for /L %%k in (0, 1, 2) do @(
..\projectVC2012\x64\Debug\batchRIshader.exe --output test%%i_%%j_%%k.png --output_w 640  --output_h 480 --num_tile_x %%i --num_tile_y %%j --super_sampling_level %%k grid_fullscr.frag particle.vert particle.frag
)))
