#include <gba.h>
#include <random.h>

void setup_display() {
  /**
   * Mode 0 is a tile/map-based mode with no rotation/scaling and supports up
   * to 4 layers.
   *
   * 1D object mapping means each 8x8 sprite piece is put side-by-side in the
   * spritesheet. There are 1024 8x8 sprite pieces per spritesheet.
   */
  *REG_DISPCNT =
    (0b000 << 0)  | // Video Mode 0
    (0b0   << 3)  | // GBA
    (0b0   << 4)  | // Unused in Mode 0
    (0b0   << 5)  | // No access to OAM in H-Blank
    (0b1   << 6)  | // 1D Object Mapping
    (0b0   << 7)  | // No fast access to VRAM,Palette,OAM
    (0b1   << 8)  | // BG 0 enable flag
    (0b0   << 9)  | // BG 1 enable flag
    (0b0   << 10) | // BG 2 enable flag
    (0b0   << 11) | // BG 3 enable flag
    (0b1   << 12) | // OBJ enable flag
    (0b0   << 13) | // Window 0 display flag
    (0b0   << 14) | // Window 1 display flag
    (0b0   << 15) // OBJ window display flag
  ;

  /**
   * Background 0 configuration.
   *
   * Single background, 16 colors color map, 32x32 tiles, tiles are 8x8 pixels each.
   */
  *REG_BG0CNT =
    (0b00    << 0)  | // Priority 0 (Highest)
    (0b01    << 2)  | // Start addr of tile data 0x06000000 + S * 0x4000
    (0b00    << 4)  | // Unused
    (0b0     << 6)  | // Mosaic effect 0 (Off)
    (0b0     << 7)  | // Use 16 color map
    (0b00000 << 8)  | // Starting addr of tile map 0x06000000 + M * 0x800
    (0b0     << 13) | // Screen over / RO
    (0b00    << 14)   // Size of tile map entries. 0 means 256x256 pixels (32x32 tiles)
  ;
}

void setup_spritesheet() {
  #include <spritesheet_objects.inc>
  for (u16_t i = 0; i < TILESET_SIZE; ++i) {
    OBJ_TILES[i] = TILESET[i];
  }
  for (u16_t i = 0; i < PALETTE_SIZE; ++i) {
    OBJ_PALETTE1[i] = PALETTE[i];
  }
}

void setup_bg_tileset() {
  #include <spritesheet_bg.inc>
  static auto const TILESET_PTR = (volatile u16_t*)(0x06000000 + 1 * 0x4000);
  for (u16_t i = 0; i < PALETTE_SIZE; ++i) {
    BG_PALETTE[i] = PALETTE[i];
  }
  for (u16_t i = 0; i < TILESET_SIZE; i++) {
    TILESET_PTR[i] = TILESET[i];
  }
}

void setup_tilemaps() {
  static auto const TILEMAP_BG0_PTR = (volatile u16_t*)(0x06000000 + 0 * 0x800);
  for (u16_t i = 0; i < 32*32; ++i) {
      TILEMAP_BG0_PTR[i] = i;
  }
}

class Bullet {
public:
  Bullet(u16_t obj_id)
    : _obj(OAM_attr::get_obj(obj_id))
  {
    this->_obj->set_sprite(OAM_attr::step16x16(5));
    this->_obj->set_size(OAM_attr::ObjectSize::_16x16);
    this->set_visible(false);
  }

  bool is_visible() { return this->_is_visible; }

  void set_visible(bool v) {
    this->_is_visible = v;
    if (v) this->_obj->set_sprite(OAM_attr::step16x16(5));
    else this->_obj->set_sprite(OAM_attr::step16x16(0));
  }

  void set_position(u16_t x, u16_t y) {
    this->_x = x;
    this->_y = y;
  }

  void set_active(bool v) {
    this->set_visible(v);
    this->_is_active = v;
  }
  bool is_active() {
    return this->_is_active;
  }

  u16_t x() { return this->_x; }
  u16_t y() { return this->_y; }

  void update() {
    if (!this->_is_active) {
      return;
    }

    this->_y -= this->_velocity;
    if (this->_y < -8) {
      this->set_active(false);
    }
    this->_obj->set_x(this->_x);
    this->_obj->set_y(this->_y);
  }

private:
  volatile OAM_attr* _obj = nullptr;
  i16_t _x = 0;
  i16_t _y = 0;
  i16_t _velocity = 2;
  bool _is_visible = false;
  bool _is_active = false;
};

