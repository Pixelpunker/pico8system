#include "picosystem.hpp"
#include "picomath.cpp"
#include <optional>
#include <array>
#include "assets/data.h"
#include "assets/assets.hpp"
using namespace std;
using namespace picosystem;
using namespace picomath;

// API wrapper from PICO-8 API to picosystem API calls

// todo make a static class to hide some stuff from users of pico8 api and omit init method
namespace pico8
{
  color_t rgb2(uint16_t r, uint16_t g, uint16_t b, uint16_t a = 0xFF) { // PicoSystem only accepts 4 bit color values (0-15)
    return rgb(r/16, g/16, b/16, a/16);
  }

 const uint8_t minimal_font_data[96][9] = {
  {6,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, //  
  {6,0x00,0x00,0x2e,0x00,0x00,0x00,0x00,0x00}, // !
  {6,0x00,0x00,0x06,0x00,0x06,0x00,0x00,0x00}, // "
  {6,0x00,0x00,0x14,0x3e,0x14,0x3e,0x14,0x00}, // #
  {6,0x00,0x00,0x04,0x2a,0x3e,0x2a,0x10,0x00}, // $
  {6,0x00,0x00,0x22,0x10,0x08,0x04,0x22,0x00}, // %
  {6,0x00,0x00,0x14,0x2a,0x2a,0x2c,0x10,0x28}, // &
  {6,0x00,0x00,0x06,0x00,0x00,0x00,0x00,0x00}, // '
  {6,0x00,0x00,0x1c,0x22,0x00,0x00,0x00,0x00}, // (
  {6,0x00,0x00,0x22,0x1c,0x00,0x00,0x00,0x00}, // )
  {6,0x00,0x00,0x14,0x08,0x14,0x00,0x00,0x00}, // *
  {6,0x00,0x00,0x08,0x1c,0x08,0x00,0x00,0x00}, // +
  {6,0x00,0x00,0x60,0x00,0x00,0x00,0x00,0x00}, // ,
  {6,0x00,0x00,0x08,0x08,0x08,0x00,0x00,0x00}, // -
  {6,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00}, // .
  {6,0x00,0x00,0x30,0x0c,0x02,0x00,0x00,0x00}, // /
  {6,0x00,0x00,0x1c,0x22,0x22,0x22,0x1e,0x00}, // 0
  {6,0x00,0x00,0x02,0x3e,0x00,0x00,0x00,0x00}, // 1
  {6,0x00,0x00,0x32,0x2a,0x2a,0x24,0x00,0x00}, // 2
  {6,0x00,0x00,0x2a,0x2a,0x2a,0x16,0x00,0x00}, // 3
  {6,0x00,0x00,0x0e,0x10,0x10,0x3e,0x10,0x00}, // 4
  {6,0x00,0x00,0x2e,0x2a,0x2a,0x12,0x00,0x00}, // 5
  {6,0x00,0x00,0x3c,0x2a,0x2a,0x2a,0x12,0x00}, // 6
  {6,0x00,0x00,0x06,0x02,0x22,0x12,0x0e,0x00}, // 7
  {6,0x00,0x00,0x14,0x2a,0x2a,0x2a,0x16,0x00}, // 8
  {6,0x00,0x00,0x04,0x2a,0x2a,0x2a,0x1e,0x00}, // 9
  {6,0x00,0x00,0x24,0x00,0x00,0x00,0x00,0x00}, // :
  {6,0x00,0x00,0x64,0x00,0x00,0x00,0x00,0x00}, // ;
  {6,0x00,0x00,0x08,0x14,0x22,0x00,0x00,0x00}, // <
  {6,0x00,0x00,0x14,0x14,0x14,0x00,0x00,0x00}, // =
  {6,0x00,0x00,0x22,0x14,0x08,0x00,0x00,0x00}, // >
  {6,0x00,0x00,0x02,0x2a,0x0a,0x04,0x00,0x00}, // ?
  {6,0x00,0x00,0x3c,0x02,0x1a,0x2a,0x22,0x1e}, // @
  {6,0x00,0x00,0x3c,0x12,0x12,0x12,0x3e,0x00}, // A
  {6,0x00,0x00,0x3c,0x2a,0x2a,0x2e,0x10,0x00}, // B
  {6,0x00,0x00,0x1c,0x22,0x22,0x22,0x00,0x00}, // C
  {6,0x00,0x00,0x3c,0x22,0x22,0x22,0x1c,0x00}, // D
  {6,0x00,0x00,0x3c,0x2a,0x2a,0x2a,0x00,0x00}, // E
  {6,0x00,0x00,0x3c,0x12,0x12,0x12,0x00,0x00}, // F
  {6,0x00,0x00,0x3c,0x22,0x22,0x2a,0x1a,0x00}, // G
  {6,0x00,0x00,0x3e,0x08,0x08,0x3e,0x00,0x00}, // H
  {6,0x00,0x00,0x22,0x3e,0x22,0x00,0x00,0x00}, // I
  {6,0x00,0x00,0x30,0x22,0x22,0x1e,0x00,0x00}, // J
  {6,0x00,0x00,0x3e,0x08,0x0c,0x32,0x00,0x00}, // K
  {6,0x00,0x00,0x3e,0x20,0x20,0x20,0x00,0x00}, // L
  {6,0x00,0x00,0x3c,0x02,0x3c,0x02,0x3c,0x00}, // M
  {6,0x00,0x00,0x3c,0x02,0x02,0x02,0x3e,0x00}, // N
  {6,0x00,0x00,0x1c,0x22,0x22,0x22,0x1e,0x00}, // O
  {6,0x00,0x00,0x3c,0x12,0x12,0x12,0x0e,0x00}, // P
  {6,0x00,0x00,0x1c,0x22,0x22,0x62,0x1e,0x00}, // Q
  {6,0x00,0x00,0x3c,0x12,0x12,0x32,0x0e,0x00}, // R
  {6,0x00,0x00,0x24,0x2a,0x2a,0x12,0x00,0x00}, // S
  {6,0x00,0x00,0x02,0x02,0x3e,0x02,0x02,0x00}, // T
  {6,0x00,0x00,0x1e,0x20,0x20,0x20,0x1e,0x00}, // U
  {6,0x00,0x00,0x0e,0x10,0x20,0x10,0x0e,0x00}, // V
  {6,0x00,0x00,0x3e,0x20,0x1e,0x20,0x1e,0x00}, // W
  {6,0x00,0x00,0x36,0x08,0x08,0x36,0x00,0x00}, // X
  {6,0x00,0x00,0x26,0x28,0x28,0x1e,0x00,0x00}, // Y
  {6,0x00,0x00,0x32,0x2a,0x2a,0x26,0x00,0x00}, // Z
  {6,0x00,0x00,0x3e,0x22,0x00,0x00,0x00,0x00}, // [
  {6,0x00,0x00,0x02,0x0c,0x30,0x00,0x00,0x00}, // "\"
  {6,0x00,0x00,0x22,0x3e,0x00,0x00,0x00,0x00}, // ]
  {6,0x00,0x00,0x04,0x02,0x04,0x00,0x00,0x00}, // ^
  {6,0x00,0x00,0x20,0x20,0x20,0x00,0x00,0x00}, // _
  {6,0x00,0x00,0x02,0x04,0x00,0x00,0x00,0x00}, // `
  {6,0x00,0x00,0x3c,0x12,0x12,0x12,0x3e,0x00}, // a
  {6,0x00,0x00,0x3c,0x2a,0x2a,0x2e,0x10,0x00}, // b
  {6,0x00,0x00,0x1c,0x22,0x22,0x22,0x00,0x00}, // c
  {6,0x00,0x00,0x3c,0x22,0x22,0x22,0x1c,0x00}, // d
  {6,0x00,0x00,0x3c,0x2a,0x2a,0x2a,0x00,0x00}, // e
  {6,0x00,0x00,0x3c,0x12,0x12,0x12,0x00,0x00}, // f
  {6,0x00,0x00,0x3c,0x22,0x22,0x2a,0x1a,0x00}, // g
  {6,0x00,0x00,0x3e,0x08,0x08,0x3e,0x00,0x00}, // h
  {6,0x00,0x00,0x22,0x3e,0x22,0x00,0x00,0x00}, // i
  {6,0x00,0x00,0x30,0x22,0x22,0x1e,0x00,0x00}, // j
  {6,0x00,0x00,0x3e,0x08,0x0c,0x32,0x00,0x00}, // k
  {6,0x00,0x00,0x3e,0x20,0x20,0x20,0x00,0x00}, // l
  {6,0x00,0x00,0x3c,0x02,0x3c,0x02,0x3e,0x00}, // m
  {6,0x00,0x00,0x3c,0x02,0x02,0x02,0x3e,0x00}, // n
  {6,0x00,0x00,0x1c,0x22,0x22,0x22,0x1e,0x00}, // o
  {6,0x00,0x00,0x3c,0x12,0x12,0x12,0x0e,0x00}, // p
  {6,0x00,0x00,0x1c,0x22,0x22,0x62,0x1e,0x00}, // q
  {6,0x00,0x00,0x3c,0x12,0x12,0x32,0x0e,0x00}, // r
  {6,0x00,0x00,0x24,0x2a,0x2a,0x12,0x00,0x00}, // s
  {6,0x00,0x00,0x02,0x02,0x3e,0x02,0x02,0x00}, // t
  {6,0x00,0x00,0x1e,0x20,0x20,0x20,0x1e,0x00}, // u
  {6,0x00,0x00,0x0e,0x10,0x20,0x10,0x0e,0x00}, // v
  {6,0x00,0x00,0x3e,0x20,0x1e,0x20,0x1e,0x00}, // w
  {6,0x00,0x00,0x36,0x08,0x08,0x36,0x00,0x00}, // x
  {6,0x00,0x00,0x26,0x28,0x28,0x1e,0x00,0x00}, // y
  {6,0x00,0x00,0x32,0x2a,0x2a,0x26,0x00,0x00}, // z
  {6,0x00,0x00,0x08,0x3e,0x22,0x00,0x00,0x00}, // {
  {6,0x00,0x00,0x3e,0x00,0x00,0x00,0x00,0x00}, // |
  {6,0x00,0x00,0x22,0x3e,0x08,0x00,0x00,0x00}, // }
  {6,0x00,0x00,0x04,0x02,0x02,0x00,0x00,0x00}, // ~
  {6,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
};

uint8_t *minimal_font = (uint8_t *)&minimal_font_data[0][0];

static const uint8_t minimal_font_width[96] = {
  3, 2, 4, 6, 6, 6, 7, 2, 3, 3, 4, 4, 2, 4, 2, 4,
  6, 3, 5, 5, 6, 5, 6, 6, 6, 6, 2, 2, 4, 4, 4, 5,
  7, 6, 6, 5, 6, 5, 5, 6, 5, 4, 5, 5, 5, 6, 6, 6,
  6, 6, 6, 5, 6, 6, 6, 6, 5, 5, 5, 3, 4, 3, 4, 4,
  3, 6, 6, 5, 6, 5, 5, 6, 5, 4, 5, 5, 5, 6, 6, 6,
  6, 6, 6, 5, 6, 6, 6, 6, 5, 5, 5, 4, 2, 4, 4, 2
};


  // tip: adjust systemoffset during gameplay
  static const array<color_t, 32> system_palette = {
      0x00F0, //rgb2(0, 0, 0),         // 0 	black (also transparent by default for sprites)
      0x25F1, //rgb2(29, 43, 83),      // 1 	dark-blue
      0x25F7, //rgb2(126, 37, 83),     // 2 	dark-purple
      0x85F0, //rgb2(0, 135, 81),      // 3 	dark-green
      0x53FA, //rgb2(171, 82, 54),     // 4 	brown
      0x54F5, //rgb2(95, 87, 79),      // 5 	dark-grey
      0xCCFC, //rgb2(194, 195, 199),   // 6 	light-grey
      0xFEFF, //rgb2(255, 241, 232),   // 7 	white
      0x04FF, //rgb2(255, 0, 77),      // 8 	red
      0xA0FF, //rgb2(255, 163, 0),     // 9	orange
      0xE2FF, //rgb2(255, 236, 39),    // 10	yellow
      0xE3F0, //rgb2(0, 228, 54),      // 11 green
      0xAFF2, //rgb2(41, 173, 255),    // 12 blue
      0x79F8, //rgb2(131, 118, 156),   // 13 lavender
      0x7AFF, //rgb2(255, 119, 168),   // 14 pink
      0xCAFF, //rgb2(255, 204, 170),   // 15 light-peach
      0x11F2, //rgb2(41, 24, 20),    // 128 	brownish-black
      0x13F1, //rgb2(17, 29, 53),    // 129 	darker-blue
      0x23F4, //rgb2(66, 33, 54),    // 130 	darker-purple
      0x55F1, //rgb2(18, 83, 89),    // 131 	blue-green
      0x22F7, //rgb2(116, 47, 41),   // 132 	dark-brown
      0x33F4, //rgb2(73, 51, 59),    // 133 	darker-grey
      0x87FA, //rgb2(162, 136, 121), // 134 	medium-grey
      0xE7FF, //rgb2(243, 239, 125), // 135 	light-yellow
      0x15FB, //rgb2(190, 18, 80),   // 136 	dark-red
      0x62FF, //rgb2(255, 108, 36),  // 137 	dark-orange
      0xE2FA, //rgb2(168, 231, 46),  // 138 	lime-green
      0xB4F0, //rgb2(0, 181, 67),    // 139 	medium-green
      0x5BF0, //rgb2(6, 90, 181),    // 140	true-blue
      0x46F7, //rgb2(117, 70, 101),  // 141 	mauve
      0x65FF, //rgb2(255, 110, 89),  // 142 	dark-peach
      0x98FF //rgb2(255, 157, 129), // 143 	peach
  };
  array<color_t, (uint_fast8_t)16> draw_palette = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
  };
  array<color_t, (uint_fast8_t)16> secondary_palette = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
  };
  bool swapped_buttons = false;

