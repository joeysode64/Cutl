cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_STANDARD 23)

project(
    Cutl
    VERSION 0.3.3
    LANGUAGES C
)

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

# Build library.

file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.c")
add_library(cutl ${SOURCES})

target_compile_options(cutl PRIVATE
    -Wall -Wextra -Wpedantic -Werror -g
)

target_link_libraries(cutl PRIVATE
    glfw
    Vulkan::Vulkan
)


configure_file(
    ${CMAKE_SOURCE_DIR}/include/info.h.in
    ${CMAKE_BINARY_DIR}/generated/info.h
    @ONLY
)

target_include_directories(cutl PUBLIC ${CMAKE_BINARY_DIR}/generated)
target_include_directories(cutl PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
)

# Compile tests.

file(GLOB TEST_SOURCES "${CMAKE_SOURCE_DIR}/tests/*.c")

foreach(test_file ${TEST_SOURCES})
    get_filename_component(test_name ${test_file} NAME_WE)

    add_executable(${test_name} ${test_file})

    target_link_libraries(${test_name} PRIVATE
        cutl
    )

    target_include_directories(${test_name} PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
    )
endforeach()

# Compile test shaders.

file(GLOB TEST_SHADERS "${CMAKE_SOURCE_DIR}/tests/ssrc/*")

foreach(shader_file ${TEST_SHADERS})
    get_filename_component(shader_name ${shader_file} NAME)
    set(shader_output "${CMAKE_SOURCE_DIR}/tests/shaders/${shader_name}.spv")
    
    add_custom_command(
        OUTPUT ${shader_output}
        COMMAND glslc ${shader_file} -o ${shader_output}
        DEPENDS ${shader_file}
        COMMENT "Compiling shader ${shader_name}"
    )
    
    list(APPEND COMPILED_SHADERS ${shader_output})
endforeach()

if(COMPILED_SHADERS)
    add_custom_target(test_shaders ALL DEPENDS ${COMPILED_SHADERS})
    
    # Make tests depend on shaders
    file(GLOB TEST_SOURCES "${CMAKE_SOURCE_DIR}/tests/*.c")
    foreach(test_file ${TEST_SOURCES})
        get_filename_component(test_name ${test_file} NAME_WE)
        add_dependencies(${test_name} test_shaders)
    endforeach()
endif()
