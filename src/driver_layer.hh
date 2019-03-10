#ifndef DRIVER_LAYER_HH
#define DRIVER_LAYER_HH

#include <vector>

#include "keymap.hh"
#include "layer.hh"
#include "packet.hh"

using std::vector;

// This will contain one rgb int for each valid keycode.
struct rgb_frame {
  // 528 bytes, 132 keys
  // All default to 0 unless set.
  unsigned int keys[MAX_KEYCODE];

  rgb_frame();
  void setkey(keycodes key, uint32_t rgb);
  int size() const { return MAX_KEYCODE; }
  void clear();
};

// Sequence of rgb light frames
typedef vector<rgb_frame> rgb_lights;

//// These calls create sets of light frames for different patterns.
// Build a calculator light program.
rgb_lights make_calc();
rgb_lights make_trail();



vector<packet> driver_light_program(const rgb_lights& framelist);



// Map from keycode to driver mode light program position.  This is inversion
// of drv_light_pos.

// Generally the position of keys in the driver light map are by physical
// position on the keyboard row by row.
extern int drv_light_assign[MAX_KEYCODE];


// Similar to rgb_assign, this maps keycode to position in a driver
// keymap.
extern int drv_keymap_assign[MAX_KEYCODE];

// Build a driver mode keymap program
vector<packet> driver_keymap_program(keymap& kmap);

#endif