  const int picowidth = 136; // workaround for
                            // screen shake issue, should be 128
  color_t _fdp[picowidth * picowidth] __attribute__ ((aligned (4))) = { };
  static buffer_t *PICO8SCREEN = buffer(picowidth, picowidth, _fdp);

  
  // rnd() // clever me uses the current battery voltage as a random seed.

  color_t getCurrentPencolor()
  { // actually we should not access 'private' variables from the picosystem namespace
    return _pen;
  }

 void cls(uint32_t color = 0)
  {
    auto lastpencolor = getCurrentPencolor();
    pen(draw_palette[color]);
    // pen(7,7,7);
  	target();
    clear();
    target(pico8::PICO8SCREEN);
    // pen(0,0,255);
    clear();
    pen(lastpencolor);
  }

  void cls(number color = 0)
  {
    cls((int)color);
  }

  // spr implementation. no swapped palettes supported yet.
  void spr(int32_t spriteindex, int32_t x, int32_t y, int32_t cols, int32_t rows, bool flipx, bool flipy)
  {
    blend(MASK);
    int32_t flags = 0;
    if (!flipx & !flipy)
    {
      flags = 0;
    }
    else if (!flipx & flipy)
    {
      flags = 2;
    }
    else if (flipx & !flipy)
    {
      flags = 1;
    }
    else if (flipx & flipy)
    {
      flags = 3;
    }
    
    sprite(spriteindex, x, y, cols, rows, 8 * cols, 8 * rows, flags);
  }

