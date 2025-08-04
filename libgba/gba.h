#ifndef _GBA_H
#define _GBA_H

// Memory data pointers
static auto const REG_DISPCNT  = (volatile u16_t*)0x04000000;
static auto const REG_VCOUNT   = (volatile u16_t*)0x04000006;
static auto const REG_BG0CNT   = (volatile u16_t*)0x04000008;
static auto const REG_BG1CNT   = (volatile u16_t*)0x0400000A;
static auto const REG_BG2CNT   = (volatile u16_t*)0x0400000C;
static auto const REG_BG3CNT   = (volatile u16_t*)0x0400000E;
static auto const REG_BG0HOFS  = (volatile u16_t*)0x04000010;
static auto const REG_BG0VOFS  = (volatile u16_t*)0x04000012;
static auto const REG_BG1HOFS  = (volatile u16_t*)0x04000014;
static auto const REG_BG1VOFS  = (volatile u16_t*)0x04000016;
static auto const REG_BG2HOFS  = (volatile u16_t*)0x04000018;
static auto const REG_BG2VOFS  = (volatile u16_t*)0x0400001A;
static auto const REG_BG3HOFS  = (volatile u16_t*)0x0400001C;
static auto const REG_BG3VOFS  = (volatile u16_t*)0x0400001E;
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
  u16_t attr0;
  u16_t attr1;
  u16_t attr2;

  // Size  Square   Horizontal  Vertical
  // 0     8x8      16x8        8x16
  // 1     16x16    32x8        8x32
  // 2     32x32    32x16       16x32
  // 3     64x64    64x32       32x64
  enum class ObjectSize {
    _8x8 = 0,
    _16x16 = 1,
    _32x32 = 2,
    _64x64 = 3
  };

  static constexpr u16_t step16x16(u16_t obj_id) { return 4 * obj_id; }
  static volatile OAM_attr* get_obj(u16_t obj_id) {
    return reinterpret_cast<volatile OAM_attr*>(OAM + obj_id * sizeof(OAM_attr));
  }
  inline void set_x(u16_t v) volatile {
    attr1 &= ~0x1ff;
    attr1 |= v & 0x1ff;
  }
  inline void set_y(u8_t v) volatile {
    attr0 &= ~0xff;
    attr0 |= v & 0xff;
  }
  inline void set_size(ObjectSize size) volatile {
    attr1 &= ~(0b11 << 14);
    attr1 |= (static_cast<u16_t>(size) << 14);
  }
  inline void set_sprite(u16_t sprite_id) volatile {
    attr2 &= ~0x1ff;
    attr2 |= (sprite_id & 0x1ff);
  }
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

