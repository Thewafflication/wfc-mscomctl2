# Runs one corpus program through wfc and compares its output with the stored
# files: standard output with EXPECTED and, when DEBUG_EXPECTED is given, the
# Immediate-window output (Debug.Print and load warnings, which wfc writes to
# standard error) with that file. The interpreter runs the program.
# Usage: cmake -DWFC=<wfc> -DINPUT=<.vbp> -DEXPECTED=<file>
#   [-DDEBUG_EXPECTED=<file>] -P run_corpus_case.cmake
# The same comparison as tests/corpus/run_corpus_case.cmake of WFC.
if(NOT WFC OR NOT EXISTS "${WFC}")
  message(STATUS "wfc executable not set (WFC_EXE): skipped")
  return()
endif()
execute_process(
  COMMAND "${WFC}" --interpret "${INPUT}"
  OUTPUT_VARIABLE actual
  ERROR_VARIABLE errors
  RESULT_VARIABLE result
)
if(NOT result EQUAL 0)
  message(FATAL_ERROR "wfc exited with ${result}: ${errors}")
endif()

file(READ "${EXPECTED}" expected)
if(DEFINED DEBUG_EXPECTED)
  file(READ "${DEBUG_EXPECTED}" debug_expected)
else()
  set(debug_expected "")
endif()
# Line endings and surrounding blanks do not count.
foreach(name actual errors expected debug_expected)
  string(REPLACE "\r\n" "\n" ${name} "${${name}}")
  string(STRIP "${${name}}" ${name})
endforeach()
if(DEFINED DEBUG_EXPECTED AND NOT errors STREQUAL debug_expected)
  message(
    FATAL_ERROR
    "debug output mismatch\n--- expected ---\n${debug_expected}\n"
    "--- actual ---\n${errors}"
  )
endif()
if(NOT actual STREQUAL expected)
  message(
    FATAL_ERROR
    "corpus output mismatch\n--- expected ---\n${expected}\n"
    "--- actual ---\n${actual}"
  )
endif()
