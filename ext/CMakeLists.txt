message(STATUS "Add FetchContent")
include(FetchContent)

message(STATUS "Add CPM.cmake")
# download CPM.cmake
file(
        DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.38.6/CPM.cmake
        ${CMAKE_BINARY_DIR}/cmake/CPM.cmake
        EXPECTED_HASH SHA256=11c3fa5f1ba14f15d31c2fb63dbc8628ee133d81c8d764caad9a8db9e0bacb07
)
include(${CMAKE_BINARY_DIR}/cmake/CPM.cmake)

message(STATUS "Fetching external libraries started")

add_subdirectory(sfml)

message(STATUS "Fetching external libraries done")