  void spr(number spriteindex, number x, number y, number cols, number rows, bool flipx, bool flipy)
  {
    spr((int)spriteindex, (int)x, (int)y, (int)cols, (int)rows, flipx, flipy);
  }

  // reset palette
  void pal()
  {
    for (auto i = 0; i < 16; i++)
    {
      draw_palette[i] = system_palette[i];
      secondary_palette[i] = system_palette[i];
    }
  }

  void pal(uint32_t c0, uint32_t c1, uint32_t p = 0)
  {
    switch (p)
    {
    case 0:
    default:
      draw_palette[c0] = system_palette[c1];
      draw_palette[c1] = system_palette[c0];
      break;
    case 1:
      // for p = 1 we would need to do a search and replace on the screen buffer but
      // not on palette indexes but on real 12 bit color values
      break;
    case 2:
      draw_palette[c0] = secondary_palette[c1];
      draw_palette[c1] = secondary_palette[c0];
      break;
    }
  }

  void pal(number c0, number c1, number p = 0)
  {
    pal((int)c0, (int)c1, (int)p);
  }

  void rect(int32_t x, int32_t y, int32_t x2, int32_t y2, int32_t c)
  {
    
    auto lastpencolor = getCurrentPencolor();
    pen(draw_palette[c]);
    picosystem::rect(x, y, x2 - x, y2 - y);
    pen(lastpencolor);
  }

