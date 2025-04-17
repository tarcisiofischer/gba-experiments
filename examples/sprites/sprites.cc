#include <types.h>
#include <gba.h>
#include <random.h>

static auto const TILEMAP_PTR = (volatile u16_t*)(0x06000000 + 1 * 0x800);
static auto const TILESET_PTR = (volatile u16_t*)(0x06000000 + 0 * 0x400);

void update_sprites();

void setup_graphics() {
  *REG_DISPCNT = 
    MODE_0 | BG0_ENABLE | OBJ_ENABLE |
    (0b1 << 6) // 1D object mapping (default is 0=2d)
  ;
  *REG_BG0CNT = 
    (0b00    << 0)  | // Priority 0 (Highest)
    (0b00    << 2)  | // Start addr of tile data 0x06000000 + S * 0x4000
    (0b00    << 4)  | // Unused
    (0b0     << 6)  | // Mosaic effect 0 (Off)
    (0b0     << 7)  | // Use 16 color map
    (0b00001 << 8)  | // Starting addr of tile map 0x06000000 + M * 0x800
    (0b0     << 13) | // Screen over / RO
    (0b00    << 15)   // Size of tile map entries. 0 means 32x32
  ;
  BG_PALETTE[0] = 0x0000;
  BG_PALETTE[1] = (0x1c << 0) | (0x12 << 5) | (0x07 << 10);

  #include "chicken.inc"

  for (int i = 0; i < PALETTE_SIZE; ++i) {
    OBJ_PALETTE[i] = PALETTE[i];
  }

  for (u16_t i = 0; i < 64; ++i) {
    *(TILESET_PTR + 16 + i) = 0x1111;
  }
  for (u16_t i = 0; i < 4; ++i) {
    for (u16_t j = 0; j < 16; ++j) {
      *(OBJ_TILES + 16 * (i + 1) + j) = TILESET[16 * i + j];
    }
  }

  for (u16_t i = 0; i < 32*32; ++i) {
    TILEMAP_PTR[i] = 1;
  }
  update_sprites();
}

static u16_t seed = 0x1abf;
u16_t my_rand() { seed = rand(seed); return seed; }

void update_sprites()
{
  for (u16_t i = 0; i < 64; ++i) {
    u16_t x = my_rand() & 0xff;
    u16_t y = my_rand() & 0xff;

    *(OAM + 4*i + 0) = y; // y
    *(OAM + 4*i + 1) = 
      (x << 0) |   // x
      (0b01 << 14) // OBJ size = 1(16x16)
    ;
    *(OAM + 4*i + 2) = 1; // tile
  }
}

int main() {
  setup_graphics();
  u16_t frame_counter = 0;
  while (1) {
    vid_vsync();

    frame_counter += 1;
    if (frame_counter != 4) {
      continue;
    }
    frame_counter = 0;

    update_sprites();
  }
}

