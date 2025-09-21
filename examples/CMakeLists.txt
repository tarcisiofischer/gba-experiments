set(CMAKE_CXX_STANDARD 20)

gba_exec(
  NAME clock
  SOURCES
  clock/clock.cc
)
target_compile_options(
  clock PRIVATE -I${CMAKE_SOURCE_DIR}/examples/clock/
)

gba_exec(
  NAME snake
  SOURCES
  snake/snake.cc
)

gba_exec(
  NAME tiles
  SOURCES
  tiles/tiles.cc
)

gba_exec(
  NAME hello-world
  SOURCES
  hello-world/hello-world.cc
)
target_link_libraries(
  hello-world
  libgba-base
)

gba_exec(
  NAME sprites
  SOURCES
  sprites/sprites.cc
)

gba_exec(
  NAME spritesheet
  SOURCES
  spritesheet/main.cc
  SPRITESHEETS
  spritesheet/chicken.png
  spritesheet/bg.png
  TILEMAP
  spritesheet/foo.json
)

gba_exec(
  NAME sound
  SOURCES
  sound/main.cc
  SPRITESHEETS
  sound/sound.png
)

gba_exec(
  NAME space-shooter
  SOURCES
  space-shooter/main.cc
  SPRITESHEETS
  space-shooter/objects.png
  space-shooter/bg.png
)