  void rect(number x, number y, number x2, number y2, number c)
  {
    rect((int)x, (int)y, (int)x2, (int)y2, (int)c);
  }

  void rectfill(int32_t x, int32_t y, int32_t x2, int32_t y2, int32_t c)
  {
    
    auto lastpencolor = getCurrentPencolor();
    pen(draw_palette[c]);
    frect(x, y, x2 - x, y2 - y);
    pen(lastpencolor);
  }

  void rectfill(number x, number y, number x2, number y2, number c)
  {
    rectfill((int)x, (int)y, (int)x2, (int)y2, (int)c);
  }

  static int gettileflag(int tile, int flag)
  {
    return tile < sizeof(sprite_flags) / sizeof(*sprite_flags) && (sprite_flags[tile] & (1 << flag)) != 0;
  }

  array<bool, 8> getFlags(int flag)
  {
    array<bool, 8> result = {
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false};
    for (auto i = 0; i < 8; i++)
    {
      auto move = flag;
      move >> i;
      result[i] = move & 0x0000000F;
    }

    return result;
  }

  //todo NUR fget/set kopieren, und alle uint32_t durch uint_fast16_t ersetzen,
  //auch in meinem custom number type
  int32_t fget(uint32_t n)
  {
    return sprite_flags[n];
  }

