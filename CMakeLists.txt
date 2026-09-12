set(COMMON_LIB common_lib)

set(SRCS
  mem.cpp
  util/mutex.cpp
  util/print.cpp
)

add_library(${COMMON_LIB} ${SRCS})
target_include_directories(${COMMON_LIB} PRIVATE ${CMAKE_SOURCE_DIR})
target_compile_options(${COMMON_LIB} PRIVATE ${CPP_FLAGS})
