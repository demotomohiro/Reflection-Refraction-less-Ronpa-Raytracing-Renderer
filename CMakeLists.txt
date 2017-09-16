cmake_minimum_required(VERSION 3.0)

if(NOT WIN32)
    option(BR_USE_EGL "Create OpenGL context using EGL" on)
endif()

project(batchRIshader)
add_executable(batchRIshader
src/batchRIshader.cpp
src/frame_buffer_object.cpp
src/glcontext.cpp
src/image_file.cpp
src/options.cpp
src/renderer.cpp
)
if(WIN32)
    target_sources(batchRIshader PRIVATE src/glcontext_wgl.cpp)
endif()

set_target_properties(batchRIshader PROPERTIES CXX_STANDARD 14)
set_target_properties(batchRIshader PROPERTIES CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(Boost_USE_STATIC_LIBS ON)
endif()
if(CMAKE_VERSION VERSION_LESS 3.5)
    find_package(Boost REQUIRED COMPONENTS wave program_options thread filesystem system)
else()
    find_package(Boost REQUIRED COMPONENTS wave program_options)
endif()
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(batchRIshader ${Boost_LIBRARIES})

find_package(GLEW REQUIRED)
find_package(PNG REQUIRED)
target_link_libraries(batchRIshader GLEW::GLEW PNG::PNG)

if(BR_USE_EGL)
    find_library(EGL_LIBRARY EGL)
    #Without linking GLESv2, glGetString always returns 0 when using EGL.
    find_library(GLESv2_LIBRARY GLESv2)
endif()

if(NOT BR_USE_EGL OR NOT EGL_LIBRARY OR NOT GLESv2_LIBRARY)
    find_package(OpenGL REQUIRED)
    if(CMAKE_VERSION VERSION_LESS 3.8)
        target_link_libraries(batchRIshader ${OPENGL_gl_LIBRARY})
        include_directories(${OPENGL_INCLUDE_DIR})
    else()
        target_link_libraries(batchRIshader OpenGL::GL)
    endif()

    if(UNIX)
        find_package(X11 REQUIRED)
        target_link_libraries(batchRIshader ${X11_X11_LIB})
        target_sources(batchRIshader PRIVATE src/glcontext_glx.cpp)
    endif()
else()
    target_sources(batchRIshader PRIVATE src/glcontext_egl.cpp)
    set_property(TARGET batchRIshader APPEND PROPERTY COMPILE_DEFINITIONS USE_EGL)
    target_link_libraries(batchRIshader ${EGL_LIBRARY} ${GLESv2_LIBRARY})
endif()

find_package(GLUtil CONFIG REQUIRED contextUtil shader cStdShader)
target_link_libraries(batchRIshader GLUtil::contextUtil GLUtil::shader GLUtil::cStdShader)

enable_testing()
add_test(
    NAME grid_test
    COMMAND batchRIshader --num_particles 256000 --output ${CMAKE_BINARY_DIR}/grid_image.png --output_w 1920  --output_h 1080 --num_tile_x 2 --num_tile_y 2 --super_sampling_level 1 grid_fullscr.frag particle_star.vert particle_star.frag
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
    NAME test_simplexnoise3d
    COMMAND batchRIshader --output ${CMAKE_BINARY_DIR}/test_simplexnoise3d.png --output_w 1920 --output_h 1080 test_simplexnoise3d.frag
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/shaders
    )

foreach(i RANGE 0)
    math(EXPR n "65536 << ${i}")
add_test(
    NAME random_dots${n}
    COMMAND batchRIshader --num_particles ${n} --super_sampling_level 0 --output ${CMAKE_BINARY_DIR}/random_dots${n}.png --output_w 1024 --output_h 1024 grid_fullscr.frag random_dots.vert vary_color.frag
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/shaders
    )
endforeach(i)

math(EXPR width "1920")
math(EXPR height "1080")
foreach(i RANGE 0)
    math(EXPR n "65536 << (${i}+8)")
add_test(
    NAME particle_simple${n}
    COMMAND batchRIshader --num_particles ${n} --super_sampling_level 2 --output ${CMAKE_BINARY_DIR}/particle_simple${n}.png --output_w ${width} --output_h ${height} main_shader.frag particle_simple.vert particle_star.frag
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/shaders
    )
endforeach(i)

#math(EXPR n "65536 << 12")
#foreach(i RANGE 2)
#    math(EXPR width "1920*(1<<${i})")
#    math(EXPR height "1080*(1<<${i})")
#add_test(
#    NAME particle_simple${n}_${width}x${height}
#    COMMAND batchRIshader --num_particles ${n} --super_sampling_level 1 --num_tile_x 16 --num_tile_y 16 --output ${CMAKE_BINARY_DIR}/particle_simple${n}_${width}x${height}.png --output_w ${width} --output_h ${height} main_shader.frag particle_simple.vert particle.frag
#    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/shaders
#    )
#endforeach(i)

math(EXPR n "65536 << (8)")
math(EXPR width "1920")
math(EXPR height "1080")
foreach(i RANGE 32)
math(EXPR rot "${i}")
set(test_name particle_DF_${n}_rot${rot})
add_test(
    NAME ${test_name}
    COMMAND batchRIshader --num_particles ${n} --super_sampling_level 2 --num_tile_x 1 --num_tile_y 1 --output ${CMAKE_BINARY_DIR}/${test_name}.png --output_w ${width} --output_h ${height} "-DROT_THETA=(${rot}/32.0*3.141)" main_shader.frag particle_DF.vert particle_DF.frag
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/shaders
    )
endforeach(i)
