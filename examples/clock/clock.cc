#include <types.h>
#include <gba.h>
#include <numbers.h>

static i16_t constexpr BLK = (0x00 << 0);
static i16_t constexpr RED = (0x1F << 0);

u16_t div10(u16_t x) { return x / 10; }
u16_t mod10(u16_t x) { return x - ((x * 0xCCCCCCCDLL) >> 35) * 10; }

void draw_at(i16_t x, i16_t y, i16_t v)
{
  {
    i16_t* N = ((i16_t*)&NUMBERS) + 3*5*v;
    for (i16_t i = 0; i < 3; ++i) {
      for (i16_t j = 0; j < 5; ++j) {
        *VRAM.at(x + i, y + j) = (N[3 * j + i] ? RED : BLK);
      }
    }
  }
}

int main()
{
  *REG_DISPCNT = /*BG MODE=*/(3 << 0) | /*SCREEN BG 2 ENABLE=*/(1 << 10);

  i16_t frame_counter = 0;
  i16_t minutes = 0;
  i16_t seconds = 0;
  while(1) {
    vid_vsync();

    frame_counter += 1;
    if (frame_counter == 60) {
      frame_counter = 0;
      seconds += 1;
      if (seconds == 60) {
        minutes += 1;
        seconds = 0;
      }
    }

    draw_at(100, 80, div10(minutes));
    draw_at(105, 80, mod10(minutes));
    draw_at(115, 80, div10(seconds));
    draw_at(120, 80, mod10(seconds));

    if (Controller::is_pressed(BTN_A)) {
      seconds += 1;
      if (seconds > 59) {
        minutes += 1;
        seconds = 0;
        if (minutes >= 100) {
          minutes = 0;
        }
      }
    } else if (Controller::is_pressed(BTN_B)) {
      seconds -= 1;
      if (seconds < 0) {
        if (minutes > 0) {
          minutes -= 1;
          seconds = 59;
        } else {
          seconds = 0;
        }
      }
    }
  }

  return 0;
}

