#include <types.h>

#define REG_DISPCNT *(volatile u16_t*)0x04000000
#define REG_BG0CNT *(volatile u16_t*)0x04000008

#define MODE_0 0x0000
#define BG0_ENABLE 0x0100

#define TILE_BASE ((volatile u16_t*)0x06000000) // Character Block 0
#define MAP_BASE ((volatile u16_t*)0x06008000)  // Screen Block 31
#define BG_PALETTE ((volatile u16_t*)0x05000000)

#define CHARBLOCK_0 (0 << 2)
#define SCREENBLOCK_31 (31 << 8)
#define BG_SIZE_0 (0 << 14)

void setup_graphics() {
    // Set Mode 0 and enable BG0
    REG_DISPCNT = MODE_0 | BG0_ENABLE;

    // Define a simple tile (8x8 pixels)
    const u16_t tile_data[8] = {
        0x1001, 0x1111,
        0x0000, 0x1001,
        0x1001, 0x1001,
        0x1111, 0x1111
    };

    BG_PALETTE[1] = 0x7C00;  // Set color index 1 to red
    // Copy tile data to VRAM
    for (int i = 0; i < 8; i++) {
        TILE_BASE[i + 1] = tile_data[i];
    }

    // Place tile 0 at (0,0) in the map
    MAP_BASE[0] = 0;

    // Configure BG0 to use Character Block 0 and Screen Block 31
    REG_BG0CNT = CHARBLOCK_0 | SCREENBLOCK_31 | BG_SIZE_0;
}

int main() {
    setup_graphics();
    while (1);
}

