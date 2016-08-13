cmake_minimum_required(VERSION 3.0)
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
set_target_properties(batchRIshader PROPERTIES CXX_STANDARD 14)
set_target_properties(batchRIshader PROPERTIES CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(Boost_USE_STATIC_LIBS ON)
endif()
if(CMAKE_VERSION VERSION_LESS 3.5)
    find_package(Boost REQUIRED COMPONENTS program_options wave thread filesystem system)
else()
    find_package(Boost REQUIRED COMPONENTS program_options wave)
endif()
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(batchRIshader ${Boost_LIBRARIES})

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

if(UNIX)
    find_package(X11 REQUIRED)
    target_link_libraries(batchRIshader ${X11_X11_LIB})
endif()

enable_testing()
add_test(
    NAME grid_test
    COMMAND batchRIshader --num_particles 256000 --output ${CMAKE_BINARY_DIR}/grid_image.png --output_w 1920  --output_h 1080 --num_tile_x 2 --num_tile_y 2 --super_sampling_level 1 grid_fullscr.frag particle.vert particle.frag
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/shaders
    )

add_test(
    NAME default_test
    COMMAND batchRIshader --output ${CMAKE_BINARY_DIR}/default.png --output_w 1920 --output_h 1080
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/shaders
    )

add_test(
    NAME test_frag_shader
    COMMAND batchRIshader --output ${CMAKE_BINARY_DIR}/test_frag_shader.png --output_w 1920 --output_h 1080 test_frag_shader.frag
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/shaders
    )

add_test(
    NAME random_dots
    COMMAND batchRIshader --num_particles 4096 --super_sampling_level 0 --output ${CMAKE_BINARY_DIR}/random_dots.png --output_w 512 --output_h 512 grid_fullscr.frag random_dots.vert vary_color.frag
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/shaders
    )
