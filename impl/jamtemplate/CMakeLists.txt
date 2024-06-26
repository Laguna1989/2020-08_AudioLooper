file(GLOB_RECURSE JAMTEMPLATEBASEFILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/common/*)

file(GLOB_RECURSE JAMTEMPLATEFILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/sfml/*)

execute_process(
        COMMAND git describe --exact-match --tags
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        RESULT_VARIABLE res
        OUTPUT_VARIABLE GIT_TAG
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

message(STATUS "Jamtemplate repository tag: '${GIT_TAG}'")

execute_process(
        COMMAND git rev-parse -q HEAD
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        RESULT_VARIABLE res
        OUTPUT_VARIABLE GIT_SHA1
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

string(TIMESTAMP CONFIGURE_TIMESTAMP "%Y-%m-%d %H:%M:%S")

configure_file("${CMAKE_SOURCE_DIR}/config/build_info.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/build_info.cpp" @ONLY)
list(APPEND JAMTEMPLATEBASEFILES "${CMAKE_CURRENT_BINARY_DIR}/build_info.cpp")

add_library(JamTemplateLib STATIC ${JAMTEMPLATEFILES} ${JAMTEMPLATEBASEFILES})

# add public includes, so they can be used by dependent targets
target_include_directories(JamTemplateLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common)

target_include_directories(JamTemplateLib SYSTEM PUBLIC ${CMAKE_SOURCE_DIR}/ext/better_enums)


target_include_directories(JamTemplateLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/sfml)
target_include_directories(JamTemplateLib SYSTEM PUBLIC ${SFML_DIR_ROOT}/include)

target_link_libraries(JamTemplateLib
        sfml-system
        sfml-window
        sfml-graphics
)


if (MSVC)
    target_compile_options(JamTemplateLib PRIVATE "/W3")
    target_compile_options(JamTemplateLib PRIVATE "/EHsc")
else ()
    target_compile_options(JamTemplateLib PRIVATE "-Wall")
    target_compile_options(JamTemplateLib PRIVATE "-Wextra")
endif ()

