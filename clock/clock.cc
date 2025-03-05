#include <types.h>
#include <gba.h>
#include <numbers.h>

static auto constexpr BLK = (0x00 << 0);
static auto constexpr RED = (0x1F << 0);

int div10(int x) { return (x * 0xCCCCCCCDLL) >> 35; }
int mod10(int x) { return x - ((x * 0xCCCCCCCDLL) >> 35) * 10; }

void draw_at(int x, int y, int v)
{
  {
    u16_t* N = ((u16_t*)&NUMBERS) + 3*5*v;
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 5; ++j) {
        *VRAM.at(x + i, y + j) = (N[3 * j + i] ? RED : BLK);
      }
    }
  }
}

int main()
{
  *REG_DISPCNT = /*BG MODE=*/(3 << 0) | /*SCREEN BG 2 ENABLE=*/(1 << 10);

  int frame_counter = 0;
  int minutes = 0;
  int seconds = 0;
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
      }
    } else if (Controller::is_pressed(BTN_B)) {
      seconds -= 1;
      if (seconds < 0) {
        minutes -= 1;
        seconds = 59;
      }
    }
  }

  return 0;
}

