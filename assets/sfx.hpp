#include <vector>
using namespace std;
namespace pico8 {
enum class waveform {
    sine = 0,
    triangle = 1,
    sawtooth = 2,
    long_square = 3,
    short_square = 4,
    ringing = 5,
    noise = 6,
    ringing_sine = 7,
    custom_0 = 8,
    custom_1 = 9,
    custom_2 = 10,
    custom_3 = 11,
    custom_4 = 12,
    custom_5 = 13,
    custom_6 = 14,
    custom_7 = 15
  };

  enum class effect {
    none = 0,
    slide = 1,
    vibrato = 2,
    drop = 3,
    fade_in = 4,
    fade_out = 5,
    arp_fast = 6,
    arp_slow = 7
  };

  struct note {
    uint_fast8_t pitch;
    waveform wave;
    uint_fast8_t volume;
    effect fx;
  };

  struct pattern {
    uint_fast8_t editor;
    uint_fast8_t speed;
    uint_fast8_t start;
    uint_fast8_t end;
    vector<note> notes;
  };
}