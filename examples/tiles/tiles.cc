#include <types.h>
#include <gba.h>

#define MAP_BASE ((volatile u16_t*)0x06008000)  // Screen Block 31

#define CHARBLOCK_0 (0 << 2)
#define SCREENBLOCK_31 (0 << 8)
#define BG_SIZE_0 (0 << 14)

void setup_graphics() {
    *REG_DISPCNT = MODE_0 | BG0_ENABLE;
    *REG_BG0CNT = 
      (0b00    << 0)  | // Priority 0 (Highest)
      (0b01    << 2)  | // Start addr of tile data 0x06000000 + S * 0x4000
      (0b00    << 4)  | // Unused
      (0b0     << 6)  | // Mosaic effect 0 (Off)
      (0b0     << 7)  | // Use 16 color map
      (0b00000 << 8)  | // Starting addr of tile map 0x06000000 + M * 0x800
      (0b0     << 13) | // Screen over / RO
      (0b00    << 15)   // Size of tile map. 0 means 32x32
    ;
    static auto const TILEMAP_PTR = (volatile u16_t*)(0x06000000 + 0 * 0x800);
    static auto const TILESET_PTR = (volatile u16_t*)(0x06000000 + 1 * 0x4000);

    const u16_t tile_data[16] = {
        0x0001, 0x3300,
        0x0000, 0x3000,
        0x0000, 0x0000,
        0x0000, 0x0000,
        0x0000, 0x0000,
        0x0000, 0x0000,
        0x0002, 0x0000,
        0x0022, 0x1000
    };

    BG_PALETTE[0] = 0x0000;
    BG_PALETTE[1] = (0x1f << 0);
    BG_PALETTE[2] = (0x1f << 5);
    BG_PALETTE[3] = (0x1f << 10);
    for (int i = 0; i < 16; i++) {
        TILESET_PTR[16 + i] = tile_data[i];
    }

    TILEMAP_PTR[32 * 0 + 0] = 1;
    TILEMAP_PTR[32 * 0 + 2] = 1;
    TILEMAP_PTR[32 * 2 + 0] = 1;

    TILEMAP_PTR[32 * 0 + 29] = 1;
    TILEMAP_PTR[32 * 0 + 27] = 1;
    TILEMAP_PTR[32 * 2 + 29] = 1;

    TILEMAP_PTR[32 * 19 + 0] = 1;
    TILEMAP_PTR[32 * 19 + 2] = 1;
    TILEMAP_PTR[32 * 17 + 0] = 1;

    TILEMAP_PTR[32 * 19 + 29] = 1;
    TILEMAP_PTR[32 * 19 + 27] = 1;
    TILEMAP_PTR[32 * 17 + 29] = 1;
}

int main() {
    setup_graphics();
    while (1);
}

