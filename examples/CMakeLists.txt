set(CMAKE_CXX_STANDARD 20)

function(gba_add_example)
  set(oneValueArgs NAME)
  set(multiValueArgs SOURCES)
  cmake_parse_arguments(arg "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  add_executable(
    ${arg_NAME}
    ${arg_SOURCES}
  )
  target_compile_options(
    ${arg_NAME} PRIVATE
    -mthumb
    -mfpu=none
    -fno-rtti
    -nostdlib
    -I${CMAKE_SOURCE_DIR}/examples/${arg_NAME}/
  )
  target_link_options(
    ${arg_NAME} PRIVATE
    -nostdlib
    -L${CMAKE_BINARY_DIR}/libgba
    -lcrt0-gba
    -Wl,-T,${CMAKE_SOURCE_DIR}/libgba/gba_cart.ld
    -L$ENV{GBA_LLVM}/lib/clang/21/lib/armv4t-none-unknown-eabi/
    -lclang_rt.builtins
  )
  set(RESOLVED_TARGET_FILE $<TARGET_FILE:${arg_NAME}>)
  add_custom_command(
    TARGET ${arg_NAME}
    POST_BUILD
    COMMAND $(GBA_LLVM)/bin/llvm-objcopy -O binary ${RESOLVED_TARGET_FILE} ${arg_NAME}.gba
    COMMENT "Building GBA ROM for ${arg_NAME}..."
  )
endfunction()

gba_add_example(
  NAME clock
  SOURCES
  clock/clock.cc
)

gba_add_example(
  NAME snake
  SOURCES
  snake/snake.cc
)

gba_add_example(
  NAME tiles
  SOURCES
  tiles/tiles.cc
)
