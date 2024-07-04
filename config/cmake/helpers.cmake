function(jt_use_assets TGT)
    add_custom_command(TARGET ${TGT} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets)

    if (MSVC)
        add_custom_command(TARGET ${TGT} PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/Debug/assets)
        add_custom_command(TARGET ${TGT} PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/Release/assets)
    endif ()
endfunction()

function(jt_use_fmod TGT)
    if (MSVC)
    else ()
        add_custom_command(TARGET ${TGT} PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                ${FMOD_DIR}/api/core/lib/x86_64/libfmod.* ${CMAKE_CURRENT_BINARY_DIR}/)
    endif ()
endfunction()

function(target_link_libraries_system target)
    set(libs ${ARGN})
    foreach (lib ${libs})
        get_target_property(lib_include_dirs ${lib} INTERFACE_INCLUDE_DIRECTORIES)
        target_include_directories(${target} SYSTEM PRIVATE ${lib_include_dirs})
        target_link_libraries(${target} ${lib})
    endforeach (lib)
endfunction(target_link_libraries_system)


function(mac_sdl_setup)
    if (NOT USE_SFML)
        if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
            include_directories(/usr/local/Cellar/sdl2/2.0.22/include/
                    /usr/local/Cellar/sdl2_image/2.0.5/include/
                    /usr/local/Cellar/sdl2_ttf/2.0.18_1/include/)
        endif ()
    endif ()
endfunction()

function(setup_sfml)
    FetchContent_GetProperties(sfml)
    # Apple users: set to /usr/local/Cellar/sfml/2.4.2_1/lib/ or respectively
    if (WIN32)
        set(SFML_DIR_ROOT "${sfml_SOURCE_DIR}" CACHE PATH "Path to SFML root dir.")
    endif ()

    if ($ENV{CLION_IDE})
        set(SFML_DIR "${SFML_DIR_ROOT}/lib/cmake/SFML")
    endif ()
    if (WIN32 OR APPLE)
        link_directories(${SFML_DIR_ROOT}/lib)
    endif ()
endfunction()
