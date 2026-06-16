cmake_minimum_required(VERSION 3.15)

project(LineEngine
    VERSION 1.0.0
    DESCRIPTION "3D line map viewer for .lge files"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

include(GNUInstallDirs)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ------------------------------------------------------------------------------
# Options
# ------------------------------------------------------------------------------
option(LGE_INSTALL_DEMO_MAPS "Install bundled test_maps with the package" ON)
option(LGE_BUILD_DEB "Enable CPack DEB package generation" ON)

# ------------------------------------------------------------------------------
# Source files (explicit list — no GLOB)
# ------------------------------------------------------------------------------
set(LGE_SOURCES
    source/Main.cpp
    source/Map.cpp
    source/Shader.cpp
    source/Buffer.cpp
    source/Window.cpp
    source/ParsConvert.cpp
    source/RenderConfig.cpp
    source/Validation.cpp
    source/Paths.cpp
)

# ------------------------------------------------------------------------------
# Dependencies: OpenGL / SDL2 / GLEW / glm
# ------------------------------------------------------------------------------
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

find_package(PkgConfig QUIET)
if(NOT SDL2_FOUND AND PKG_CONFIG_FOUND)
    pkg_check_modules(SDL2 REQUIRED sdl2)
endif()
if(NOT GLEW_FOUND AND PKG_CONFIG_FOUND)
    pkg_check_modules(GLEW REQUIRED glew)
endif()

set(GLM_LIB "")
if(glm_FOUND AND TARGET glm::glm)
    set(GLM_LIB glm::glm)
endif()

# ------------------------------------------------------------------------------
# Executable
# ------------------------------------------------------------------------------
add_executable(lineengine ${LGE_SOURCES})

target_include_directories(lineengine PRIVATE
    ${CMAKE_SOURCE_DIR}/hdrs
)

target_compile_definitions(lineengine PRIVATE
    LGE_DATA_DIR="${CMAKE_INSTALL_FULL_DATADIR}/lineengine"
    LGE_DEV_DATA_DIR="${CMAKE_SOURCE_DIR}"
)

# ------------------------------------------------------------------------------
# Platform-specific OpenGL link rules
# ------------------------------------------------------------------------------
if(WIN32)
    target_compile_definitions(lineengine PRIVATE PLATFORM_WINDOWS)
    target_link_libraries(lineengine PRIVATE opengl32)
elseif(APPLE)
    target_compile_definitions(lineengine PRIVATE PLATFORM_MAC)
    target_link_libraries(lineengine PRIVATE "-framework OpenGL")
else()
    target_compile_definitions(lineengine PRIVATE PLATFORM_LINUX)
    if(TARGET OpenGL::GL)
        target_link_libraries(lineengine PRIVATE OpenGL::GL)
    elseif(OPENGL_gl_LIBRARY)
        target_link_libraries(lineengine PRIVATE ${OPENGL_gl_LIBRARY})
    endif()
endif()

if(TARGET SDL2::SDL2)
    target_link_libraries(lineengine PRIVATE SDL2::SDL2)
elseif(SDL2_LIBRARIES)
    target_include_directories(lineengine PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(lineengine PRIVATE ${SDL2_LIBRARIES})
endif()

if(TARGET GLEW::GLEW)
    target_link_libraries(lineengine PRIVATE GLEW::GLEW)
elseif(GLEW_LIBRARIES)
    target_include_directories(lineengine PRIVATE ${GLEW_INCLUDE_DIRS})
    target_link_libraries(lineengine PRIVATE ${GLEW_LIBRARIES})
endif()

if(GLM_LIB)
    target_link_libraries(lineengine PRIVATE ${GLM_LIB})
endif()

# Backward-compatible alias for existing scripts/docs
add_executable(LineEngine ALIAS lineengine)

# ------------------------------------------------------------------------------
# Dev build: copy shaders and demo maps next to the binary
# ------------------------------------------------------------------------------
add_custom_command(TARGET lineengine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:lineengine>/Shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/Shaders $<TARGET_FILE_DIR:lineengine>/Shaders
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:lineengine>/test_maps
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/test_maps $<TARGET_FILE_DIR:lineengine>/test_maps
)

# ------------------------------------------------------------------------------
# Install rules (Linux desktop integration)
# ------------------------------------------------------------------------------
if(UNIX AND NOT APPLE)
    install(TARGETS lineengine
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    install(DIRECTORY Shaders/
        DESTINATION ${CMAKE_INSTALL_DATADIR}/lineengine/Shaders
        FILES_MATCHING PATTERN "*.glsl"
    )

    if(LGE_INSTALL_DEMO_MAPS)
        install(DIRECTORY test_maps/
            DESTINATION ${CMAKE_INSTALL_DATADIR}/lineengine/test_maps
            FILES_MATCHING PATTERN "*.lge"
        )
    endif()

    install(FILES packaging/lineengine.xml
        DESTINATION ${CMAKE_INSTALL_DATADIR}/mime/packages
    )

    foreach(_icon_size 48 128 256)
        if(EXISTS "${CMAKE_SOURCE_DIR}/icons/hicolor/${_icon_size}x${_icon_size}/apps/lineengine.png")
            install(FILES "icons/hicolor/${_icon_size}x${_icon_size}/apps/lineengine.png"
                DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/${_icon_size}x${_icon_size}/apps
            )
        endif()
    endforeach()

    configure_file(
        packaging/lineengine.desktop.in
        ${CMAKE_BINARY_DIR}/lineengine.desktop
        @ONLY
    )
    install(FILES ${CMAKE_BINARY_DIR}/lineengine.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
    )

    # ------------------------------------------------------------------------------
    # CPack: .deb package
    # ------------------------------------------------------------------------------
    if(LGE_BUILD_DEB)
        set(CPACK_GENERATOR "DEB")
        set(CPACK_PACKAGE_NAME "lineengine")
        set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
        set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
        set(CPACK_PACKAGE_VENDOR "Manvel Sargsyan")
        set(CPACK_PACKAGE_CONTACT "manvel.sargsyan@example.com")
        set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
        set(CPACK_DEBIAN_PACKAGE_SECTION "graphics")
        set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
        set(CPACK_DEBIAN_PACKAGE_DEPENDS
            "libsdl2-2.0-0, libglew2.2, libgl1"
        )
        set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "zenity")
        set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
            "${CMAKE_SOURCE_DIR}/packaging/debian/postinst;${CMAKE_SOURCE_DIR}/packaging/debian/prerm"
        )

        include(CPack)
    endif()
endif()

message(STATUS "Configured LineEngine v${PROJECT_VERSION} (${CMAKE_SYSTEM_NAME})")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Data directory: ${CMAKE_INSTALL_FULL_DATADIR}/lineengine")
