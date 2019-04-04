#ifndef LAYER_HH
#define LAYER_HH

#include "keymap.hh"

// This contains the full set of key positions for a driver keymap
const int MAX_KEYMAP=120;
extern int keymap_assign[];
struct keymap {
public:
  uint32_t keys[MAX_KEYMAP];

  int size() const { return MAX_KEYMAP; }

  // This function tells keyboard to send emits when key is pressed.
  void assign(keycodes key, keycodes emits);

  void clear();

  keymap() { clear(); }
};


#endif
