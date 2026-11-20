# wfc-mscomctl2: Windows Common Controls-2 (mscomct2.ocx, library MSComCtl2).
#
# The member table is generated from the type-library evidence
# (scripts/New-UiControlTables.ps1 in WFC).
# The table is built static by default; WFC_MSCOMCTL2_SHARED builds it as
# wfcmscomctl2.dll so a program can load it at run time instead. The version is
# the one in VERSION and is stamped into the DLL's version resource.
#
# Parent project variables used when present: WFC_OCX_SHARED (the default of
# WFC_MSCOMCTL2_SHARED) and the functions wfc_enable_strict_warnings and
# wsp_enable_hardening.

# Standalone build: the library outside WFC. WFC itself adds this directory
# with add_subdirectory and supplies the project, CTest and the WSP hardening.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  cmake_minimum_required(VERSION 3.28)
  project(wfc-mscomctl2 LANGUAGES CXX)
  if(WFC_WINELIB)
    # winegcc writes name.exe (a launcher) and name.exe.so; the platform module
    # resets the suffix when the project is declared, so it is set here.
    set(CMAKE_EXECUTABLE_SUFFIX ".exe")
  endif()
  option(WFC_BUILD_TESTS "Build the tests" ON)
  include(CTest)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
  include(WfcWarnings)
  # WSP-SEC-0015: the pinned WSP baseline supplies the hardening flags (not for
  # the WebAssembly and Winelib targets, whose toolchains do not take them).
  if(
    EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/wsp/tools/cmake/WspBuild.cmake"
    AND NOT EMSCRIPTEN
    AND NOT WFC_WINELIB
  )
    include("${CMAKE_CURRENT_SOURCE_DIR}/wsp/tools/cmake/WspBuild.cmake")
  endif()
endif()

# Test names. Inside this library's own repository a CTest name is
# TC-NNNN-<name>. When another project mounts the library (add_subdirectory),
# the names carry the library name, wfc-mscomctl2-TC-NNNN-<name>, so that the
# names of different libraries never collide. WFC_MSCOMCTL2_TEST_PREFIX holds
# that prefix: empty for the top-level project, "wfc-mscomctl2-" otherwise. The
# including project may set it (even to empty) before add_subdirectory. The
# TC-NNNN identifiers inside the names never change. (PROJECT_IS_TOP_LEVEL is
# inherited by subdirectories of the parent, so the source directory decides.)
if(NOT DEFINED WFC_MSCOMCTL2_TEST_PREFIX)
  if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(WFC_MSCOMCTL2_TEST_PREFIX "")
  else()
    set(WFC_MSCOMCTL2_TEST_PREFIX "wfc-mscomctl2-")
  endif()
endif()
set(wfc_tp "${WFC_MSCOMCTL2_TEST_PREFIX}")

if(NOT DEFINED WFC_OCX_SHARED)
  set(WFC_OCX_SHARED OFF)
endif()
option(
  WFC_MSCOMCTL2_SHARED
  "Build the WFC mscomctl2 library as wfcmscomctl2.dll"
  ${WFC_OCX_SHARED}
)

file(
  STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION"
  WFC_MSCOMCTL2_VERSION
  LIMIT_COUNT 1
)
set(
  WFC_MSCOMCTL2_VERSION
  "${WFC_MSCOMCTL2_VERSION}"
  CACHE INTERNAL
  "wfcmscomctl2 version"
)

set(WFC_MSCOMCTL2_SOURCES src/mscomctl2_tables.cpp)

