# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\tugas1_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\tugas1_autogen.dir\\ParseCache.txt"
  "tugas1_autogen"
  )
endif()
