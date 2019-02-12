// Custom layer programming
//
// Custom layer is different from driver layer.  It seems all bits have to be
// sent, at least Windows driver does so.

#include <cmath>
#include <vector>

#include "keymap.hh"
#include "packet.hh"

struct cus_anim_frame {
  union {
    unsigned char data[26];
    struct {
      uint32_t header;
      unsigned char keymap[22];        // keymap is packed into 22 bits
    };
  };

  cus_anim_frame() {
    header = htole32(0x00160003);
    memset(keymap, 0, 22);
  }
};


 struct cus_light_frame {
  union {
    unsigned char data[32];
    struct {
      uint16_t header;
      unsigned char keymap[22];        // keymap is packed into 22 bits
      uint8_t R;
      uint8_t G;
      uint8_t B;       // colors
      uint8_t dummy;
      // type is monochrome, rgb cycle, breathing
      uint8_t pattern;
      uint8_t inv_duration;                // inverse duration
      // Gap between cycles for breathing pattern.  0x79 for mono and rgb cycle.
      uint8_t gapcode;
      uint8_t dummy2;
    };
  };

  // Set up monochrome by default
  cus_light_frame() {
    header = htole16(0x2000);
    memset(data+2, 0, 30);
    gapcode = 0x79;                        // monochrome terminator
  }

  // Set lighting for keys in keymap to be fixed at RGB.
  void monochrome(uint8_t red, uint8_t green, uint8_t blue) {
    data[0] = 0;		// Enable monochrome
    R = red; G = green; B = blue;
    // 00007900
    pattern = inv_duration = 0;
    gapcode = 0x79;    
  }

  // Set lighting for keys in keymap to be breathing pattern with duration
  // being how long the cycle lasts and gap frames between cycles.
  void breathe(uint8_t red, uint8_t green, uint8_t blue, int duration, int gap) {
    data[0] = 1;		// Enable breathing
    R = red; G = green; B = blue;
    // dur 00 gap 00
    // Actual duration value is inverse of specified value
    // Matching the windows driver
    int speed = floor(100 / duration);
    pattern = speed;
    inv_duration = 0;
    gapcode = gap;
  }

  // Set lighting to RGB cycle starting with specified value.  Duration
  // specifies the speed of changing through the colors.
  void rgb_cycle(uint8_t red, uint8_t green, uint8_t blue, int duration) {
    data[0] = 1;		// Enable RGB cycle
    // dur 00 79 00
    R = red; G = green; B = blue;
    // Actual duration value is inverse of specified value
    // Matching the windows driver
    int speed = floor(360/duration);
    pattern = speed;
    inv_duration = 0;
    gapcode = 0x79;
  }
 };

struct custom_light_prog {
  std::vector<cus_anim_frame> aframes;
  std::vector<cus_light_frame> lframes;
};


std::vector<packet> custom_program(char layer);


// This contains the full set of key positions for a driver keymap
// XXX 120 needs to become a constant somewhere
const int MAX_KEYMAP=120;
struct cus_keymap {
  uint32_t keys[MAX_KEYMAP];
  int size() const { return MAX_KEYMAP; }

  // This function tells keyboard to send emits when key is pressed.
  void assign(keycodes key, keycodes emits);

  void clear();

  cus_keymap() { clear(); }
};

// Similar to drv_light_assign, this maps keycode to position in a driver
// keymap.

extern int cus_keymap_assign[MAX_KEYCODE];