  number fget(number n)
  {
    return fget((int)n);
  }

  bool fget(uint32_t n, uint32_t f)
  {
    return n < sizeof(sprite_flags) / sizeof(*sprite_flags) && (sprite_flags[n] & (1 << f)) != 0;
    // auto spriteflag = sprite_flags[n];
    // spriteflag = spriteflag >> f;
    // return (bool)(spriteflag | 0x01);
  }

  bool fget(number n, number f)
  {
    return fget((int)n, (int)f);
  }

  void fset(uint32_t n, uint32_t f)
  {
    sprite_flags[n] = f;
  }

  void fset(number n, number f)
  {
    fset((int)n, (int)f);
  }

  void fset(uint32_t n, uint32_t f, bool val)
  {

    auto spriteflag = sprite_flags[n];
    auto newvalue = (uint32_t)val;
    newvalue = newvalue << f;
    sprite_flags[n] = spriteflag | newvalue;
  }

  void fset(number n, number f, bool val)
  {
    fset(n, f, val);
  }

  void map(uint32_t cell_x = 0, uint32_t cell_y = 0, int32_t sx = 0, int32_t sy = 0, uint32_t cell_w = 128, uint32_t cell_h = 32, uint32_t layers = 0)
  {
    blend(MASK);
    
    for (auto x = cell_x; x < cell_w + cell_x; x++)
    {
      for (auto y = cell_y; y < cell_h + cell_y; y++)
      {
        auto tileindex = map_data[x + y * 128];
        // if (layers == 0 || sprite_flags[tileindex] == 4 || gettileflag(tileindex, layers != 4 ? layers-1 : layers))
        auto masked = sprite_flags[tileindex] | layers;
        // if (layers == 0 || (layers == 4 && sprite_flags[tileindex] == 4) || fget(tileindex, layers != 4 ? layers-1 : layers))
        if (layers == 0 || masked == sprite_flags[tileindex])
        {
          sprite(tileindex, sx + (x * 8 - cell_x * 8), sy + (y * 8 - cell_y * 8));
        }
      }
    }
  }

