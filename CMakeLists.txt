cmake_minimum_required(VERSION 3.15)
project(LineEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ------------------------------------------------------------------------------
# Source files
# ------------------------------------------------------------------------------
file(GLOB_RECURSE LGE_SOURCES
    "${CMAKE_SOURCE_DIR}/source/*.cpp"
)

include_directories(${CMAKE_SOURCE_DIR}/hdrs)
include_directories(${CMAKE_SOURCE_DIR}/Shaders)

# ------------------------------------------------------------------------------
# Dependencies: OpenGL / SDL2 / GLEW / glm
# ------------------------------------------------------------------------------

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

# --- SDL2 fallback to pkg-config ---
find_package(PkgConfig QUIET)
if(NOT SDL2_FOUND AND PKG_CONFIG_FOUND)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    include_directories(${SDL2_INCLUDE_DIRS})
endif()

# --- GLEW fallback to pkg-config ---
if(NOT GLEW_FOUND AND PKG_CONFIG_FOUND)
    pkg_check_modules(GLEW REQUIRED glew)
    include_directories(${GLEW_INCLUDE_DIRS})
endif()

# glm
if(glm_FOUND AND TARGET glm::glm)
    set(GLM_LIB glm::glm)
endif()

# ------------------------------------------------------------------------------
# Executable
# ------------------------------------------------------------------------------
add_executable(LineEngine ${LGE_SOURCES})

# ------------------------------------------------------------------------------
# Platform-specific OpenGL link rules
# ------------------------------------------------------------------------------

if(WIN32)
    target_compile_definitions(LineEngine PRIVATE PLATFORM_WINDOWS)
    target_link_libraries(LineEngine PRIVATE opengl32)
elseif(APPLE)
    target_compile_definitions(LineEngine PRIVATE PLATFORM_MAC)
    target_link_libraries(LineEngine PRIVATE "-framework OpenGL")
else() # Linux / Unix
    target_compile_definitions(LineEngine PRIVATE PLATFORM_LINUX)
    if(TARGET OpenGL::GL)
        target_link_libraries(LineEngine PRIVATE OpenGL::GL)
    elseif(OPENGL_gl_LIBRARY)
        target_link_libraries(LineEngine PRIVATE ${OPENGL_gl_LIBRARY})
    endif()
endif()

# ------------------------------------------------------------------------------
# Link SDL2
# ------------------------------------------------------------------------------

if(TARGET SDL2::SDL2)
    target_link_libraries(LineEngine PRIVATE SDL2::SDL2)
elseif(SDL2_LIBRARIES)
    target_link_libraries(LineEngine PRIVATE ${SDL2_LIBRARIES})
endif()

# ------------------------------------------------------------------------------
# Link GLEW
# ------------------------------------------------------------------------------

if(TARGET GLEW::GLEW)
    target_link_libraries(LineEngine PRIVATE GLEW::GLEW)
elseif(GLEW_LIBRARIES)
    target_link_libraries(LineEngine PRIVATE ${GLEW_LIBRARIES})
endif()

# ------------------------------------------------------------------------------
# Link glm
# ------------------------------------------------------------------------------
if(GLM_LIB)
    target_link_libraries(LineEngine PRIVATE ${GLM_LIB})
endif()

# ------------------------------------------------------------------------------
# Copy shaders and test_maps into build folder
# ------------------------------------------------------------------------------
add_custom_command(TARGET LineEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:LineEngine>/Shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/Shaders $<TARGET_FILE_DIR:LineEngine>/Shaders
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:LineEngine>/test_maps
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/test_maps $<TARGET_FILE_DIR:LineEngine>/test_maps
)

message(STATUS "Configured LineEngine (${CMAKE_SYSTEM_NAME}) with ${CMAKE_CXX_COMPILER_ID} C++${CMAKE_CXX_STANDARD}")