class Player {
public:
  enum class State {
    ACTIVE = 0,
    DYING = 1,
    DEAD = 2
  };

  Player(Sound& m_shoot)
    : _obj(OAM_attr::get_obj(0))
    , _x(110)
    , _y(120)
    , _bullets{Bullet(1), Bullet(2), Bullet(3)}
    , _m_shoot(m_shoot)
  {
    _obj->set_sprite(OAM_attr::step16x16(1));
    _obj->set_x(_x);
    _obj->set_y(_y);
    _obj->set_size(OAM_attr::ObjectSize::_16x16);
  }

  u16_t x() { return this->_x; }
  u16_t y() { return this->_y; }

  void set_state(Player::State s) { this->_state = s; }
  Player::State state() { return this->_state; }

  void move_right() { this->_x++; if (this->_x > 204) { this->_x = 204; } }
  void move_left() { this->_x--; if (this->_x < 8) { this->_x = 8; } }
  void move_up() { this->_y--; if (this->_y < 50) { this->_y = 50; } }
  void move_down() { this->_y++; if (this->_y > 120) { this->_y = 120; } }
  void shoot() {
    if (this->_bullet_cooldown) {
      return;
    }

    for (u16_t i = 0; i < 3; ++i) {
      if (this->_bullets[i].is_active()) {
        // Already in use
        continue;
      }
      this->_bullets[i].set_position(this->_x, this->_y - 6);
      this->_bullets[i].set_active(true);
      this->_m_shoot.play();
      this->_bullet_cooldown = Player::BULLET_COOLDOWN;
      break;
    }
  }

  void update() {
    for (u16_t i = 0; i < 3; ++i) {
      this->_bullets[i].update();
    }
    if (this->_bullet_cooldown) {
      this->_bullet_cooldown--;
    }

    if (this->_state == Player::State::ACTIVE) {
      this->_obj->set_x(this->_x);
      this->_obj->set_y(this->_y);
    } else if (this->_state == Player::State::DYING) {
      if (!this->_tpf) {
        this->_dying_step++;
        if (this->_dying_step >= 10) {
          this->_state = Player::State::DEAD;
          this->_tpf = 0;
          this->_dying_step = 5;
          return;
        }
        this->_obj->set_sprite(OAM_attr::step16x16(this->_dying_step));
        this->_tpf = 4;
      }
      this->_tpf--;
    } else if (this->_state == Player::State::DEAD) {
      this->_x = 110;
      this->_y = 120;
      this->_obj->set_x(this->_x);
      this->_obj->set_y(this->_y);
      this->_obj->set_sprite(OAM_attr::step16x16(1));
      this->_state = Player::State::ACTIVE;
    }
  }

// private:
public:
  volatile OAM_attr* _obj;
  u16_t _x;
  u16_t _y;
  Bullet _bullets[3];
  static const u16_t BULLET_COOLDOWN = 24;
  u16_t _bullet_cooldown = Player::BULLET_COOLDOWN;
  Sound& _m_shoot;
  Player::State _state = Player::State::ACTIVE;
  i16_t _tpf = 0;
  i16_t _dying_step = 5;
};

static u16_t seed = 0x1abf;
u16_t my_rand() { seed = rand(seed); return seed; }

class Enemy {
public:
  enum class State {
    ACTIVE = 0,
    DYING = 1,
    DEAD = 2
  };

  Enemy(i16_t x, i16_t y, u16_t obj_id)
    : _obj(OAM_attr::get_obj(obj_id))
    , _x(x)
    , _y(y)
  {
    this->_obj->set_sprite(OAM_attr::step16x16(2));
    this->_obj->set_x(_x);
    this->_obj->set_y(_y);
    this->_obj->set_size(OAM_attr::ObjectSize::_16x16);
  }

  u16_t x() { return this->_x; }
  u16_t y() { return this->_y; }

  void set_visible(bool v) {
    this->_is_visible = v;
    if (v) this->_obj->set_sprite(OAM_attr::step16x16(2));
    else this->_obj->set_sprite(OAM_attr::step16x16(0));
  }

  void set_state(Enemy::State s) { this->_state = s; }
  Enemy::State state() { return this->_state; }