if(WFC_MSCOMCTL2_SHARED AND WIN32)
  enable_language(RC)
  string(REPLACE "." ";" version_parts "${WFC_MSCOMCTL2_VERSION}")
  list(GET version_parts 0 WFC_LIB_VERSION_MAJOR)
  list(GET version_parts 1 WFC_LIB_VERSION_MINOR)
  list(GET version_parts 2 WFC_LIB_VERSION_PATCH)
  set(WFC_LIB_VERSION "${WFC_MSCOMCTL2_VERSION}")
  file(
    STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/TARGET"
    WFC_LIB_TARGET
    LIMIT_COUNT 1
  )
  configure_file(
    src/registration_info.hpp.in
    "${CMAKE_CURRENT_BINARY_DIR}/registration_info.hpp"
    @ONLY
  )
  set(WFC_LIB_NAME wfcmscomctl2)
  set(WFC_LIB_DESCRIPTION "WFC Windows Common Controls-2")
  configure_file(
    version.rc.in
    "${CMAKE_CURRENT_BINARY_DIR}/wfcmscomctl2.rc"
    @ONLY
  )
  add_library(
    wfc_mscomctl2
    SHARED
    ${WFC_MSCOMCTL2_SOURCES}
    src/registration.cpp
    src/wfcmscomctl2.def
    "${CMAKE_CURRENT_BINARY_DIR}/wfcmscomctl2.rc"
  )
  target_include_directories(
    wfc_mscomctl2
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
  )
  target_link_libraries(wfc_mscomctl2 PRIVATE advapi32 ole32 oleaut32)
  target_compile_definitions(
    wfc_mscomctl2
    PUBLIC WFC_MSCOMCTL2_SHARED
    PRIVATE WFC_MSCOMCTL2_BUILD_DLL
  )
  set_target_properties(
    wfc_mscomctl2
    PROPERTIES
      OUTPUT_NAME wfcmscomctl2
      VERSION "${WFC_MSCOMCTL2_VERSION}"
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
  )
else()
  add_library(wfc_mscomctl2 STATIC ${WFC_MSCOMCTL2_SOURCES})
  set_target_properties(
    wfc_mscomctl2
    PROPERTIES OUTPUT_NAME wfcmscomctl2_static
  )
