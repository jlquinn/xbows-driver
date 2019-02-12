// Custom layer programming
//
// Custom layer is different from driver layer.  It seems all bits have to be
// sent, at least Windows driver does so.

#include <vector>

#include "keymap.hh"
#include "packet.hh"

struct cus_anim_frame {
  unsigned char keymap[22];	// keymap is packed into 22 bits
};

struct cus_light_frame {
  unsigned char keymap[22];	// keymap is packed into 22 bits
  unsigned char R,G,B;
  uint8_t pattern;		// type is monochrome, rgb cycle, breathing
  uint8_t duration;		// inverse duration
  uint8_t gap;			// gap between cycles for breathing pattern.
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

