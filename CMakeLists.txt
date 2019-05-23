cmake_minimum_required(VERSION 3.12)
include(CMakePackageConfigHelpers)

project(libcommon VERSION 0.1 LANGUAGES C CXX)

find_package(codegen CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)

if(NOT MSVC)
  find_library(FS_LIBRARY c++fs)
endif()

include(codegen)

file(GLOB_RECURSE source_files
    "Source/*.cpp"
    "Source/*.h"
    "Source/*.hpp"
)

add_library(libcommon ${source_files})

target_compile_features(libcommon PRIVATE cxx_std_17 c_std_11)

# Make MSVC act like a real C/C++ compiler
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options(libcommon PRIVATE "/permissive-")
endif()

target_include_directories(libcommon
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source>
)

target_link_libraries(
    libcommon
    PUBLIC tinyxml2::tinyxml2
    PRIVATE codegen::codegen "${FS_LIBRARY}"
)

get_target_property(libcommon_include_directories libcommon INCLUDE_DIRECTORIES)

add_codegen_targets(
    "${source_files}"
    codegen_generated_files
    "${PROJECT_SOURCE_DIR}"
    "${PROJECT_BINARY_DIR}"
    "${libcommon_include_directories}"
)
add_custom_target(libcommon_codegen DEPENDS ${codegen_generated_files} SOURCES ${source_files})

# Add the generated sources to the library target
target_sources(libcommon PRIVATE ${codegen_generated_files})

# Install the project headers
install(
    DIRECTORY "${PROJECT_SOURCE_DIR}/Source/Common"
    DESTINATION "include/libcommon"
    PATTERN "*.cpp" EXCLUDE
)

set(version_config_file "${PROJECT_BINARY_DIR}/libcommonConfigVersion.cmake")
set(config_file "${PROJECT_BINARY_DIR}/libcommonConfig.cmake")
set(config_install_dir "lib/cmake/libcommon")

# Associate target with export
install(
    TARGETS libcommon
    EXPORT libcommonTargets
    ARCHIVE DESTINATION "lib"
    INCLUDES DESTINATION "include/libcommon"  # This sets the INTERFACE_INCLUDE_DIRECTORIES property of the target.
)

# Install the target config files
install(
    EXPORT libcommonTargets
    NAMESPACE "libcommon::"
    DESTINATION "${config_install_dir}"
)

# Generate version config file
write_basic_package_version_file(
    "${version_config_file}"
    COMPATIBILITY SameMajorVersion
)

# Generate config file
configure_package_config_file(
    "Config.cmake.in"
    "${config_file}"
    INSTALL_DESTINATION "lib/cmake/libcommon"
)

# Install the config files
install(
    FILES "${config_file}" "${version_config_file}"
    DESTINATION ${config_install_dir}
)
