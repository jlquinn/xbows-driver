#ifndef DRIVER_LAYER_HH
#define DRIVER_LAYER_HH

#include <vector>

#include "keymap.hh"
#include "packet.hh"

using std::vector;

// This will contain one rgb int for each valid keycode.
struct drv_light_frame {
  // 528 bytes, 132 keys
  // All default to 0 unless set.
  unsigned int keys[MAX_KEYCODE];

  drv_light_frame();
  void setkey(keycodes key, uint32_t rgb);
  int size() const { return MAX_KEYCODE; }
  void clear();
};

//// These calls create sets of light frames for different patterns.
// Build a calculator light program.
vector<drv_light_frame> make_calc();
vector<drv_light_frame> make_trail();



// The program calls init_driver_mode once.  Then it can create and send light
// programs as needed.

// This is the 2 packets needed to get the attention of the keyboard in driver
// mode.
extern vector<packet> drv_attn;

// Set up default settings to talk to keyboard in driver mode.
void init_driver_mode();

vector<packet> light_program(const vector<drv_light_frame>& framelist);



// Map from keycode to driver mode light program position.  This is inversion
// of drv_light_pos.

// Generally the position of keys in the driver light map are by physical
// position on the keyboard row by row.
extern int drv_light_assign[MAX_KEYCODE];


// This contains the full set of key positions for a driver keymap
// XXX 120 needs to become a constant somewhere
const int MAX_KEYMAP=120;
struct drv_keymap {
  uint32_t keys[MAX_KEYMAP];
  int size() const { return MAX_KEYMAP; }
};

// Similar to drv_light_assign, this maps keycode to position in a driver
// keymap.
extern int drv_keymap_assign[MAX_KEYCODE];

// Build a driver mode keymap program
vector<packet> keymap_program();

#endif
