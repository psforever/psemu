cmake_minimum_required(VERSION 3.1)

project(psemu)

# Platform detection defines
IF (WIN32)
    add_definitions(-DPSEMU_PLATFORM_WIN)
ELSEIF (APPLE)
    add_definitions(-DPSEMU_PLATFORM_OSX)
ELSEIF (UNIX AND NOT APPLE)
    add_definitions(-DPSEMU_PLATFORM_LIN)
    set(LINUX TRUE)
ENDIF ()

# Binary output path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Configurations
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING "" FORCE)

# /Zi = Generate debug info
# /EHsc = C++ exceptions only
set(CMAKE_CXX_FLAGS "/Zi /EHsc" CACHE STRING "" FORCE)

# Silence deprecation warnings
add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)

# Using standalone asio (no boost)
add_definitions(-DASIO_STANDALONE)

# Use static asio lib
set(BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

# Source directories
add_subdirectory(src)
add_subdirectory(externals/cryptopp)

# Print out definitions
get_directory_property(COMPILEDEFS COMPILE_DEFINITIONS)
message("COMPILE_DEFINITIONS = ${COMPILEDEFS}")
