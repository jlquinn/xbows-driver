#ifndef BASE_LAYER_HH
#define BASE_LAYER_HH

#include "keymap.hh"

// Layer codes
enum layercode { NONE=0, STATIC=1, CUSTOM1=2, CUSTOM2=3, CUSTOM3=4, DRIVER=5 };

// This contains the full set of key positions for a driver keymap
const int MAX_KEYMAP=120;
extern int keymap_assign[];
struct keymap {
public:
  uint32_t keys[MAX_KEYMAP];
  bool dflt;			// true if this is the default keymap

  int size() const { return MAX_KEYMAP; }

  // This function tells keyboard to send emits when key is pressed.
  void assign(keycodes key, keycodes emits);
  // This function tells keyboard to play the n'th macro when key is pressed.
  void assign_macro(keycodes key, int n);
  // This function tells keyboard to switch to layer n when key is pressed.  N
  // is 1 for driver layer, 2-4 for custom layers.
  void assign_layer_switch(keycodes key, layercode layer);

  void clear();

  // Return true if this keymap is the default map
  bool is_default() { return dflt; }

  // Return true if key is assigned a macro
  bool is_macro(keycodes key);
  // Return macro index for key
  int macro(keycodes key);

  // Return true if key is assigned a layer switch
  bool is_layer_switch(keycodes key);
  // Return layer this key switches to
  // Returns 2, 3, 4 for custom, 1 for driver
  int layer(keycodes key);

  keymap() { clear(); }
};


#endif
