cmake_minimum_required(VERSION 3.22)
project(jsontoolkit VERSION 1.0.0 LANGUAGES CXX
  DESCRIPTION "The swiss-army knife for JSON programming in C++"
  HOMEPAGE_URL "https://jsontoolkit.sourcemeta.com")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(vendor/noa/cmake/noa.cmake)
include(cmake/CompilerOptions.cmake)

# Options
option(JSONTOOLKIT_URI "Build the JSON Toolkit URI library" ON)
option(JSONTOOLKIT_JSON "Build the JSON Toolkit JSON library" ON)
option(JSONTOOLKIT_JSONSCHEMA "Build the JSON Toolkit JSON Schema library" ON)
option(JSONTOOLKIT_JSONPOINTER "Build the JSON Toolkit JSON Pointer library" ON)
option(JSONTOOLKIT_JSONL "Build the JSON Toolkit JSONL library" ON)
option(JSONTOOLKIT_TESTS "Build the JSON Toolkit tests" OFF)
option(JSONTOOLKIT_CONTRIB "Build the JSON Toolkit extra programs" OFF)
option(JSONTOOLKIT_DOCS "Build the JSON Toolkit docs" OFF)
option(JSONTOOLKIT_INSTALL "Install the JSON Toolkit library" ON)
option(JSONTOOLKIT_ADDRESS_SANITIZER "Build JSON Toolkit with an address sanitizer" OFF)
option(JSONTOOLKIT_UNDEFINED_SANITIZER "Build JSON Toolkit with an undefined behavior sanitizer" OFF)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
configure_package_config_file(
  config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
  COMPATIBILITY SameMajorVersion)
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
  COMPONENT sourcemeta_jsontoolkit_dev)

if(JSONTOOLKIT_URI)
  find_package(UriParser REQUIRED)
  add_subdirectory(src/uri)
endif()

if(JSONTOOLKIT_JSON)
  add_subdirectory(src/json)
endif()

if(JSONTOOLKIT_JSON AND JSONTOOLKIT_JSONPOINTER)
  add_subdirectory(src/jsonpointer)
endif()

if(JSONTOOLKIT_JSON AND JSONTOOLKIT_JSONSCHEMA)
  add_subdirectory(src/jsonschema)
endif()

if(JSONTOOLKIT_JSON AND JSONTOOLKIT_JSONL)
  add_subdirectory(src/jsonl)
endif()

if(JSONTOOLKIT_ADDRESS_SANITIZER)
  noa_sanitizer(TYPE address)
elseif(JSONTOOLKIT_UNDEFINED_SANITIZER)
  noa_sanitizer(TYPE undefined)
endif()

if(JSONTOOLKIT_CONTRIB)
  if(JSONTOOLKIT_JSON)
    add_subdirectory(contrib/prettify)
    add_subdirectory(contrib/minify)
  endif()
  if(JSONTOOLKIT_JSON AND JSONTOOLKIT_JSONSCHEMA)
    add_subdirectory(contrib/jsonschema_walker)
    add_subdirectory(contrib/jsonschema_keywords)
    add_subdirectory(contrib/jsonschema_frame)
  endif()
endif()

if(JSONTOOLKIT_DOCS)
  noa_target_doxygen(CONFIG "${PROJECT_SOURCE_DIR}/doxygen/Doxyfile.in"
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/website")
endif()

if(PROJECT_IS_TOP_LEVEL)
  noa_target_clang_format(SOURCES
    src/*.h src/*.cc
    test/*.h test/*.cc
    contrib/*.h contrib/*.cc)
  noa_target_clang_tidy(SOURCES
    src/*.h src/*.cc)
endif()

# Testing
if(JSONTOOLKIT_TESTS)
  find_package(GoogleTest REQUIRED)
  enable_testing()

  if(JSONTOOLKIT_URI)
    add_subdirectory(test/uri)
  endif()

  if(JSONTOOLKIT_JSON)
    add_subdirectory(test/json)
  endif()

  if(JSONTOOLKIT_JSON AND JSONTOOLKIT_JSONSCHEMA)
    add_subdirectory(test/jsonschema)
  endif()

  if(JSONTOOLKIT_JSON AND JSONTOOLKIT_JSONPOINTER)
    add_subdirectory(test/jsonpointer)
  endif()

  if(JSONTOOLKIT_JSON AND JSONTOOLKIT_JSONL)
    add_subdirectory(test/jsonl)
  endif()

  if(WIN32)
    set(IS_SAME_BIN "${PROJECT_SOURCE_DIR}/test/scripts/is_same.bat")
    set(IS_SAME_STDIN_BIN "${PROJECT_SOURCE_DIR}/test/scripts/is_same_stdin.bat")
  else()
    set(IS_SAME_BIN "${PROJECT_SOURCE_DIR}/test/scripts/is_same.sh")
    set(IS_SAME_STDIN_BIN "${PROJECT_SOURCE_DIR}/test/scripts/is_same_stdin.sh")
  endif()

  if(JSONTOOLKIT_CONTRIB)
    add_subdirectory(test/contrib)
  endif()

  if(PROJECT_IS_TOP_LEVEL)
    # Otherwise we need the child project to link
    # against the sanitizers too.
    if(NOT JSONTOOLKIT_ADDRESS_SANITIZER AND NOT JSONTOOLKIT_UNDEFINED_SANITIZER)
      add_subdirectory(test/packaging)
    endif()
  endif()
endif()
