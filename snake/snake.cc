#include <types.h>
#include <gba.h>

static auto constexpr BLK = (0x00 << 0);
static auto constexpr RED = (0x1F << 0);
static auto constexpr BLUE = (0x1F << 10);
static auto constexpr MAX_SNAKE_SIZE = 100;

enum class Dir { UP, DOWN, LEFT, RIGHT };
struct Pos { int x = 0; int y = 0; };
struct SnakePiece {
  Pos cur_pos;
  Pos old_pos;
  SnakePiece* prev = nullptr;

  void set_pos(Pos new_pos) {
    old_pos = cur_pos;
    cur_pos = new_pos;
    if (prev) {
      prev->set_pos(old_pos);
    }
  }

  void do_update(Dir const& direction)
  {
    old_pos = cur_pos;
    switch (direction) {
      case Dir::UP:
        cur_pos.y -= 1;
        break;
      case Dir::DOWN:
        cur_pos.y += 1;
        break;
      case Dir::LEFT:
        cur_pos.x -= 1;
        break;
      case Dir::RIGHT:
        cur_pos.x += 1;
        break;
    }
    if (cur_pos.x < 20) cur_pos.x = 220;
    if (cur_pos.x > 220) cur_pos.x = 20;
    if (cur_pos.y < 20) cur_pos.y = 140;
    if (cur_pos.y > 140) cur_pos.y = 20;
    if (prev) {
      prev->set_pos(old_pos);
    }
  }

  void draw()
  {
    *VRAM.at(old_pos.x, old_pos.y) = BLK;
    *VRAM.at(cur_pos.x, cur_pos.y) = RED;
    if (prev) {
      prev->draw();
    }
  }
};

struct Snake {
  SnakePiece head;
  Dir direction;
  int time;

  bool update(int elapsed_time)
  {
    time += elapsed_time;
    if (time < 50) {
      return false;
    }
    time = 0;
    head.do_update(direction);
    return true;
  }

  void draw()
  {
    head.draw();
  }

  void set_direction(Dir const& new_direction)
  {
    if (
         (new_direction == Dir::UP    && direction != Dir::DOWN)
      || (new_direction == Dir::DOWN  && direction != Dir::UP)
      || (new_direction == Dir::RIGHT && direction != Dir::LEFT)
      || (new_direction == Dir::LEFT  && direction != Dir::RIGHT)
    ) {
      direction = new_direction;
    }
  }
};

// Guarantee random values ;D
static u32_t prand_c = 42;
static u32_t prand_l[] = {
   43, 118, 100,  84, 122,  42,  76, 38,  44, 89,
  117,  48, 133,  44, 129, 111,  51, 80, 133, 58,
  101,  40,  80,  87,  37, 124,  62, 36,  50, 34,
   48,  51, 109, 117, 101,  65, 103, 71, 119, 63,
  105, 130,  20, 125, 109,  98, 126, 95, 100, 62,
   45,  70, 120,  55,  49,  99,  66, 45,  40, 52,
   68,  55,  67,  55
};
u32_t prand() {
  return prand_l[++prand_c & 0x3f];
}

struct Fruit {
  Pos pos;

  void random_pos()
  {
    pos.x = prand();
    pos.y = prand();
  }

  void draw()
  {
    *VRAM.at(pos.x, pos.y) = BLUE;
  }
};

bool check_collision(Snake const& s, Fruit const& f)
{
  return s.head.cur_pos.x == f.pos.x && s.head.cur_pos.y == f.pos.y;
}

static SnakePiece pieces[MAX_SNAKE_SIZE];
int main()
{
  *REG_DISPCNT = /*BG MODE=*/(3 << 0) | /*SCREEN BG 2 ENABLE=*/(1 << 10);

  Snake s;
  s.head.cur_pos.x = 100;
  s.head.cur_pos.y = 80;
  s.direction = Dir::RIGHT;
  s.head.prev = &pieces[0];
  SnakePiece* tail = s.head.prev;
  int n_pieces = 1;

  Fruit f;
  f.random_pos();
  while(1) {
    vid_vsync();
    if(s.update(17)) {
      if (check_collision(s, f)) {
        f.random_pos();
        if (n_pieces < MAX_SNAKE_SIZE) {
          tail->prev = &pieces[n_pieces];
          tail = tail->prev;
          n_pieces++;
        }
      }
      s.draw();
      f.draw();
    }

    if (Controller::is_pressed(BTN_UP)) {
      s.set_direction(Dir::UP);
    } else if (Controller::is_pressed(BTN_DOWN)) {
      s.set_direction(Dir::DOWN);
    } else if (Controller::is_pressed(BTN_LEFT)) {
      s.set_direction(Dir::LEFT);
    } else if (Controller::is_pressed(BTN_RIGHT)) {
      s.set_direction(Dir::RIGHT);
    }
  }

  return 0;
}

