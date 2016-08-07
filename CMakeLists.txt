cmake_minimum_required(VERSION 2.8)
set_property(GLOBAL PROPERTY CXX_STANDARD 14)
set_property(GLOBAL PROPERTY CXX_STANDARD_REQUIRED ON)
project(batchRIshader)
add_executable(batchRIshader
src/batchRIshader.cpp
src/cstd_shader.cpp
src/frame_buffer_object.cpp
src/glcontext.cpp
src/image_file.cpp
src/options.cpp
src/shader.cpp
)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS program_options wave)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(batchRIshader Boost::program_options Boost::wave)

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
target_link_libraries(batchRIshader ${GLEW_LIBRARIES})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(batchRIshader ${OPENGL_gl_LIBRARY})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIRS})
target_link_libraries(batchRIshader ${PNG_LIBRARIES})
add_definitions(${PNG_DEFINITIONS})


enable_testing()
add_test(
    NAME test
    COMMAND batchRIshader --num_particles 256000 --output ${CMAKE_BINARY_DIR}/image.png --output_w 1920  --output_h 1080 --num_tile_x 2 --num_tile_y 2 --super_sampling_level 1 grid_fullscr.frag particle.vert particle.frag
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/shaders
    )

