file(GLOB_RECURSE GAMELIBFILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*)

add_library(GameLib STATIC ${GAMELIBFILES})
if (JT_ENABLE_WEB)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif ()

# Jamtemplate setup
target_link_libraries(GameLib PUBLIC JamTemplateLib)
target_include_directories(GameLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# FMod setup
target_include_directories(GameLib PUBLIC ${FMOD_DIR}/api/core/inc)
target_include_directories(GameLib PUBLIC ${FMOD_DIR}/api/studio/inc)
target_link_directories(GameLib PUBLIC ${FMOD_DIR}/api/core/lib/x86_64)
target_link_directories(GameLib PUBLIC ${FMOD_DIR}/api/studio/lib/x86_64)

target_link_libraries(GameLib PUBLIC ${FMOD_DIR}/api/core/lib/x86_64/libfmod.so)
target_link_libraries(GameLib PUBLIC ${FMOD_DIR}/api/studio/lib/x86_64/libfmodstudio.so)

# compiler options
if (MSVC)
    target_compile_options(GameLib PRIVATE "/W3")
    target_compile_options(GameLib PRIVATE "/EHsc")
else ()
    target_compile_options(GameLib PRIVATE "-Wall")
    target_compile_options(GameLib PRIVATE "-Wextra")
endif ()