  void map(number cell_x = 0, number cell_y = 0, number sx = 0, number sy = 0, number cell_w = 128, number cell_h = 32, number layers = 0)
  {
    map((int)cell_x, (int)cell_y, (int)sx, (int)sy, (int)cell_w, (int)cell_h, (int)layers);
  }

  int32_t mget(uint32_t celx, uint32_t cely)
  {
    return map_data[celx + cely * 128];
  }

  number mget(number celx, number cely)
  {
    return mget((int)celx, (int)cely);
  }

  void camera(int32_t x, int32_t y)
  {
    picosystem::camera(x, y);
  }

  void camera(number x, number y)
  {
    camera((int)x, (int)y);
  }

  // reset camera
  void camera()
  {
    picosystem::camera();
  }

  pair<int32_t, int32_t> lastline(0, 0); // PRIVATE
  void line(int32_t x0, int32_t y0, optional<int32_t> x1, optional<int32_t> y1, optional<int32_t> c)
  {
    
    auto color = getCurrentPencolor();
    int32_t startx, starty, endx, endy;
    if (x1.has_value())
    {
      startx = x0;
      starty = y0;
      endx = x1.value();
      endy = y1.value();
      lastline.first = x1.value();
      lastline.second = y1.value();
    }
    else
    {
      startx = lastline.first;
      starty = lastline.second;
      endx = x0;
      endy = y0;
      lastline.first = x0;
      lastline.second = y0;
    }
    if (c.has_value())
    {
      pen(draw_palette[c.value()]);
    }
    if (startx == endx)
    {
      vline(startx, starty, endy - starty);
    }
    else if (starty == endy)
    {
      hline(startx, starty, endx - startx);
    }
    else
    {
      picosystem::line(startx, starty, endx, endy);
    }
    pen(color); // restore previous pen color
  }

  void line(number x0, number y0, optional<number> x1, optional<number> y1, optional<number> c)
  {
    line((int)x0, (int)y0, (optional<number>)x1, (optional<number>)y1, (optional<number>)c);
  }

  static void circ(int32_t x, int32_t y, uint32_t r, optional<uint32_t> c)
  {
    auto color = getCurrentPencolor();
    
    if (c.has_value())
    {
      pen(draw_palette[c.value()]);
    }
    circle(x, y, r);
    pen(color); // restore previous pen color
  }

  static void circ(number x, number y, number r, optional<number> c)
  {
    circ((int)x, (int)y, (int)r, (optional<int>)c);
  }

  static void circfill(int32_t x, int32_t y, uint32_t r, optional<uint32_t> c)
  {
    auto color = getCurrentPencolor();
    
    if (c.has_value())
    {
      pen(draw_palette[c.value()]);
    }
    fcircle(x, y, r);
    pen(color); // restore previous pen color
  }

  static void circfill(number x, number y, number r, optional<number> c)
  {
    circfill((int)x, (int)y, (int)r, (optional<int>)c);
  }

  // no support for the P8SCII special control codes planned
  static void print(string str, int32_t x, int32_t y, optional<uint32_t> c)
  {
    auto color = getCurrentPencolor();
    
    if (c.has_value())
    {
      pen(draw_palette[c.value()]);
    }
    text(str, x, y);
    pen(color); // restore previous pen color
  }

  static void print(string str, number x, number y, optional<number> c)
  {
    print(str, (int)x, (int)y, (optional<int>)c);
  }

  // no support for the P8SCII special control codes planned
  static void print(string str, optional<uint32_t> c)
  {
    auto color = getCurrentPencolor();
    
    if (c.has_value())
    {
      pen(draw_palette[c.value()]);
    }
    text(str);
    pen(color); // restore previous pen color
  }

