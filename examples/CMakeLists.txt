set(CMAKE_CXX_STANDARD 20)

function(gba_add_example)
  set(oneValueArgs NAME)
  set(multiValueArgs SOURCES SPRITESHEETS)
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
    -I${CMAKE_SOURCE_DIR}/libgba/
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
  set(SS_OUTPUT_DIR "${CMAKE_BINARY_DIR}/${arg_NAME}_spritesheets/")
  add_custom_command(
    OUTPUT "${SS_OUTPUT_DIR}"
    COMMAND mkdir -p "${SS_OUTPUT_DIR}"
    COMMENT "Generating spritesheet output dir ${SS_OUTPUT_DIR}"
    VERBATIM
  )
  add_custom_target(${arg_NAME}_spritesheet_outdir DEPENDS "${SS_OUTPUT_DIR}")
  add_dependencies(${arg_NAME} ${arg_NAME}_spritesheet_outdir)
  foreach (SS ${arg_SPRITESHEETS})
    get_filename_component(SS_TARGET "${SS}" NAME_WE)
    set(SS_TARGET "spritesheet_${SS_TARGET}")
    set(SS_OUTPUT "${SS_OUTPUT_DIR}/${SS_TARGET}.inc")
    add_custom_command(
      OUTPUT "${SS_OUTPUT}"
      COMMAND img2bytes "${CMAKE_CURRENT_SOURCE_DIR}/${SS}" "${SS_OUTPUT}"
      DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${SS}"
      COMMENT "Generating spritesheet ${SS_OUTPUT}"
      VERBATIM
    )
    add_custom_target(${SS_TARGET} DEPENDS "${SS_OUTPUT}")
    add_dependencies(${arg_NAME} ${SS_TARGET})
    add_dependencies(${SS_TARGET} ${arg_NAME}_spritesheet_outdir)
  endforeach()
  target_include_directories(${arg_NAME} PRIVATE ${SS_OUTPUT_DIR})
  add_custom_command(
    TARGET ${arg_NAME}
    POST_BUILD
    COMMAND $(GBA_LLVM)/bin/llvm-objcopy -O binary ${RESOLVED_TARGET_FILE} ${arg_NAME}.gba
    COMMENT "Building GBA ROM for ${arg_NAME}..."
    VERBATIM
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

gba_add_example(
  NAME hello-world
  SOURCES
  hello-world/hello-world.cc
)

gba_add_example(
  NAME sprites
  SOURCES
  sprites/sprites.cc
)

gba_add_example(
  NAME spritesheet
  SOURCES
  spritesheet/main.cc
  SPRITESHEETS
  spritesheet/chicken.png
  spritesheet/bg.png
)

