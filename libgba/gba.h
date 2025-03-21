#ifndef _GBA_H
#define _GBA_H

// Memory data pointers
static auto const REG_DISPCNT  = (volatile u16_t*)0x04000000;
static auto const REG_VCOUNT   = (volatile u16_t*)0x04000006;
static auto const REG_BG0CNT   = (volatile u16_t*)0x04000008;
static auto const REG_KEYINPUT = (volatile u16_t*)0x04000130;
static auto const BG_PALETTE   = (volatile u16_t*)0x05000000;
static auto const OBJ_PALETTE  = (volatile u16_t*)0x05000200;
static auto const PALETTE_END  = (volatile u16_t*)0x05000400;
// Note: VRAM can have different semantics depending on the configurations.
static auto const VRAM_PTR     = (volatile u16_t*)0x06000000;
static auto const OBJ_TILES    = (volatile u16_t*)0x06010000;
static auto const OAM          = (volatile u16_t*)0x07000000;

// Video modes
static auto constexpr MODE_0     = (u16_t)(0 << 0);
static auto constexpr BG0_ENABLE = (u16_t)(1 << 8);
static auto constexpr BG1_ENABLE = (u16_t)(1 << 9);
static auto constexpr BG2_ENABLE = (u16_t)(1 << 10);
static auto constexpr BG3_ENABLE = (u16_t)(1 << 11);
static auto constexpr OBJ_ENABLE = (u16_t)(1 << 12);

// Controller values
static auto constexpr BTN_A      = (1 << 0);
static auto constexpr BTN_B      = (1 << 1);
static auto constexpr BTN_SELECT = (1 << 2);
static auto constexpr BTN_START  = (1 << 3);
static auto constexpr BTN_RIGHT  = (1 << 4);
static auto constexpr BTN_LEFT   = (1 << 5);
static auto constexpr BTN_UP     = (1 << 6);
static auto constexpr BTN_DOWN   = (1 << 7);
static auto constexpr BTN_R      = (1 << 8);
static auto constexpr BTN_L      = (1 << 9);

struct OAM_attr {
  char attr0;
  char attr1;
  char attr2;

  void x(u16_t v) { attr1 |= ((v & 0x1ff) << 0);  }
  void y(u8_t v) { attr0 |= ((v & 0xff) << 0);  }
};

struct _VRAM_t {
  inline volatile u16_t* at(int x, int y) { return &(VRAM_PTR[x + y*240]); }
};
static _VRAM_t VRAM;

struct Controller {
  static inline bool is_pressed(u16_t btn) { return ~(*REG_KEYINPUT) & btn; }
};

void vid_vsync()
{
  while(*REG_VCOUNT >= 160);
  while(*REG_VCOUNT < 160);
}

#endif