endif()
target_include_directories(
  wfc_mscomctl2
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_compile_features(wfc_mscomctl2 PRIVATE cxx_std_20)
if(COMMAND wfc_enable_strict_warnings)
  wfc_enable_strict_warnings(wfc_mscomctl2)
endif()
if(COMMAND wsp_enable_hardening)
  wsp_enable_hardening(wfc_mscomctl2)
endif()

# --- Tests --------------------------------------------------------------------
# The table (any build) and, in the shared build, the DLL itself: a C program
# loads wfcmscomctl2.dll, checks its COM entry points and version resource, and
# registers and unregisters it in the registry of the current user.
if(BUILD_TESTING AND WFC_BUILD_TESTS)
  add_executable(mscomctl2_tables_test tests/mscomctl2_tables_test.cpp)
  target_link_libraries(mscomctl2_tables_test PRIVATE wfc_mscomctl2)
  if(WFC_MSCOMCTL2_SHARED)
    # Next to the DLL, so the program finds it.
    set_target_properties(
      mscomctl2_tables_test
      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
  endif()
  target_compile_features(mscomctl2_tables_test PRIVATE cxx_std_20)
  if(COMMAND wfc_enable_strict_warnings)
    wfc_enable_strict_warnings(mscomctl2_tables_test)
  endif()
  if(COMMAND wsp_enable_hardening)
    wsp_enable_hardening(mscomctl2_tables_test)
  endif()
  add_test(NAME ${wfc_tp}TC-0001-tables COMMAND mscomctl2_tables_test)
  set_tests_properties(
    ${wfc_tp}TC-0001-tables
    PROPERTIES LABELS "unit;mscomctl2" TIMEOUT 120
  )

  add_executable(mscomctl2_updown_test tests/updown_test.cpp)
  target_link_libraries(mscomctl2_updown_test PRIVATE wfc_mscomctl2)
  target_compile_features(mscomctl2_updown_test PRIVATE cxx_std_20)
  if(COMMAND wfc_enable_strict_warnings)
    wfc_enable_strict_warnings(mscomctl2_updown_test)
  endif()
  if(COMMAND wsp_enable_hardening)
    wsp_enable_hardening(mscomctl2_updown_test)
  endif()
  add_test(NAME ${wfc_tp}TC-0002-updown-model COMMAND mscomctl2_updown_test)
  set_tests_properties(
    ${wfc_tp}TC-0002-updown-model
    PROPERTIES LABELS "unit;mscomctl2" TIMEOUT 120
  )

  add_executable(mscomctl2_scrollbar_test tests/scrollbar_test.cpp)
  target_link_libraries(mscomctl2_scrollbar_test PRIVATE wfc_mscomctl2)
  target_compile_features(mscomctl2_scrollbar_test PRIVATE cxx_std_20)
  if(COMMAND wfc_enable_strict_warnings)
    wfc_enable_strict_warnings(mscomctl2_scrollbar_test)
  endif()
  if(COMMAND wsp_enable_hardening)
    wsp_enable_hardening(mscomctl2_scrollbar_test)
  endif()
  add_test(
    NAME ${wfc_tp}TC-0003-scrollbar-model
    COMMAND mscomctl2_scrollbar_test
  )
  set_tests_properties(
    ${wfc_tp}TC-0003-scrollbar-model
    PROPERTIES LABELS "unit;mscomctl2" TIMEOUT 120
  )

  add_executable(mscomctl2_monthview_test tests/monthview_test.cpp)
  target_link_libraries(mscomctl2_monthview_test PRIVATE wfc_mscomctl2)
  target_compile_features(mscomctl2_monthview_test PRIVATE cxx_std_20)
  if(COMMAND wfc_enable_strict_warnings)
    wfc_enable_strict_warnings(mscomctl2_monthview_test)
  endif()
  if(COMMAND wsp_enable_hardening)
    wsp_enable_hardening(mscomctl2_monthview_test)
  endif()
  add_test(
    NAME ${wfc_tp}TC-0004-monthview-model
    COMMAND mscomctl2_monthview_test
  )
  set_tests_properties(
    ${wfc_tp}TC-0004-monthview-model
    PROPERTIES LABELS "unit;mscomctl2" TIMEOUT 120
  )

  add_executable(mscomctl2_dtpicker_test tests/dtpicker_test.cpp)
  target_link_libraries(mscomctl2_dtpicker_test PRIVATE wfc_mscomctl2)
  target_compile_features(mscomctl2_dtpicker_test PRIVATE cxx_std_20)
  if(COMMAND wfc_enable_strict_warnings)
    wfc_enable_strict_warnings(mscomctl2_dtpicker_test)
  endif()
  if(COMMAND wsp_enable_hardening)
    wsp_enable_hardening(mscomctl2_dtpicker_test)
  endif()
  add_test(NAME ${wfc_tp}TC-0005-dtpicker-model COMMAND mscomctl2_dtpicker_test)
  set_tests_properties(
    ${wfc_tp}TC-0005-dtpicker-model
    PROPERTIES LABELS "unit;mscomctl2" TIMEOUT 120
  )

  if(WIN32 AND WFC_MSCOMCTL2_SHARED)
    enable_language(C)
    add_executable(mscomctl2_dll_test tests/mscomctl2_dll_test.c)
    target_link_libraries(mscomctl2_dll_test PRIVATE version advapi32)
    add_dependencies(mscomctl2_dll_test wfc_mscomctl2)
    if(MSVC)
      target_compile_options(mscomctl2_dll_test PRIVATE /W4 /WX)
    else()
      target_compile_options(mscomctl2_dll_test PRIVATE -Wall -Wextra -Werror)
    endif()
    if(COMMAND wsp_enable_hardening)
      wsp_enable_hardening(mscomctl2_dll_test)
    endif()
    add_test(
      NAME ${wfc_tp}TC-0006-dll
      COMMAND
        mscomctl2_dll_test "$<TARGET_FILE:wfc_mscomctl2>"
        "${WFC_MSCOMCTL2_VERSION}" "${WFC_LIB_TARGET}"
    )
    set_tests_properties(
      ${wfc_tp}TC-0006-dll
      PROPERTIES
        LABELS "integration;mscomctl2"
        RESOURCE_LOCK registry
        SKIP_RETURN_CODE 77
    )
  endif()
endif()

# The corpus programs of tests/corpus run through the wfc executable of WFC
# (WFC_EXE). WFC itself runs them when it mounts the library, so this is the
# standalone build only; without WFC_EXE the cases report as skipped.
if(
  BUILD_TESTING
  AND WFC_BUILD_TESTS
  AND CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR
)
  set(WFC_EXE "" CACHE FILEPATH "wfc executable that runs the corpus programs")
  set(
    wfc_corpus_cases
    "0007|corpus-updown|211-mscomctl2-updown"
    "0008|corpus-flatscrollbar|212-mscomctl2-flatscrollbar"
    "0009|corpus-updown-buddy|213-mscomctl2-updown-buddy"
    "0010|corpus-monthview|214-mscomctl2-monthview"
    "0011|corpus-dtpicker|215-mscomctl2-dtpicker"
    "0012|corpus-animation|217-mscomctl2-animation"
    "0013|corpus-getdaybold|218-mscomctl2-getdaybold"
    "0014|corpus-dtpicker-callback|219-mscomctl2-dtpicker-callback"
    "0015|corpus-monthview-hittest|220-mscomctl2-monthview-hittest"
    "0016|corpus-calendar-colors|221-mscomctl2-calendar-colors"
  )
  foreach(wfc_corpus_case IN LISTS wfc_corpus_cases)
    string(REPLACE "|" ";" wfc_corpus_parts "${wfc_corpus_case}")
    list(GET wfc_corpus_parts 0 wfc_corpus_id)
    list(GET wfc_corpus_parts 1 wfc_corpus_slug)
    list(GET wfc_corpus_parts 2 wfc_corpus_name)
    set(wfc_corpus_dir "${CMAKE_CURRENT_SOURCE_DIR}/tests/corpus")
    set(wfc_corpus_debug "")
    if(EXISTS "${wfc_corpus_dir}/${wfc_corpus_name}.debug")
      set(
        wfc_corpus_debug
        "-DDEBUG_EXPECTED=${wfc_corpus_dir}/${wfc_corpus_name}.debug"
      )
    endif()
    add_test(
      NAME ${wfc_tp}TC-${wfc_corpus_id}-${wfc_corpus_slug}
      COMMAND
        "${CMAKE_COMMAND}" "-DWFC=${WFC_EXE}"
        "-DINPUT=${wfc_corpus_dir}/${wfc_corpus_name}.vbp"
        "-DEXPECTED=${wfc_corpus_dir}/${wfc_corpus_name}.expected"
        ${wfc_corpus_debug} -P
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/run_corpus_case.cmake"
    )
    set_tests_properties(
      ${wfc_tp}TC-${wfc_corpus_id}-${wfc_corpus_slug}
      PROPERTIES
        LABELS "corpus;mscomctl2"
        TIMEOUT 120
        SKIP_REGULAR_EXPRESSION "wfc executable not set"
    )
  endforeach()
endif()

# --- WebAssembly demo ---------------------------------------------------------
# With Emscripten the member table also builds as a module and a page that lists
# the classes (web/index.html); CI opens the page in Chromium for the README
# screenshot.
if(EMSCRIPTEN)
  file(
    STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/TARGET"
    WFC_WEB_TARGET
    LIMIT_COUNT 1
  )
  add_executable(mscomctl2_web web/wasm_api.cpp)
  target_link_libraries(mscomctl2_web PRIVATE wfc_mscomctl2)
  target_compile_features(mscomctl2_web PRIVATE cxx_std_20)
  target_compile_definitions(
    mscomctl2_web
    PRIVATE
      WFC_WEB_VERSION="${WFC_MSCOMCTL2_VERSION}"
      WFC_WEB_TARGET="${WFC_WEB_TARGET}"
  )
  set_target_properties(
    mscomctl2_web
    PROPERTIES OUTPUT_NAME mscomctl2 SUFFIX .js
  )
  set(
    WFC_WEB_NAMES
    library_name
    library_version
    library_target
    class_count
    class_name
    member_count
    event_count
    member_name
    event_name
  )
  string(JOIN "','_" WFC_WEB_EXPORTS ${WFC_WEB_NAMES})
  target_link_options(
    mscomctl2_web
    PRIVATE
      -sMODULARIZE=1
      -sEXPORT_NAME=create_mscomctl2
      "-sEXPORTED_FUNCTIONS=['_${WFC_WEB_EXPORTS}']"
      "-sEXPORTED_RUNTIME_METHODS=['ccall','UTF8ToString']"
      -sENVIRONMENT=web,node
  )
  add_custom_command(
    TARGET mscomctl2_web
    POST_BUILD
    COMMAND
      "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/web/index.html"
      "$<TARGET_FILE_DIR:mscomctl2_web>/index.html"
  )
endif()
