#ifndef CUSTOM_LAYER_HH
#define CUSTOM_LAYER_HH

// Custom layer programming
//
// Custom layer is different from driver layer.  It seems all bits have to be
// sent, at least Windows driver does so.

#include <cmath>
#include <vector>

#include "keymap.hh"
#include "packet.hh"
#include "layer.hh"


// A step is either an action or a delay.  Actions have key and down, with
// duration 0.  Delay has nonzero duration.
struct cus_macro_step {
  int duration;			// duration in milliseconds
  keycodes key;			// key/button action for this step
  bool down;			// true down, false up
};

struct cus_macro {
  int id;			// id to connect macro with keymap
  keycodes key;			// key the macro is assigned to
  int play_mode;		// 1 - auto stop
				// 2 - release stop
				// 3 - press again to stop
  int count;			// how many times to play the macro
  std::vector<cus_macro_step> steps;	// macro sequence
  cus_macro() : id(-1), play_mode(1), count(1) {}
  size_t size() const { return steps.size(); }
};

// One animation frame of a custom light program.  This is just a bitmask of
// keys that allow lighting to shine through.
struct animation_frame {
  union {
    unsigned char data[26];
    struct {
      uint32_t header;
      unsigned char keymap[22];        // keymap is packed into 22 bits
    };
  };

  animation_frame();
  
  // Turn on key for this frame.
  void enable(keycodes key);
  // Turn off all keys for this frame.
  void clear();
};


struct pattern_frame {
  union {
    unsigned char data[32];
    struct {
      uint16_t header;
      unsigned char keymap[22];        // keymap is packed into 22 bits
      uint8_t R;		       // red 0-255
      uint8_t G;		       // green 0-255
      uint8_t B;		       // blue 0-255
      uint8_t dummy;
      // type is monochrome, rgb cycle, breathing
      uint8_t inv_duration;                // inverse duration
      uint8_t dummy2;

      // Gap frames between breathing pattern cycles.
      uint8_t gapcode;
      uint8_t dummy3;
    };
  };

  // Set up monochrome by default
  pattern_frame() {
    header = htole16(0x2000);
    memset(data+2, 0, 30);
  }

  // Turn on key for this frame.
  void enable(keycodes key);
  // Turn off all keys for this frame.
  void clear();
  // Turn on all keys for this frame.
  void all();

  // Set lighting for keys in keymap to be fixed at RGB.
  void monochrome(uint8_t red, uint8_t green, uint8_t blue) {
    data[0] = 0;		// Enable monochrome
    R = red; G = green; B = blue;
    inv_duration = 0;
    gapcode = 0;
    dummy3 = 0;
  }

  // Set lighting to RGB cycle starting with specified value.  Duration
  // specifies the speed of changing through the colors.
  void rgb_cycle(uint8_t red, uint8_t green, uint8_t blue, int duration) {
    data[0] = 1;		// Enable RGB cycle
    // dur 00 79 00
    R = red; G = green; B = blue;
    // Actual duration value is inverse of specified value
    // Matching the windows driver
    int speed = floor(360 / duration);
    inv_duration = speed;
    gapcode = 0;
    dummy3 = 0;
  }

  // Set lighting for keys in keymap to be breathing pattern with duration
  // being how long the cycle lasts and gap frames between cycles.
  void breathe(uint8_t red, uint8_t green, uint8_t blue, int duration, int gap) {
    data[0] = 2;		// Enable breathing
    R = red; G = green; B = blue;
    // dur 00 gap 00
    // Actual duration value is inverse of specified value
    // Matching the windows driver
    int speed = floor(100 / duration);
    inv_duration = speed;
    gapcode = gap;
  }
};

struct custom_light_prog {
  std::vector<animation_frame> aframes;
  std::vector<pattern_frame> lframes;
};

// Construct a light program with 1 animation frame with all keys disabled,
// one monochrome light frame with no keys enabled.  This is useful if setting
// a flashlight with no other lighting.
custom_light_prog no_lights();

// Complete program for a layer
struct custom_layer_prog {
  keymap kmap;
  custom_light_prog lights;
};

// How do I get mappings into this function to be converted to a program?
struct program;
std::vector<packet> custom_program(char layer, program& cus_prog);



#endif
