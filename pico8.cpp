#include "picosystem.hpp"
#include "picomath.cpp"
#include <optional>
#include <array>
#include "assets/data.h"
#include "assets/assets.hpp"
#include "assets/mountain.hpp"
#include <stdio.h>
// #include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/flash.h"

using namespace std;
using namespace picosystem;
using namespace picomath;

// API wrapper from PICO-8 API to picosystem API calls

namespace pico8
{
#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES / 4 * 3 - FLASH_SECTOR_SIZE * 2)

  const uint8_t *flash_target_contents = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);

  uint8_t save_data[FLASH_PAGE_SIZE] = {123, 1, 1, 0, 0};

  const uint8_t magicvalue = 123;

  color_t rgb2(uint16_t r, uint16_t g, uint16_t b, uint16_t a = 0xFF)
  { // PicoSystem only accepts 4 bit color values (0-15)
    return rgb(r / 16, g / 16, b / 16, a / 16);
  }

  const uint8_t _minimal_font_data[96][9] = {
      {4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //
      {3, 0x80, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00}, // !
      {5, 0xA0, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // "
      {7, 0x50, 0xF8, 0x50, 0xF8, 0x50, 0x00, 0x00, 0x00}, // #
      {7, 0x70, 0xA0, 0x70, 0x28, 0x70, 0x00, 0x00, 0x00}, // $
      {7, 0x88, 0x10, 0x20, 0x40, 0x88, 0x00, 0x00, 0x00}, // %
      {8, 0x60, 0x90, 0x74, 0x88, 0x74, 0x00, 0x00, 0x00}, // &
      {3, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // '
      {4, 0x40, 0x80, 0x80, 0x80, 0x40, 0x00, 0x00, 0x00}, // (
      {4, 0x80, 0x40, 0x40, 0x40, 0x80, 0x00, 0x00, 0x00}, // )
      {5, 0x00, 0xA0, 0x40, 0xA0, 0x00, 0x00, 0x00, 0x00}, // *
      {5, 0x00, 0x40, 0xE0, 0x40, 0x00, 0x00, 0x00, 0x00}, // +
      {3, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00}, // ,
      {5, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00}, // -
      {3, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00}, // .
      {5, 0x20, 0x40, 0x40, 0x80, 0x80, 0x00, 0x00, 0x00}, // /
      {7, 0x78, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x00}, // 0
      {4, 0xC0, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00}, // 1
      {6, 0xE0, 0x10, 0x60, 0x80, 0xF0, 0x00, 0x00, 0x00}, // 2
      {6, 0xF0, 0x10, 0xE0, 0x10, 0xE0, 0x00, 0x00, 0x00}, // 3
      {7, 0x90, 0x90, 0x90, 0x78, 0x10, 0x00, 0x00, 0x00}, // 4
      {6, 0xF0, 0x80, 0xE0, 0x10, 0xE0, 0x00, 0x00, 0x00}, // 5
      {7, 0x78, 0x80, 0xF0, 0x88, 0xF0, 0x00, 0x00, 0x00}, // 6
      {7, 0xF8, 0x88, 0x08, 0x10, 0x20, 0x00, 0x00, 0x00}, // 7
      {7, 0x78, 0x88, 0x70, 0x88, 0x70, 0x00, 0x00, 0x00}, // 8
      {7, 0x78, 0x88, 0x78, 0x08, 0x70, 0x00, 0x00, 0x00}, // 9
      {3, 0x00, 0x80, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00}, // :
      {3, 0x00, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00}, // ;
      {5, 0x20, 0x40, 0x80, 0x40, 0x20, 0x00, 0x00, 0x00}, // <
      {5, 0x00, 0xE0, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00}, // =
      {5, 0x80, 0x40, 0x20, 0x40, 0x80, 0x00, 0x00, 0x00}, // >
      {6, 0xE0, 0x10, 0x60, 0x00, 0x40, 0x00, 0x00, 0x00}, // ?
      {8, 0x7C, 0x84, 0xB4, 0xA4, 0x98, 0x00, 0x00, 0x00}, // @
      {7, 0x78, 0x88, 0x88, 0xF8, 0x88, 0x00, 0x00, 0x00}, // A
      {7, 0x70, 0x90, 0xF0, 0x88, 0xF0, 0x00, 0x00, 0x00}, // B
      {6, 0x70, 0x80, 0x80, 0x80, 0x70, 0x00, 0x00, 0x00}, // C
      {7, 0x70, 0x88, 0x88, 0x88, 0xF0, 0x00, 0x00, 0x00}, // D
      {6, 0x70, 0x80, 0xF0, 0x80, 0xF0, 0x00, 0x00, 0x00}, // E
      {6, 0x70, 0x80, 0x80, 0xF0, 0x80, 0x00, 0x00, 0x00}, // F
      {7, 0x78, 0x80, 0x98, 0x88, 0xF0, 0x00, 0x00, 0x00}, // G
      {6, 0x90, 0x90, 0xF0, 0x90, 0x90, 0x00, 0x00, 0x00}, // H
      {5, 0xE0, 0x40, 0x40, 0x40, 0xE0, 0x00, 0x00, 0x00}, // I
      {6, 0x70, 0x10, 0x10, 0x90, 0xE0, 0x00, 0x00, 0x00}, // J
      {6, 0x90, 0xA0, 0xE0, 0x90, 0x90, 0x00, 0x00, 0x00}, // K
      {6, 0x80, 0x80, 0x80, 0x80, 0xF0, 0x00, 0x00, 0x00}, // L
      {7, 0x50, 0xA8, 0xA8, 0xA8, 0xA8, 0x00, 0x00, 0x00}, // M
      {7, 0x78, 0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00}, // N
      {7, 0x78, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x00}, // O
      {7, 0x78, 0x88, 0x88, 0xF0, 0x80, 0x00, 0x00, 0x00}, // P
      {7, 0x78, 0x88, 0x88, 0x88, 0x70, 0x10, 0x00, 0x00}, // Q
      {7, 0x78, 0x88, 0x88, 0xF0, 0x90, 0x00, 0x00, 0x00}, // R
      {6, 0x70, 0x80, 0x60, 0x10, 0xE0, 0x00, 0x00, 0x00}, // S
      {7, 0xF8, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00}, // T
      {7, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x00}, // U
      {7, 0x88, 0x88, 0x88, 0x50, 0x20, 0x00, 0x00, 0x00}, // V
      {7, 0xA8, 0xA8, 0xA8, 0xA8, 0xD0, 0x00, 0x00, 0x00}, // W
      {6, 0x90, 0x90, 0x60, 0x90, 0x90, 0x00, 0x00, 0x00}, // X
      {6, 0x90, 0x90, 0x70, 0x10, 0xE0, 0x00, 0x00, 0x00}, // Y
      {6, 0xF0, 0x10, 0x60, 0x80, 0xF0, 0x00, 0x00, 0x00}, // Z
      {4, 0xC0, 0x80, 0x80, 0x80, 0xC0, 0x00, 0x00, 0x00}, // [
      {5, 0x80, 0x40, 0x40, 0x20, 0x20, 0x00, 0x00, 0x00}, // "\"
      {4, 0xC0, 0x40, 0x40, 0x40, 0xC0, 0x00, 0x00, 0x00}, // ]
      {5, 0x40, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // ^
      {5, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00}, // _
      {4, 0x80, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // `
      {7, 0x78, 0x88, 0x88, 0xF8, 0x88, 0x00, 0x00, 0x00}, // a
      {7, 0x70, 0x90, 0xF0, 0x88, 0xF0, 0x00, 0x00, 0x00}, // b
      {6, 0x70, 0x80, 0x80, 0x80, 0x70, 0x00, 0x00, 0x00}, // c
      {7, 0x70, 0x88, 0x88, 0x88, 0xF0, 0x00, 0x00, 0x00}, // d
      {6, 0x70, 0x80, 0xF0, 0x80, 0xF0, 0x00, 0x00, 0x00}, // e
      {6, 0x70, 0x80, 0x80, 0xF0, 0x80, 0x00, 0x00, 0x00}, // f
      {7, 0x78, 0x80, 0x98, 0x88, 0xF0, 0x00, 0x00, 0x00}, // g
      {6, 0x90, 0x90, 0xF0, 0x90, 0x90, 0x00, 0x00, 0x00}, // h
      {5, 0xE0, 0x40, 0x40, 0x40, 0xE0, 0x00, 0x00, 0x00}, // i
      {6, 0x70, 0x10, 0x10, 0x90, 0xE0, 0x00, 0x00, 0x00}, // j
      {6, 0x90, 0xA0, 0xE0, 0x90, 0x90, 0x00, 0x00, 0x00}, // k
      {6, 0x80, 0x80, 0x80, 0x80, 0xF0, 0x00, 0x00, 0x00}, // l
      {7, 0x58, 0xA8, 0xA8, 0xA8, 0xA8, 0x00, 0x00, 0x00}, // m
      {7, 0x78, 0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00}, // n
      {7, 0x78, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x00}, // o
      {7, 0x78, 0x88, 0x88, 0xF0, 0x80, 0x00, 0x00, 0x00}, // p
      {7, 0x78, 0x88, 0x88, 0x88, 0x70, 0x10, 0x00, 0x00}, // q
      {7, 0x78, 0x88, 0x88, 0xF0, 0x90, 0x00, 0x00, 0x00}, // r
      {6, 0x70, 0x80, 0x60, 0x10, 0xE0, 0x00, 0x00, 0x00}, // s
      {7, 0xF8, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00}, // t
      {7, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x00}, // u
      {7, 0x88, 0x88, 0x88, 0x50, 0x20, 0x00, 0x00, 0x00}, // v
      {7, 0xA8, 0xA8, 0xA8, 0xA8, 0xD0, 0x00, 0x00, 0x00}, // w
      {6, 0x90, 0x90, 0x60, 0x90, 0x90, 0x00, 0x00, 0x00}, // x
      {6, 0x90, 0x90, 0x70, 0x10, 0xE0, 0x00, 0x00, 0x00}, // y
      {6, 0xF0, 0x10, 0x60, 0x80, 0xF0, 0x00, 0x00, 0x00}, // z
      {5, 0x60, 0x40, 0xC0, 0x40, 0x60, 0x00, 0x00, 0x00}, // {
      {3, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00}, // |
      {5, 0xC0, 0x40, 0x60, 0x40, 0xC0, 0x00, 0x00, 0x00}, // }
      {5, 0x60, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // ~
      {3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

  uint8_t *_minimal_font = (uint8_t *)&_minimal_font_data[0][0];

  static array<color_t, 32> system_palette = {
      // 0xGBAR
      0x0000, // rgb2(0, 0, 0),         // 0 	black (also transparent by default for sprites)
      0x2511, // rgb2(29, 43, 83),      // 1 	dark-blue
      0x2527, // rgb2(126, 37, 83),     // 2 	dark-purple
      0x8530, // rgb2(0, 135, 81),      // 3 	dark-green
      0x534A, // rgb2(171, 82, 54),     // 4 	brown
      0x5455, // rgb2(95, 87, 79),      // 5 	dark-grey
      0xCC6C, // rgb2(194, 195, 199),   // 6 	light-grey
      0xFE7F, // rgb2(255, 241, 232),   // 7 	white
      0x048F, // rgb2(255, 0, 77),      // 8 	red
      0xA09F, // rgb2(255, 163, 0),     // 9	orange
      0xE2AF, // rgb2(255, 236, 39),    // 10	yellow
      0xE3B0, // rgb2(0, 228, 54),      // 11 green
      0xAFC2, // rgb2(41, 173, 255),    // 12 blue
      0x79D8, // rgb2(131, 118, 156),   // 13 lavender
      0x7AEF, // rgb2(255, 119, 168),   // 14 pink
      0xCAFF, // rgb2(255, 204, 170),   // 15 light-peach
      0x1102, // rgb2(41, 24, 20),    // 128 	brownish-black
      0x1311, // rgb2(17, 29, 53),    // 129 	darker-blue
      0x2324, // rgb2(66, 33, 54),    // 130 	darker-purple
      0x5531, // rgb2(18, 83, 89),    // 131 	blue-green
      0x2247, // rgb2(116, 47, 41),   // 132 	dark-brown
      0x3354, // rgb2(73, 51, 59),    // 133 	darker-grey
      0x876A, // rgb2(162, 136, 121), // 134 	medium-grey
      0xE77F, // rgb2(243, 239, 125), // 135 	light-yellow
      0x158B, // rgb2(190, 18, 80),   // 136 	dark-red
      0x629F, // rgb2(255, 108, 36),  // 137 	dark-orange
      0xE2AA, // rgb2(168, 231, 46),  // 138 	lime-green
      0xB4B0, // rgb2(0, 181, 67),    // 139 	medium-green
      0x5BC0, // rgb2(6, 90, 181),    // 140	true-blue
      0x46D7, // rgb2(117, 70, 101),  // 141 	mauve
      0x65EF, // rgb2(255, 110, 89),  // 142 	dark-peach
      0x98FF  // rgb2(255, 157, 129), // 143 	peach
  };
  static array<color_t, 32> system_palette2 = {
      // 0xGBAR
      0x00F0, // rgb2(0, 0, 0),         // 0 	black (also transparent by default for sprites)
      0x25F1, // rgb2(29, 43, 83),      // 1 	dark-blue
      0x25F7, // rgb2(126, 37, 83),     // 2 	dark-purple
      0x85F0, // rgb2(0, 135, 81),      // 3 	dark-green
      0x53FA, // rgb2(171, 82, 54),     // 4 	brown
      0x54F5, // rgb2(95, 87, 79),      // 5 	dark-grey
      0xCCFC, // rgb2(194, 195, 199),   // 6 	light-grey
      0xFEFF, // rgb2(255, 241, 232),   // 7 	white
      0x04FF, // rgb2(255, 0, 77),      // 8 	red
      0xA0FF, // rgb2(255, 163, 0),     // 9	orange
      0xE2FF, // rgb2(255, 236, 39),    // 10	yellow
      0xE3F0, // rgb2(0, 228, 54),      // 11 green
      0xAFF2, // rgb2(41, 173, 255),    // 12 blue
      0x79F8, // rgb2(131, 118, 156),   // 13 lavender
      0x7AFF, // rgb2(255, 119, 168),   // 14 pink
      0xCAFF, // rgb2(255, 204, 170),   // 15 light-peach
      0x11F2, // rgb2(41, 24, 20),    // 128 	brownish-black
      0x13F1, // rgb2(17, 29, 53),    // 129 	darker-blue
      0x23F4, // rgb2(66, 33, 54),    // 130 	darker-purple
      0x55F1, // rgb2(18, 83, 89),    // 131 	blue-green
      0x22F7, // rgb2(116, 47, 41),   // 132 	dark-brown
      0x33F4, // rgb2(73, 51, 59),    // 133 	darker-grey
      0x87FA, // rgb2(162, 136, 121), // 134 	medium-grey
      0xE7FF, // rgb2(243, 239, 125), // 135 	light-yellow
      0x15FB, // rgb2(190, 18, 80),   // 136 	dark-red
      0x62FF, // rgb2(255, 108, 36),  // 137 	dark-orange
      0xE2FA, // rgb2(168, 231, 46),  // 138 	lime-green
      0xB4F0, // rgb2(0, 181, 67),    // 139 	medium-green
      0x5BF0, // rgb2(6, 90, 181),    // 140	true-blue
      0x46F7, // rgb2(117, 70, 101),  // 141 	mauve
      0x65FF, // rgb2(255, 110, 89),  // 142 	dark-peach
      0x98FF  // rgb2(255, 157, 129), // 143 	peach
  };
  static array<uint_fast8_t, (uint_fast8_t)16> draw_palette = {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  static array<uint_fast8_t, (uint_fast8_t)16> screen_palette = {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  static array<uint_fast8_t, (uint_fast8_t)16> secondary_palette = {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  static array<uint_fast8_t, (uint_fast8_t)16> transparency_palette = {
      0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  static bool high_color_mode = true;
  static uint_fast8_t dontmap = 3; // seems like color 3 is never remapped (Alpha 48)
  static uint_fast8_t berries = 1;
  static bool sound = true;
  static bool swapped_buttons = false;

  auto mountain = buffer(95, 48, mountaindata);
  auto celeste = buffer(128, 64, spritedata);

  const int picowidth = 136; // workaround for
                             // screen shake issue, should be 128
  color_t _fdp[picowidth * picowidth] __attribute__((aligned(4))) = {};
  static buffer_t *PICO8SCREEN = buffer(picowidth, picowidth, _fdp);

  // copy the source over the destination only if source color index has
  // opacity set in transparency palette
  // copy using draw palette ignorierung actual rgb values
  void SPRITE(color_t *ps, int32_t so, int32_t ss, color_t *pd, uint32_t c)
  {
    if (!high_color_mode)
    {
      while (c--)
      {
        color_t s = *(ps + (so >> 16));
        auto index = (s >> 4) & 0x0F;

        // copy if alpha component in transparency palette is opaque
        if (transparency_palette[index] != 0)
        {
          *pd = system_palette[draw_palette[index]];
        }

        // step destination and source
        pd++;
        so += ss;
      }
    }
    else
    {
      while (c--)
      {
        color_t s = *(ps + (so >> 16));
        auto index = (s >> 4) & 0x0F;
        if (index == dontmap)
        {
          // copy if not black
          if ((s & 0xFF0F) != 0x0000)
          {
            *pd = s;
          }
        }
        else
        {
          // copy if alpha component in transparency palette is opaque
          if (transparency_palette[index] != 0)
          {
            *pd = system_palette[draw_palette[index]];
          }
        }

        // step destination and source
        pd++;
        so += ss;
      }
    }
  }

  // copy using draw palette ignorierung actual rgb values
  void PALETTE(color_t *ps, int32_t so, int32_t ss, color_t *pd, uint32_t c)
  {
    if (!high_color_mode)
    {
      while (c--)
      {
        color_t s = *(ps + (so >> 16));
        auto index = (s >> 4) & 0x0F;

        *pd = system_palette[draw_palette[index]];

        // step destination and source
        pd++;
        so += ss;
      }
    }
    else
    {
      while (c--)
      {
        color_t s = *(ps + (so >> 16));
        auto index = (s >> 4) & 0x0F;
        if (index == dontmap)
        {
          *pd = s;
        }
        else
        {
          *pd = system_palette[draw_palette[index]];
        }

        // step destination and source
        pd++;
        so += ss;
      }
    }
  }

  // copy using screen palette ignorierung actual rgb values
  // and converting to normal rgba values for other picosystem effects
  void CONVERT(color_t *ps, int32_t so, int32_t ss, color_t *pd, uint32_t c)
  {
    if (!high_color_mode)
    {
      while (c--)
      {
        color_t s = *(ps + (so >> 16));
        auto index = (s >> 4) & 0x0F;

        *pd = system_palette2[screen_palette[index]];

        // step destination and source
        pd++;
        so += ss;
      }
    }
    else
    {
      while (c--)
      {
        color_t s = *(ps + (so >> 16));
        auto index = (s >> 4) & 0x0F;

        if (index == dontmap)
        {
          *pd = s | 0x00F0;
        }
        else
        {
          *pd = system_palette2[screen_palette[index]];
        }

        // step destination and source
        pd++;
        so += ss;
      }
    }
  }

  color_t getCurrentPencolor()
  {
    return _pen;
  }

  void cls(uint32_t color = 0)
  {
    auto lastpencolor = getCurrentPencolor();
    pen(system_palette[color]);
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

  // spr implementation.
  void spr(int32_t spriteindex, int32_t x, int32_t y, int32_t cols, int32_t rows, bool flipx, bool flipy)
  {
    blend(SPRITE);
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

    if (berries == 1)
    {
      if (spriteindex == 26)
      {
        spritesheet(mountain);
        sprite(0, x, y, cols, rows, 8 * cols, 8 * rows, flags);
        spritesheet(celeste);
      }
      else if (spriteindex == 28)
      {
        spritesheet(mountain);
        sprite(1, x, y, cols, rows, 8 * cols, 8 * rows, flags);
        spritesheet(celeste);
      }
      else
      {
        sprite(spriteindex, x, y, cols, rows, 8 * cols, 8 * rows, flags);
      }
    }
    else
    {
      sprite(spriteindex, x, y, cols, rows, 8 * cols, 8 * rows, flags);
    }
    blend(PALETTE);
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
      draw_palette[i] = i;
      screen_palette[i] = i;
      secondary_palette[i] = i;
      transparency_palette[i] = i == 0 ? 0 : 1;
    }
  }

  void pal(uint_fast8_t c0, uint_fast8_t c1, uint_fast8_t p = 0)
  {
    switch (p)
    {
    case 0:
    default:
      draw_palette[c0] = c1;
      draw_palette[c1] = c0;
      break;
    case 1:
      if (c1 >> 4 == 0)
      { // normal palette
        screen_palette[c0] = (c1 && 0x0F);
      }
      else if (c1 >> 4 == 1)
      { // secret palette
        screen_palette[c0] = (c1 && 0x0F) + 16;
      }
      break;
    case 2:
      secondary_palette[c0] = c1;
      secondary_palette[c1] = c0;
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
    pen(system_palette[c]);
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
    pen(system_palette[c]);
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
    blend(SPRITE);
    for (auto x = cell_x; x < cell_w + cell_x; x++)
    {
      for (auto y = cell_y; y < cell_h + cell_y; y++)
      {
        auto tileindex = map_data[x + y * 128];
        auto masked = sprite_flags[tileindex] | layers;
        if (layers == 0 || masked == sprite_flags[tileindex])
        {
          sprite(tileindex, sx + (x * 8 - cell_x * 8), sy + (y * 8 - cell_y * 8));
        }
      }
    }
    blend(PALETTE);
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
      pen(system_palette[c.value()]);
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
      pen(system_palette[c.value()]);
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
      pen(system_palette[c.value()]);
    }
    fcircle(x, y, r);
    pen(color); // restore previous pen color
  }

  static void circfill(number x, number y, number r, optional<number> c)
  {
    circfill((int)x, (int)y, (int)r, (optional<int>)c);
  }

  // no support for the P8SCII special control yet
  static void print(string str, int32_t x, int32_t y, optional<uint32_t> c)
  {
    auto color = getCurrentPencolor();

    if (c.has_value())
    {
      pen(system_palette[c.value()]);
    }
    text(str, x, y);
    pen(color); // restore previous pen color
  }

  static int gettextwidth(string str)
  {
    auto width = 0;
    for (std::size_t i = 0; i < str.size(); i++)
    {
      char c = str[i];
      width += _font[(c - 32) * 9];
    }
    return width;
  }

  static void center(string str, int32_t y, optional<uint32_t> c)
  {
    auto color = getCurrentPencolor();

    if (c.has_value())
    {
      pen(system_palette[c.value()]);
    }
    text(str, 64 - (gettextwidth(str) / 2), y);
    pen(color); // restore previous pen color
  }

  static void center(string str, number y, optional<number> c)
  {
    center(str, (int)y, (optional<int>)c);
  }

  static void print(string str, number x, number y, optional<number> c)
  {
    print(str, (int)x, (int)y, (optional<int>)c);
  }

  // no support for the P8SCII special control yet
  static void print(string str, optional<uint32_t> c)
  {
    auto color = getCurrentPencolor();

    if (c.has_value())
    {
      pen(system_palette[c.value()]);
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

  // The intention of the pico8 compatibility layer is to aid in the porting
  // of pico-8 carts that can be enhanced even further, not to be an automatic
  // pico-8 runtime.
  void peek(int32_t address)
  {
  }

  static uint_fast8_t getVolume(uint_fast8_t v)
  {
    return ceil(100 / 7 * v);
  }

  static float getPitch(uint_fast8_t v)
  {
    return 440.f * exp2(((float)v - 33.f) / 12.f);
  }

  static uint_fast16_t getDuration(uint_fast8_t d)
  {
    return ceil((float)d * measure);
  }

  static voice_t getVoice(waveform wave, effect fx, uint_fast8_t speed)
  {
    auto noise = 0;
    auto a = 0, d = 0, s = 0, r = 0;
    if (wave == waveform::noise)
    {
      noise = 100;
    }
    if (fx == effect::fade_in)
    {
      a = getDuration(speed) / 2;
      s = getDuration(speed) / 2;
    }
    else if (fx == effect::fade_out)
    {
      s = getDuration(speed) / 2;
      r = getDuration(speed) / 2;
    }
    else
    {
      s = getDuration(speed);
    }
    return voice(a, d, s, r, 0, 0, 0, noise, 0);
  }

  static void internalsfx(uint32_t n, uint32_t channel = 0, uint32_t offset = 0, uint32_t length = 255)
  {
    auto pattern = patterns.at(n);
    for (auto note : pattern.notes)
    {
      // Create voice
      auto voice = getVoice(note.wave, note.fx, pattern.speed);
      // Play voice
      play(voice, getPitch(note.pitch), getDuration(pattern.speed), getVolume(note.volume));
      // Wait
      sleep_ms(getDuration(pattern.speed));
    }
    // play empty note to silence audio
    play(voice(0, 0, 100, 0, 0, 0, 0, 0, 0), 0, 100, 100);
  }

  static void launchsfx()
  {
    uint32_t n = multicore_fifo_pop_blocking();
    internalsfx(n);
  }

  // int sfxqueue;
  static void sfx(uint32_t n, uint32_t channel = 0, uint32_t offset = 0, uint32_t length = 255)
  {
    if (sound == true)
    {
      multicore_reset_core1();
      multicore_launch_core1(launchsfx);
      multicore_fifo_push_blocking(n);
    }
  }

  // music will not be implemented because it would sound like crap anyway
  // with the barely audible piezo speaker,  only one channel and waveform
  void music(int32_t n, uint32_t fade_len = 0, uint32_t channel_mask = 15)
  {
  }

  void hudDrawing(bool active)
  { // inversly adjust the camera for drawing hud elements so they stay in the
    // same place while moving the viewport
  }

  void restoreSettingsFromFlash()
  {
    if ((uint8_t)flash_target_contents[0] == magicvalue)
    { // we have written before and can restore settings
      if ((uint8_t)flash_target_contents[1] == 0)
      {
        sound = false;
      }
      else if ((uint8_t)flash_target_contents[1] == 1)
      {
        sound = true;
      }
      else
      {
        sound = true;
      }
      if ((uint8_t)flash_target_contents[2] == 0)
      {
        berries = 0;
        high_color_mode = false;
      }
      else if ((uint8_t)flash_target_contents[2] == 1)
      {
        berries = 1;
        high_color_mode = true;
      }
      else
      {
        berries = 1;
        high_color_mode = true;
      }
    }
    else
    {
      sound = true;
      berries = 1;
      high_color_mode = true;
    }
  }

  void writeSettingsToFlash()
  {
    save_data[0] = magicvalue;
    save_data[1] = (uint8_t)sound;
    save_data[2] = (uint8_t)berries;
    multicore_reset_core1();
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE * 1);
    flash_range_program(FLASH_TARGET_OFFSET, save_data, FLASH_PAGE_SIZE * 1);
    restore_interrupts(ints);
  }

  void init(bool swapped_buttons = false)
  {
    restoreSettingsFromFlash();
    font(-1, -1, -1, _minimal_font);
    swapped_buttons = swapped_buttons;
    // set drawing region to 128x128 (visible only 120x120 controlled by system_offset and hud_offset)
    clip(0, 0, 128, 128);

    target(PICO8SCREEN);

    blend(PALETTE);
    spritesheet(celeste);
    pal();
  }
}