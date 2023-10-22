#include <endian.h>
#include <cstring>

#include "layer.hh"

// Map from keycode to position in drv_keymap
int keymap_assign[MAX_KEYCODE] = {
  // Position 0 is empty
  0xff,				  // K_NONE
  // Letters
  10, 11, 12, 13,			  // A B C D packet 1
  14, 15, 16, 17, 18, 19, 20, 21,	  // E F G H I J K L packet 2
  22, 23, 24, 25, 26, 27,		  // M N O P Q R packet 2
  28, 29, 30, 31, 32, 33, 34, 35,	  // S T U V W X Y Z packet 3
  // Numbers
  36, 37, 38, 39, 40, 41,		  // 1 2 3 4 5 6 packet 3
  42, 43, 44, 45,			  // 7 8 9 0 packet 4
  // Other printing chars
  51, 52, 53, 54, 55,		// -_ += [{ ]} \| packet 4
  56, 57, 59,			// ;: '" ,<
  60, 61, 58,			// .> /? `~
  // Nonprinting keys
  47, 49, 62,			// Esc Tab Capslock
  63, 64, 65, 66, 67, 68, 69,	// F1 F2 F3 F4 F5 F6 F7
  70, 71, 72, 73, 74,		// F8 F9 F10 F11 F12
  46, 113, 			// REnter MEnter
  3, 115, 7,			// LShift MShift RShift
  2, 114, 6,			// LControl MControl RControl
  4, 8,				// LAlt RAlt
  112, 48,			// MBackspace RBackspace
  50, 116,			// LSpace RSpace
  5,				// Windows
  80, 83,			// PageUp PageDown
  85, 84, 87, 86,		// Left Right Up Down
  75, 81,			// PrtScrn Delete
  0xff,				// XBows
  // Numpad
  0xff, 0xff, 0xff, 0xff, 	// Numlock NPSlash NPStar NPEnter
  0xff, 0xff, 0xff, 0xff,	// NP1 NP2 NP3 NP4
  0xff, 0xff, 0xff, 0xff,	// NP5 NP6 NP7 NP8
  0xff, 0xff, 0xff, 0xff, 0xff,	// NP9 NP0 NP. NP- NP+
  // Media
  0xff, 0xff, 0xff, 0xff,	// Play Pause Stop Last
  0xff, 0xff, 0xff, 0xff,	// Next VolUp VolDown Mute
  // Mouse
  0xff, 0xff, 0xff, 0xff, 0xff,	// LClick MClick RClick Back Forward
  // Sys/net
  0xff, 0xff, 0xff, 0xff,	// NetBack NetFwd NetRefresh NetCollection
  0xff, 0xff, 0xff, 0xff,	// NetHome NetEmail NetComp NetCalc
  0xff, 0xff, 0xff,		// NetCopy NetPaste NetPrtScrn
  // Not on xbows kbd
  0xff, 0xff, 0xff,		// Home End Insert
  // 102nd (\|)
  0xff,
  // Function key
  0xff				// Fn
};


void keymap::assign(keycodes key, keycodes emits) {
  // XXX CHECK THIS!  It should probably be htole16
  if(keyid[emits] > 0xffff)
    keys[keymap_assign[key]] = htobe32(keyid[emits]) | 0x02000000;
  else
    keys[keymap_assign[key]] = htobe16(keyid[emits]) | 0x02000000;
}

void keymap::assign_macro(keycodes key, int n) {
  keys[keymap_assign[key]] = htole16(n) | 0x0a010000;
}

void keymap::assign_layer_switch(keycodes key, layercode layer) {
  keys[keymap_assign[key]] = htole16(layer) | 0x0a070000;
}


// Set up the default keymap.  Gets called when a keymap is created.
// This code is identical to the default driver keymap.
void keymap::clear() {
  // Iterate through table and assign values to kmap

  // I need to assign LCtrl position 3 with value 01000002, etc.
  // Unused/unknown slots get 0xffffffff.

  // How do I track the position mapping?  drv_keymap_assign.

  // There is a keyid map that is shared between driver and custom.
  // That gives me the value to write into the slot.

  // First fill with 0xff for unused/unknown slots
  memset(keys, 0xff, MAX_KEYMAP * 4);

  // Now set specific key slots
  for (int i=0; i < MAX_KEYCODE; i++) {
    uint16_t id = keyid[i];	// 2 of the 4 bytes we need
    int pos = keymap_assign[i];

    if (id == 0xffff) continue;
    if (pos == 0xff) continue;

    // Each value looks like id 00 02
    keys[pos] = 0x02000000 | (uint32_t)htobe16(id);
  }

}


bool keymap::is_macro(keycodes key) {
  uint32_t val = keys[keymap_assign[key]];
  uint32_t masked = val &= 0xffff0000;
  return masked == 0x0a010000;
}

int keymap::macro(keycodes key) {
  uint32_t val = keys[keymap_assign[key]];
  return val &= 0xffff;
}

bool keymap::is_layer_switch(keycodes key) {
  uint32_t val = keys[keymap_assign[key]];
  uint32_t masked = val &= 0xffff0000;
  return masked == 0x0a070000;
}

// Returns 2, 3, 4 for custom, 1 for driver
int keymap::layer(keycodes key) {
  uint32_t val = keys[keymap_assign[key]];
  return val &= 0xffff;
}