  void update() {
    if (this->_state == Enemy::State::ACTIVE) {
      this->_y += this->_velocity;
      this->_obj->set_x(this->_x);
      this->_obj->set_y(this->_y);

      if (this->_y > 160) {
        this->_state = Enemy::State::DEAD;
      }
    } else if (this->_state == Enemy::State::DYING) {
      if (!this->_tpf) {
        this->_dying_step++;
        if (this->_dying_step >= 10) {
          this->_state = Enemy::State::DEAD;
          this->_tpf = 0;
          this->_dying_step = 5;
          return;
        }
        this->_obj->set_sprite(OAM_attr::step16x16(this->_dying_step));
        this->_tpf = 4;
      }
      this->_tpf--;
    } else if (this->_state == Enemy::State::DEAD) {
      this->_y = -8;
      this->_x = my_rand() & 0xff;
      this->_velocity = 1 + (my_rand() & 0x1);
      this->_obj->set_x(this->_x);
      this->_obj->set_y(this->_y);
      this->_obj->set_sprite(OAM_attr::step16x16(2));
      this->_state = Enemy::State::ACTIVE;
    }
  }

private:
  volatile OAM_attr* _obj;
  i16_t _x = 0;
  i16_t _y = 0;
  i16_t _velocity = 1;
  bool _is_visible = true;
  Enemy::State _state = Enemy::State::ACTIVE;
  i16_t _tpf = 0;
  i16_t _dying_step = 5;
};

int main() {
  setup_display();
  setup_spritesheet();
  setup_bg_tileset();
  setup_tilemaps();
  Sound::init();

  u16_t song1[] = {
    D2, D2, C2, E2, D2, _, D2, D2, E2, D2, A2, _,
    D2, D2, D2, C2, E2, D2, C2, E2, C2, D2, D2, _,
    D2, D2, C2, E2, D2, _, D2, D2, E2, D2, A2, _,
    D3, D3, D3, C3, E3, D3, C3, E3, C3, D3, D3, _,
    D3, C3, D3, C3, D3, C3, D2, D2, D2, E2, D2, A2, _
  };
  Sound m1 = Sound(song1, sizeof(song1) / sizeof(u16_t));
  m1.config_ch1_volume(4);
  u16_t shoot_noise[] = {D4, E4};
  Sound m_shoot = Sound(shoot_noise, sizeof(shoot_noise) / sizeof(u16_t));
  m_shoot.config_ch1_type(Sound::TypeD);
  m_shoot.set_stop_on_finish(true);
  m_shoot.stop();

  auto controller = Controller();
  auto player = Player(m_shoot);
  Enemy enemies[] = {
    Enemy(10, -8, 10),
    Enemy(10, -8, 11),
    Enemy(60, -32, 12),
    Enemy(60, -32, 13),
    Enemy(120, -30, 14)
  };

  u16_t offset = 0;
  u16_t offset_tpf = 2;
  while (1) {
    vid_vsync();
    controller.update();

    offset_tpf--;
    if (!offset_tpf) {
      offset -= 1;
      *REG_BG0VOFS = offset;
      offset_tpf = 2;
    }

    if (controller.is_pressed(BTN_RIGHT)) { player.move_right(); }
    if (controller.is_pressed(BTN_LEFT)) { player.move_left(); }
    if (controller.is_pressed(BTN_UP)) { player.move_up(); }
    if (controller.is_pressed(BTN_DOWN)) { player.move_down(); }
    if (controller.is_pressed(BTN_A)) { player.shoot(); }
    player.update();
    for (auto& enemy : enemies) {
      enemy.update();
    }
    m1.update();
    m_shoot.update();

    for (u16_t i = 0; i < 3; ++i) {
      auto& bullet = player._bullets[i];
      if (!bullet.is_active()) {
        continue;
      }

      for (auto& enemy : enemies) {
        if (enemy.state() != Enemy::State::ACTIVE) {
          continue;
        }
        if (
          enemy.x()       < bullet.x() + 8 &&
          enemy.x() + 8   > bullet.x()     &&
          enemy.y()       < bullet.y() + 8 &&
          enemy.y() + 8   > bullet.y()
        ) {
          enemy.set_state(Enemy::State::DYING);
          bullet.set_active(false);
        }
      }
    }
    for (auto& enemy : enemies) {
      if (enemy.state() != Enemy::State::ACTIVE) {
        continue;
      }
      if (
        enemy.x()       < player.x() + 12 &&
        enemy.x() + 12  > player.x()     &&
        enemy.y()       < player.y() + 12 &&
        enemy.y() + 12  > player.y()
      ) {
        enemy.set_state(Enemy::State::DYING);
        player.set_state(Player::State::DYING);
      }
    }
  }
}