  static number btn()
  { // return bitfield
    auto left = (int)button(LEFT);
    auto right = (int)button(RIGHT) << 1;
    auto up = (int)button(UP) << 2;
    auto down = (int)button(DOWN) << 3;
    auto button_o = (int)button((swapped_buttons ? A : B)) << 4;
    auto button_x = (int)button((swapped_buttons ? B : A)) << 5;
    return left | right | up | down | button_o | button_x;
  }

  static bool btn(int32_t b, int32_t pl = 0)
  {
    pl = 0; // pl is always 0, parameter only offered for compatibility.
            // picosystem is a single player device
    switch (b)
    {
    case 0: // left
      return button(LEFT);
    case 1: // right
      return button(RIGHT);
    case 2: // up
      return button(UP);
    case 3: // down
      return button(DOWN);
    case 4: // button_o
      return button((swapped_buttons ? A : B));
    case 5: // button_x
      return button((swapped_buttons ? B : A));
    default:
      return false;
    }
  }

  static bool btn(number b, number pl = 0)
  {
    return btn((int)b, (int)pl);
  }

  static number btnp()
  { // return bitfield
    auto left = (int)pressed(LEFT);
    auto right = (int)pressed(RIGHT) << 1;
    auto up = (int)pressed(UP) << 2;
    auto down = (int)pressed(DOWN) << 3;
    auto button_o = (int)pressed((swapped_buttons ? A : B)) << 4;
    auto button_x = (int)pressed((swapped_buttons ? B : A)) << 5;
    return left | right | up | down | button_o | button_x;
  }

  static bool btnp(int32_t b, int32_t pl = 0)
  {
    pl = 0; // pl is always 0, parameter only offered for compatibility.
            // picosystem is a single player device
    switch (b)
    {
    case 0: // left
      return pressed(LEFT);
    case 1: // right
      return pressed(RIGHT);
    case 2: // up
      return pressed(UP);
    case 3: // down
      return pressed(DOWN);
    case 4: // button_o
      return pressed((swapped_buttons ? A : B));
    case 5: // button_x
      return pressed((swapped_buttons ? B : A));
    default:
      return false;
    }
  }

  static bool btnp(number b, number pl = 0)
  {
    return btnp((int)b, (int)pl);
  }

  // **********************************************************************************************************************************
  // UNDER CONSTRUCTION SECTION

  // Note: with the exception of map data and user-defined data
  // it is probably best not to reimplement all the peek and poke
  // trickery of the fantasy console but to change the source code
  // of the game to achieve things using the picosystem api directly.
  //
  // GFX
  // Specifically the gfx portion cannot be accessed because
  // picosystem stores the spritesheet as rgb colors (4096 possible colors)
  // plus 16 levels of transparency while pico8 uses a fixed 16 color palette of
  // which one color can be defined as transparent. we can convert from pico8
  // to picosystem but not vice versa.
  // living in picosystem land enables more colors and the different blend effects.
  //
  // The intention of the pico8 compatibility layer is to aid in the porting
  // of pico-8 carts that can be enhanced even further, not to be an automatic
  // pico-8 runtime.
  void peek(int32_t address)
  {
  }

  // TODO implement sfx
  // dummy function to be implemented
  void sfx(uint32_t n, uint32_t channel = 0, uint32_t offset = 0, uint32_t length = 255)
  {
  }

  // TODO implement music
  // dummy function to be implemented
  void music(int32_t n, uint32_t fade_len = 0, uint32_t channel_mask = 15)
  {
  }

  void hudDrawing(bool active) { // inversly adjust the camera for drawing hud elements so they stay in the
                                 // same place while moving the viewport
  }


  void init(bool swapped_buttons = false)
  {
    //font(-1, 6, 8, minimal_font);
    swapped_buttons = swapped_buttons;
    // set drawing region to 128x128 (visible only 120x120 controlled by system_offset and hud_offset)
    clip(0, 0, 128, 128);
    

    
    target(PICO8SCREEN);

    blend(MASK);
    // load spritesheet
    spritesheet(buffer(128, 64, spritedata));
    // reset palette
    pal();
  }

}
