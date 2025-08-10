#include <gba.h>

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
}

void setup_spritesheet() {
  #include <spritesheet_sound.inc>
  for (u16_t i = 0; i < TILESET_SIZE; ++i) {
    OBJ_TILES[i] = TILESET[i];
  }

  for (u16_t i = 0; i < PALETTE_SIZE; ++i) {
    OBJ_PALETTE1[i] = PALETTE[i];
  }
  for (u16_t i = 0; i < PALETTE_SIZE; ++i) {
    OBJ_PALETTE2[i] = 0b0'11000'11000'11000;
  }
}

int main() {
  bool is_pressed_right = false;
  bool is_pressed_left = false;
  bool is_pressed_up = false;
  bool is_pressed_down = false;

  u16_t song1[] = {
    C2, _, D2, _, E2, _, F2, _, G2, _, A2, _, B2, _,
    C3, _, D3, _, E3, _, F3, _, G3, _, A3, _, B3, _,
    C4, _, D4, _, E4, _, F4, _, G4, _, A4, _, B4, _,
    C5, _, D5, _, E5, _, F5, _, G5, _, A5, _, B5, _
  };
  Sound m1 = Sound(song1, sizeof(song1) / sizeof(u16_t));

  u16_t song2[] = {
    E5, Eb5, E5, Eb5, E5, B4, D5, C5, A4, _,
    C4, E4, A4, B4, _,
    E4, Gs4, B4, C5, _,
    E4, Eb5, E5, Eb5, E5, B4, D5, C5, A4, _
  };
  Sound m2 = Sound(song2, sizeof(song2) / sizeof(u16_t), 10);

  u16_t song3_ch1[] = {
    E4, E4, F4, G4,
    G4, F4, E4, D4,
    C4, C4, D4, E4,
    E4, D4, D4, _,
    E4, E4, F4, G4,
    G4, F4, E4, D4,
    C4, C4, D4, E4,
    D4, C4, C4
  };
  u16_t song3_ch2[] = {
    C3, C3, C3, C3,
    C3, C3, C3, C3,
    C3, C3, C3, C3,
    C3, C3, C3, C3,
    B2, B2, B2, B2,
    B2, B2, B2, B2,
    B2, B2, B2, B2,
    B2, B2, B2
  };
  Sound m3 = Sound(song3_ch1, song3_ch2, sizeof(song3_ch1) / sizeof(u16_t), 16);

  u16_t song4_ch1[] = {
    C2, C2, G2, G2, A2, A2, F2, F2,
    C3, C3, G2, G2, B2, B2, G2, G2,
    A2, A2, E2, E2, F2, F2, C2, C2,
    G2, G2, D3, D3, E2, E2, C2, C2,

    C2, G2, A2, F2, C3, G2, B2, G2,
    A2, E2, F2, C2, G2, D3, E2, C2,
    C2, G2, A2, F2, C3, G2, B2, G2,
    A2, E2, F2, C2, G2, D3, E2, C2
  };
  u16_t song4_ch2[] = {
    E4, G4, A4, G4, F4, D4, E4, C4,
    G4, A4, B4, A4, G4, E4, D4, C4,
    C4, D4, E4, G4, A4, G4, F4, E4,
    G4, F4, E4, D4, C4, B3, C4, D4,

    E4, E4, G4, F4, E4, D4, C4, D4,
    E4, G4, A4, G4, F4, E4, D4, C4,
    G4, A4, B4, A4, G4, E4, D4, C4,
    C4, D4, E4, G4, A4, G4, F4, E4
  };
  Sound m4 = Sound(song4_ch1, song4_ch2, sizeof(song4_ch1) / sizeof(u16_t), 12);
  m4.config_ch1_volume(6);

  u16_t song5_ch1[] = {
    C3, _, A2, F2, _, C2, C2, C2, D2, F2, Bb2, D3, C3, _, _,
    A2, Bb2, G2, E2, Bb2, A2, F2, _,
    A2, G2, D2, E2, F2, G2, _, _
  };
  Sound m5 = Sound(song5_ch1, sizeof(song5_ch1) / sizeof(u16_t), 12);

  u16_t song6_ch1[] = {
    E4, E4, E4, _, C4, E4, _, G4, _, G3, _, C4, _, G3, _, E3, _, A3, B3,
    Bb3, A3, G3, _, E4, G4, A4, _, F4, G4, _, E4, C4, D4, B3, _, C4, G3, E3, A3, B3, _, Bb3, A3, _, G3, _, E4, G4, A4, F4, G4, _, E4, C4, D4, B3

  };
  Sound m6 = Sound(song6_ch1, sizeof(song6_ch1) / sizeof(u16_t), 10);

  Sound* playlist[] = {&m1, &m5, &m2, &m3, &m4, &m6};
  i16_t selected_song = 0;

  u16_t sound_types[] = {Sound::TypeA, Sound::TypeB, Sound::TypeC, Sound::TypeD};
  i16_t selected_type = 0;

  setup_display();
  setup_spritesheet();
  Sound::init();

  u16_t next_obj = 0;
  { // UP
    auto* obj = OAM_attr::get_obj(next_obj++);
    obj->set_sprite(1);
    obj->set_x(116);
    obj->set_y(75);
    obj->set_size(OAM_attr::ObjectSize::_8x8);
  }
  { // DOWN
    auto* obj = OAM_attr::get_obj(next_obj++);
    obj->set_sprite(3);
    obj->set_x(116);
    obj->set_y(85);
    obj->set_size(OAM_attr::ObjectSize::_8x8);
  }
  { // LEFT
    auto* obj = OAM_attr::get_obj(next_obj++);
    obj->set_sprite(2);
    obj->set_x(112);
    obj->set_y(80);
    obj->set_size(OAM_attr::ObjectSize::_8x8);
  }
  { // RIGHT
    auto* obj = OAM_attr::get_obj(next_obj++);
    obj->set_sprite(2);
    obj->set_x(120);
    obj->set_y(80);
    obj->set_size(OAM_attr::ObjectSize::_8x8);
    obj->set_hflip(true);
  }
  { // label "NEXT"
    auto* obj1 = OAM_attr::get_obj(next_obj++);
    obj1->set_sprite(19);
    obj1->set_x(129);
    obj1->set_y(80);
    obj1->set_size(OAM_attr::ObjectSize::_8x8);
    auto* obj2 = OAM_attr::get_obj(next_obj++);
    obj2->set_sprite(20);
    obj2->set_x(129 + 8);
    obj2->set_y(80);
    obj2->set_size(OAM_attr::ObjectSize::_8x8);
  }
  { // label "PREV"
    auto* obj1 = OAM_attr::get_obj(next_obj++);
    obj1->set_sprite(17);
    obj1->set_x(94);
    obj1->set_y(80);
    obj1->set_size(OAM_attr::ObjectSize::_8x8);
    auto* obj2 = OAM_attr::get_obj(next_obj++);
    obj2->set_sprite(18);
    obj2->set_x(94 + 8);
    obj2->set_y(80);
    obj2->set_size(OAM_attr::ObjectSize::_8x8);
  }
  volatile OAM_attr* play_btn[2] = {nullptr, nullptr};
  { // label "PLAY"
    auto* obj1 = OAM_attr::get_obj(next_obj++);
    obj1->set_sprite(6);
    obj1->set_x(111);
    obj1->set_y(66);
    obj1->set_size(OAM_attr::ObjectSize::_8x8);
    auto* obj2 = OAM_attr::get_obj(next_obj++);
    obj2->set_sprite(7);
    obj2->set_x(111 + 8);
    obj2->set_y(66);
    obj2->set_size(OAM_attr::ObjectSize::_8x8);
    play_btn[0] = obj1;
    play_btn[1] = obj2;
  }
  play_btn[0]->set_palette(1);
  play_btn[1]->set_palette(1);
  { // label "SOUND TYPE"
    u16_t x = 100;
    auto* obj1 = OAM_attr::get_obj(next_obj++);
    obj1->set_sprite(28);
    obj1->set_x(x + 8 * 0);
    obj1->set_y(93);
    obj1->set_size(OAM_attr::ObjectSize::_8x8);
    auto* obj2 = OAM_attr::get_obj(next_obj++);
    obj2->set_sprite(29);
    obj2->set_x(x + 8 * 1);
    obj2->set_y(93);
    obj2->set_size(OAM_attr::ObjectSize::_8x8);
    auto* obj3 = OAM_attr::get_obj(next_obj++);
    obj3->set_sprite(30);
    obj3->set_x(x + 8 * 2);
    obj3->set_y(93);
    obj3->set_size(OAM_attr::ObjectSize::_8x8);
    auto* obj4 = OAM_attr::get_obj(next_obj++);
    obj4->set_sprite(31);
    obj4->set_x(x + 8 * 3 - 3);
    obj4->set_y(93);
    obj4->set_size(OAM_attr::ObjectSize::_8x8);
    auto* obj5 = OAM_attr::get_obj(next_obj++);
    obj5->set_sprite(32);
    obj5->set_x(x + 8 * 4 - 3);
    obj5->set_y(93);
    obj5->set_size(OAM_attr::ObjectSize::_8x8);
  }
  volatile OAM_attr* type_btn[2] = {nullptr, nullptr};
  { // label "1 2 3 4"
    auto* obj1 = OAM_attr::get_obj(next_obj++);
    obj1->set_sprite(39);
    obj1->set_x(110);
    obj1->set_y(101);
    obj1->set_size(OAM_attr::ObjectSize::_8x8);
    auto* obj2 = OAM_attr::get_obj(next_obj++);
    obj2->set_sprite(50);
    obj2->set_x(110 + 8);
    obj2->set_y(101);
    obj2->set_size(OAM_attr::ObjectSize::_8x8);
    type_btn[0] = obj1;
    type_btn[1] = obj2;
  }
  auto update_type_btn = [&selected_type, &type_btn]() {
    switch (selected_type) {
      case 0:
      type_btn[0]->set_sprite(40);
      type_btn[1]->set_sprite(50);
      break;
      case 1:
      type_btn[0]->set_sprite(41);
      type_btn[1]->set_sprite(50);
      break;
      case 2:
      type_btn[0]->set_sprite(39);
      type_btn[1]->set_sprite(51);
      break;
      case 3:
      type_btn[0]->set_sprite(39);
      type_btn[1]->set_sprite(52);
      break;
      default:
      type_btn[0]->set_sprite(39);
      type_btn[1]->set_sprite(50);
      break;
    }
  };
  type_btn[0]->set_sprite(40);

  while (1) {
    vid_vsync();

    if (Controller::is_pressed(BTN_UP) && !is_pressed_up) {
      if (playlist[selected_song]->is_paused()) {
        playlist[selected_song]->play();
        play_btn[0]->set_palette(1);
        play_btn[1]->set_palette(1);
      } else {
        playlist[selected_song]->pause();
        play_btn[0]->set_palette(0);
        play_btn[1]->set_palette(0);
      }
      is_pressed_up = true;
    }
    if (Controller::is_released(BTN_UP) && is_pressed_up) {
      is_pressed_up = false;
    }

    if (Controller::is_pressed(BTN_RIGHT) && !is_pressed_right) {
      playlist[selected_song]->pause();
      selected_song += 1;
      if (selected_song >= (sizeof(playlist) / sizeof(Sound*))) {
        selected_song = 0;
      }
      is_pressed_right = true;
      playlist[selected_song]->reset();
      playlist[selected_song]->config_ch1_type(sound_types[selected_type]);
      playlist[selected_song]->play();
      play_btn[0]->set_palette(1);
      play_btn[1]->set_palette(1);
    }
    if (Controller::is_released(BTN_RIGHT) && is_pressed_right) {
      is_pressed_right = false;
    }

    if (Controller::is_pressed(BTN_LEFT) && !is_pressed_left) {
      playlist[selected_song]->pause();
      selected_song -= 1;
      if (selected_song < 0) {
        selected_song = (sizeof(playlist) / sizeof(Sound*)) - 1;
      }
      is_pressed_left = true;
      playlist[selected_song]->reset();
      playlist[selected_song]->config_ch1_type(sound_types[selected_type]);
      playlist[selected_song]->play();
      play_btn[0]->set_palette(1);
      play_btn[1]->set_palette(1);
    }
    if (Controller::is_released(BTN_LEFT) && is_pressed_left) {
      is_pressed_left = false;
    }

    if (Controller::is_pressed(BTN_DOWN) && !is_pressed_down) {
      selected_type++;
      if (selected_type >= (sizeof(sound_types) / sizeof(u16_t))) {
        selected_type = 0;
      }
      playlist[selected_song]->config_ch1_type(sound_types[selected_type]);
      update_type_btn();
      is_pressed_down = true;
    }
    if (Controller::is_released(BTN_DOWN) && is_pressed_down) {
      is_pressed_down = false;
    }

    playlist[selected_song]->update();
  }
}

